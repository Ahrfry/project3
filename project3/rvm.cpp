#include "rvm.h"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>


using namespace std;

//global pointer to log file
const char *log_file;

//<segname , segment_t>
static std::map<std::string, segment_t *> mapsegname;

//map the seg pointer to the segment_t
static std::map<void*, segment_t *> map_pt_to_seg;

//map mod regions to their transactions
static map<trans_t, list<region_header_t *> *> map_trans_to_regions;

int verbose = 1;

void print_str(string data , int flag){
	if(flag != -1){
		cout<<data;
	}
}

rvm_t rvm_init(const char *dir){
	
	rvm_t rvm;
    	struct stat sb;
	
	char *log = (char *)malloc(256);
	//set to 0 all 256	
        memset(log , 0 , 256);
	//construct path to log file
	strcat(log, dir);
        strcat(log,"/rvm.log");                                                         
	
	log_file = log;
	
	//std::cout<<"Init log file path at " <<log_file <<std::endl;

	//if dir exists 
    	if (stat(dir, &sb) == 0 && S_ISDIR(sb.st_mode)){
        	rvm.dir = dir;	
    	}else{
        	mkdir(dir, S_IRWXU | S_IRWXG );
		rvm.dir = dir;
	}

		
	return rvm;
}


void *rvm_map(rvm_t rvm , const char *segname , int size_to_create){
	
	std::string seg_name_str = segname;

	segment_t *segment = (segment_t *) malloc (sizeof(segment_t)); 
	
	//check if segname is already mapped
	if ( mapsegname.find(seg_name_str) == mapsegname.end() ) {
		print_str("seg not found \n" , 1);
		
		//create segment structure
		segment->name = segname;
		segment->size = size_to_create;
		segment->load =  (void *) malloc(size_to_create);
		segment->load_backup =  (void *) malloc(size_to_create);
		segment->trans_id = -1; //starts as not being used;
		memset(segment->load,0,size_to_create);
		memset(segment->load_backup,0,size_to_create);
		char *path_to_segment = (char *)malloc(256);
		
		strcat(path_to_segment, rvm.dir);
		strcat(path_to_segment, "/");
		strcat(path_to_segment, segname);

		//std::cout<<path_to_segment<<std::endl;	
		
		struct stat sb;
		if((stat(path_to_segment, &sb) == 0)){
			print_str("File exists \n" , verbose);
			if(sb.st_size != size_to_create){
				truncate(path_to_segment , size_to_create);
			}
			
			rvm_truncate_log(rvm);
			
			FILE *fp = fopen(path_to_segment, "r");
			fread(segment->load, 1, size_to_create, fp);
			fread(segment->load_backup, 1, size_to_create, fp);
					
		}else{
			FILE *fp = fopen(path_to_segment, "w");
			fclose(fp);
			truncate(path_to_segment, size_to_create);	
			print_str("File does not exists \n" , 1);
		}

	} else {
		  
		string str = "Segment " + seg_name_str + " already mapped \n";
		print_str(str , 1);
		return NULL;
	}

	//map name to segment
	mapsegname.insert(pair<std::string,segment_t *>(std::string(segname),segment));
	//map pointer to segment
	map_pt_to_seg.insert(pair<void *,segment_t *>(segment->load,segment));
	
	return segment->load;
	
}

streampos file_size( const char* file_path ){

    std::streampos fsize = 0;
    std::ifstream file( file_path, std::ios::binary );

    fsize = file.tellg();
    file.seekg( 0, std::ios::end );
    fsize = file.tellg() - fsize;
    file.close();

    return fsize;
}

