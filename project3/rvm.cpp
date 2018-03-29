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
//global pointer to log file
const char *log_file;

//<segname , segment val>
static std::map<std::string, segment_t *> mapsegname;


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

	//check if segname is already mapped
	if ( mapsegname.find(seg_name_str) == mapsegname.end() ) {
		std::cout<<"seg not found" << std::endl;
		
		//create segment structure
		segment_t *segment = (segment_t *) malloc (sizeof(segment_t)); 
		segment->name = segname;
		segment->size = size_to_create;
		segment->load =  (void *) calloc(size_to_create , size_to_create);
		segment->transaction_id = -1; //starts as not being used;
		
		char *path_to_segment = (char *)malloc(256);
		
		strcat(path_to_segment, rvm.dir);
		strcat(path_to_segment, "/");
		strcat(path_to_segment, segname);

		//std::cout<<path_to_segment<<std::endl;	
		
		struct stat sb;
		if((stat(segname, &sb) == 0)){
			std::cout<<"File exists"<<std::endl;
			if(sb.st_size != size_to_create){
				truncate(path_to_segment , size_to_create);
			}

			//rvm_truncate(rvm);
		}else{
			FILE *fp = fopen(path_to_segment, "w");
			fclose(fp);
			truncate(path_to_segment, size_to_create);	
			std::cout<<"File does not exists"<<std::endl;
		}

	} else {
		  
		std::cout<<"Segment "<< seg_name_str <<" already mapped" << std::endl;
		return NULL;
	}


	
}

using namespace std;

void rvm_truncate(){
	
	vector<string> list;
  	string line;
	//open log file for reading
	ifstream myfile ("test/rvm.log");
  	//	
	if (myfile.is_open())
  	{
    		//push line by line into list
		while ( getline (myfile,line) ){
      			list.push_back(line);
    		}
  	}else{
		 cout << "Unable to open file";
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
			strcat(file_path, "test/");
			strcat(file_path , list.at(j).c_str());
			cout<<file_path <<endl;
			FILE *seg_file = fopen(file_path,"r+");
			//set the offset
			fseek(seg_file, 10 , SEEK_SET);
			//write data to file
			fwrite(list.at(j+2).c_str(), 1, data_size , seg_file);
			fflush(seg_file);
			fclose(seg_file);
			j = j + 3;
		}
		/*	
		char file_path[256];
		memset(file_path, 0, 256);
		strcat(file_path, "test/segment1");
		//open seg file for writing
		FILE *seg_file = fopen(file_path,"r+");
		//set the offset
		fseek(seg_file, 10 , SEEK_SET);
		//write data to file
		fwrite("testing this", 1, 13 , seg_file);
		fflush(seg_file);
		fclose(seg_file);	

		cout<<"number of segs " << seg_num<<endl;*/

	}else{
		cout<<"Transaction aborted before full commit"<<endl;
	}

	/*	
	for(int i=0; i < list.size(); i++){
		cout<<list.at(i) << endl;
	}
	*/
	
    	myfile.close();
}

int main(){
	
	rvm_t rvm = rvm_init("test");
	
	rvm_map(rvm , "segment1" , 1000);
	
	rvm_truncate();	
	
	//std::cout<<rvm.dir<<std::endl;
	
	return 0;




}
