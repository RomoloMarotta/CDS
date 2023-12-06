#ifndef __SET__
#define __SET__

#define PESSIMISTIC	0	
#define CHAINED		1
#define OPTIMISTIC	2
#define LOCKFREE	3
#define IDEAL           4

#ifndef LOCK_MODE 
#define LOCK_MODE 0
#endif

#define MARK 1ULL


#define lock_t pthread_spinlock_t
#define LOCK pthread_spin_lock
#define UNLOCK pthread_spin_unlock
#define INIT(x) {pthread_spin_init(x, PTHREAD_PROCESS_PRIVATE);}


typedef struct _node{
	struct _node* 
#if LOCK_MODE == LOCKFREE
	volatile 
#endif
	next;
	int key;
	int mark;
	lock_t lock;
	char pad[128-16-sizeof(pthread_spinlock_t)];
}node;


typedef struct _set{
	node *head;
	node *tail;
	pthread_spinlock_t g_lock;
}ll_set;




ll_set* init_ll_set();
bool find(ll_set *set, int key);
bool insert(ll_set *set, int key);
bool delete(ll_set *set, int key);




#endif