void rvm_truncate_log(rvm_t rvm){
	
	vector<string> list;
  	string line;
	
	char *log = (char *)malloc(256);
	//set to 0 all 256	
        memset(log , 0 , 256);
	//construct path to log file
	strcat(log, rvm.dir);
        strcat(log,"/rvm.log");
	if(file_size(log) > 0){	
		//open log file for reading
		ifstream myfile (log);
		
		if (myfile.is_open())
		{
			//push line by line into list
			while ( getline (myfile,line) ){
				list.push_back(line);
			}
		}else{
			 cout << "Unable to open log file";
		}

		//check	last line in file is trans_end
		string trans_end = list.at(list.size() -1 );
		if(trans_end.compare("trans_end") == 0){
			//cout<<"Strings are equal"<<endl;
			int num_segs = stoi(list.at(2));
			int j = 3;
			for(int i = 0; i < num_segs; i++){	
				
				int offset = stoi(list.at(j+1));
				int data_size = list.at(j+2).length();
				
				char file_path[256];
				memset(file_path, 0, 256);
				strcat(file_path, rvm.dir);
				strcat(file_path, "/");
				strcat(file_path , list.at(j).c_str());
				cout<<file_path <<endl;
				FILE *seg_file = fopen(file_path,"r+");
				//set the offset
				fseek(seg_file, offset , SEEK_SET);
				//write data to file
				fwrite(list.at(j+2).c_str(), 1, data_size , seg_file);
				fflush(seg_file);
				fclose(seg_file);
				j = j + 3;
			}
			

		}else{
			print_str("Transaction aborted before full commit" , 1);
		}
		cout<<log<<endl;

		print_str(log , verbose);		
		print_str("\n" , verbose);
		myfile.close();
	}
	FILE* lfp = fopen(log, "w");
	fclose(lfp);
}

void rvm_destroy(rvm_t rvm, const char *segname){
	
	char file_path[256];
	memset(file_path, 0, 256);
	strcat(file_path, rvm.dir);
	strcat(file_path, "/");
	strcat(file_path , segname);
	remove(file_path);
	//cout<<"Segment name to be destroyed "<<file_path<<endl;
}

trans_t rvm_begin_trans(rvm_t rvm , int numsegs , void **segbases){
	
	
	map<void *, segment_t *>::iterator it;	

	for (int i=0; i< numsegs; i++) {
		//See if segment is mapped
		//cout <<segbases[i]<<endl;	
		it = map_pt_to_seg.find(segbases[i]);
		//if find == end --> pointer not found
		if(it == map_pt_to_seg.end()){
			return -1;
		//if seg is mapped, check if it's not being modified by another trans	
		}else if(it->second->trans_id != -1){//segments default to trans_id -1
			return -1;
		}	
	}		
	//Generate random trans_id	
	trans_t trans_id = rand() % 1000 + 1;
	
	//pass trans_id to all segments to garantee that no other transaction is going to access it
	for (int i=0; i< numsegs; i++) {
		it = map_pt_to_seg.find(segbases[i]);
		it->second->trans_id = trans_id;
		strncpy ((char *) it->second->load_backup, (char *) it->second->load , (int)it->second->size);
	}

	return trans_id;
}


void rvm_about_to_modify(trans_t trans_id , void *segbase , int offset, int size){
	
	//iterator for trans maps	
	map<trans_t, list<region_header_t *> *>::iterator trans_it;
		
	map<void *, segment_t *>::iterator it;	
	it = map_pt_to_seg.find(segbase);
	//check if trans owns seg and if seg is mapped	
	
	region_header_t *region = (region_header_t *) malloc(sizeof(region_header_t));
	if(it->second->trans_id == trans_id && it != map_pt_to_seg.end()){
		
		//create region
		region->size = size;
		region->trans_id = trans_id;
		region->offset = offset;
		region->load_mod = (char *)malloc(size);
		region->segment = it->second;
		memset(region->load_mod , 0 , size);
		memcpy(region->load_mod, (char*)segbase+offset, size);	
		//see if trans is already mapped
	       	trans_it = map_trans_to_regions.find(trans_id);
		if(trans_it == map_trans_to_regions.end()){
			list<region_header_t *> *regions = new list<region_header_t *>;
			regions->push_back(region);
			
			map_trans_to_regions.insert(pair<trans_t , list<region_header_t*> *>(trans_id, regions));
			print_str("Trans not Mapped \n" , verbose);
		}else{
			trans_it->second->push_back(region);
			print_str("Trans mapped \n" , verbose);	
		}	
	}else{
		print_str("region not mapped, or already being modified \n" , verbose);	
	}


}


