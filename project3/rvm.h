#ifndef RVM_H
#define RVM_H

typedef struct _rvm_t{
	const char *dir;
} rvm_t;


typedef int trans_t;

typedef struct _segment_t{
	const char *name;
	void *load;
	void *load_backup;
	int size;
	int trans_id;
} segment_t;

typedef struct _region_header_t{
	trans_t trans_id;
	int offset;
	int size;
	char *load_mod;
	segment_t * segment;
}region_header_t;



typedef struct _trans_header_t{
	int trans_id;
	int num_segs;

}trans_header_t;


rvm_t rvm_init(const char *directory);
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create);
void rvm_unmap(rvm_t rvm, void *segbase);
void rvm_destroy(rvm_t rvm, const char *segname);
trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases);
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size);
void rvm_commit_trans(trans_t tid);
void rvm_abort_trans(trans_t tid);
void rvm_truncate_log(rvm_t rvm);


#endif
