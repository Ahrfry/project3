#ifndef RVM_H
#define RVM_H

typedef struct _rvm_t{
	const char *dir;
} rvm_t;


typedef struct _segment_t{
	const char *name;
	void *load;
	int size;
	int transaction_id;
} segment_t;

#endif