void rvm_commit_trans(trans_t trans_id){
	map<trans_t, list<region_header_t *> *>::iterator trans_it;
	trans_it = map_trans_to_regions.find(trans_id);
	
	ofstream log(log_file);
	log<<"trans_start"<<endl;
	log<<trans_id<<endl;
	log<<trans_it->second->size()<<endl;	
	
	for(std::list<region_header_t *>::iterator it = trans_it->second->begin(); it!=trans_it->second->end(); it++){
		
		char buff[256];
		memset(buff , 0 , 256);
		strncpy (buff, (char *)(*it)->segment->load , (*it)->size +1);
		strncpy ((char *)(*it)->segment->load_backup, (char *)(*it)->segment->load , (*it)->size +1);
		log << (*it)->segment->name<<endl;
		log << (*it)->offset<<endl;
		log << buff <<endl;
		(*it)->segment->trans_id = -1;
		free(*it);
	}


	log<<"trans_end"<<endl;	
	log.close();	

	/*
	for(std::map<trans_t , region_header_t *>::iterator it = map_trans_to_regions.begin(); it!=map_trans_to_region.end(); it++){
		cout<<"workinnnnnnnnnnnn \n";
	};*/	
		
}

void rvm_abort_trans(trans_t trans_id){
	//Need to set all segment->trans_id to -1 
	map<trans_t, list<region_header_t *> *>::iterator trans_it;
	trans_it = map_trans_to_regions.find(trans_id);
	for(std::list<region_header_t *>::iterator it = trans_it->second->begin(); it!=trans_it->second->end(); it++){
		strncpy((char *)(*it)->segment->load + (*it)->offset, (char *)(*it)->segment->load_backup, (*it)->segment->size +1);
		/*
		char buff[256];
		memset(buff , 0, 256);
		strncpy (buff + 100, (char *)(*it)->segment->load , 12);
		//cout << (*it)->segment->name<<endl;
		//log << (*it)->offset<<endl;
		//cout << buff <<endl;
		ofstream log("test.txt");
		log<< (char *)(*it)->segment->load;
		log.close();*/
	}
}

#include <sys/wait.h>


#define SEGNAME0  "testseg1"
#define SEGNAME1  "testseg2"

#define OFFSET0  10
#define OFFSET1  100

#define STRING0 "hello, world"
#define STRING1 "black agagadrof!"


void proc1() 
{
     rvm_t rvm;
     char* segs[2];
     trans_t trans;
     
     rvm = rvm_init("rvm_segments");

     rvm_destroy(rvm, SEGNAME0);
     rvm_destroy(rvm, SEGNAME1);

     segs[0] = (char*) rvm_map(rvm, SEGNAME0, 1000);
     segs[1] = (char*) rvm_map(rvm, SEGNAME1, 1000);

     trans = rvm_begin_trans(rvm, 2, (void **)segs);

     rvm_about_to_modify(trans, segs[0], OFFSET0, 100);
     strcpy(segs[0]+OFFSET0, STRING0);
     rvm_about_to_modify(trans, segs[1], OFFSET1, 100);
     strcpy(segs[1]+OFFSET1, STRING1);

     rvm_commit_trans(trans);

     abort();
}


void proc2() 
{
     rvm_t rvm;
     char *segs[2];

     rvm = rvm_init("rvm_segments");
     segs[0] = (char*) rvm_map(rvm, SEGNAME0, 1000);
     segs[1] = (char*) rvm_map(rvm, SEGNAME1, 1000);

     if(strcmp(segs[0] + OFFSET0, STRING0)) {
	  printf("ERROR in segment 0 (%s)\n",
		 segs[0]+OFFSET0);
	  exit(2);
     }
     if(strcmp(segs[1] + OFFSET1, STRING1)) {
	  printf("ERROR in segment 1 (%s)\n",
		 segs[1]+OFFSET1);
	  exit(2);
     }

     printf("OK\n");
}


int main(int argc, char **argv) 
{
     int pid;

     pid = fork();
     if(pid < 0) {
	  perror("fork");
	  exit(2);
     }
     if(pid == 0) {
	  proc1();
	  exit(0);
     }

     waitpid(pid, NULL, 0);

     proc2();

     return 0;
}
