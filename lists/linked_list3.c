#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include "set.h"
#include "set_common.h"


#define PESSIMISTIC	0	
#define CHAINED		1
#define OPTIMISTIC	2
#define LOCKFREE	3

#ifndef LOCK_MODE 
#define LOCK_MODE 0
#endif

#define MARK 1ULL

static inline void*   get_marked_ref(void* ptr) { return (void*) (((unsigned long long)ptr) | MARK);}
static inline void* get_unmarked_ref(void* ptr) { return (void*) (((unsigned long long)ptr) & (~MARK));}
static inline bool     is_marked_ref(void* ptr) { return  (bool) (((unsigned long long)ptr) & MARK);}




node* search(ll_set *set, int k, node** r_node
#if LOCK_MODE == LOCKFREE
, node *new_node
#endif
){
	node *l, *t, *t_next, *t_next_next, *l_next, *prev;
	int loops = 0;
begin:
	t = set->head;
	t_next = set->head->next;
#if LOCK_MODE == CHAINED
	pthread_spin_lock(&t->lock);
#endif
	while(1){
		if(!t->mark && !is_marked_ref(t_next)){ l = t; l_next = t_next;}
		prev = t;
		t = get_unmarked_ref(t_next);
		t_next = t->next;
#if LOCK_MODE == CHAINED
		pthread_spin_lock(&t->lock);
#endif
		if(t == set->tail || 
		(!t->mark && !is_marked_ref(t_next) &&  t->key >= k)
		) break;
#if LOCK_MODE == CHAINED
		pthread_spin_unlock(&prev->lock);
#endif
	}
	*r_node = t;
#if LOCK_MODE == OPTIMISTIC
	pthread_spin_lock(&l->lock);
	pthread_spin_lock(&(*r_node)->lock);
	if(l->mark || (*r_node)->mark) goto retry;
	if(l->next != (*r_node)) l->next = (*r_node);
#endif

#if LOCK_MODE == LOCKFREE
	if(new_node == NULL) if(l_next != (*r_node) && !__sync_bool_compare_and_swap(&l->next, l_next, (*r_node))) { /*printf("FAILED CAS 3 %p %p %p %p %d\n", l, l->next, l_next, *r_node, k);*/ goto begin;}
	if(new_node != NULL){
		new_node->next = *r_node;
		if(!__sync_bool_compare_and_swap(&l->next, l_next, (new_node))) { /*printf("FAILED CAS 3 %p %p %p %p %d\n", l, l->next, l_next, *r_node, k);*/ goto begin;}

	} 
#endif
	return l;
retry:
	pthread_spin_unlock(&l->lock);
	pthread_spin_unlock(&(*r_node)->lock);
	goto begin;
}

bool find(ll_set *set, int key){
	bool res;
	node *r, *l;
#if LOCK_MODE == PESSIMISTIC
	pthread_spin_lock(&set->g_lock);
#endif
	l = search(set, key, &r
#if LOCK_MODE == LOCKFREE
	,NULL
#endif
		);
	res = r->key == key;
#if LOCK_MODE == PESSIMISTIC
	pthread_spin_unlock(&set->g_lock);
#endif
#if LOCK_MODE == CHAINED
	pthread_spin_unlock(&l->lock);
	pthread_spin_unlock(&r->lock);
#endif
	return res;
}

bool insert(ll_set *set, int key){
	bool res = true;
	node *r, *l;
	node *n = nalloc();
	n->key = key;
begin:
#if LOCK_MODE == PESSIMISTIC
	pthread_spin_lock(&set->g_lock);
#endif
	l = search(set, key, &r
#if LOCK_MODE == LOCKFREE
	,NULL
#endif
		);
	n->next = r;
	if(r != set->tail && r->key == key) res = false;
	else{
#if LOCK_MODE != LOCKFREE 
		l->next = n;
#else
		if(!__sync_bool_compare_and_swap(&l->next, r, n)) { /*printf("FAILED CAS 1 %p %p\n", l, r);*/ goto begin;}
#endif
	}
#if LOCK_MODE == PESSIMISTIC
	pthread_spin_unlock(&set->g_lock);
#endif
#if LOCK_MODE == CHAINED || LOCK_MODE == OPTIMISTIC
	pthread_spin_unlock(&l->lock);
	pthread_spin_unlock(&r->lock);
#endif
	if(!res) nfree(n);
	return res;
}

bool delete(ll_set *set, int key){
	bool res = false;
	node *r, *l, *r_next;
	int loops = 0;
begin:
	loops++;
	if(loops % 10000 == 0){printf("ARGH %d\n", loops);}
#if LOCK_MODE == PESSIMISTIC
	pthread_spin_lock(&set->g_lock);
#endif
	l = search(set, key, &r
#if LOCK_MODE == LOCKFREE
	,NULL
#endif
		);
	if(r != set->tail && r->key == key){
#if LOCK_MODE != LOCKFREE
		l->next = r->next;
#else
		r_next = r->next;
		if(!__sync_bool_compare_and_swap(&r->next, get_unmarked_ref(r_next), get_marked_ref(r_next))){
				 //printf("FAILED CAS 2 %p %p\n", l, r); 
				 goto begin;
		}
#endif
		res = true;
		r->mark = true;
	}
#if LOCK_MODE == PESSIMISTIC
	pthread_spin_unlock(&set->g_lock);
#endif
#if LOCK_MODE == CHAINED || LOCK_MODE == OPTIMISTIC
	pthread_spin_unlock(&l->lock);
	pthread_spin_unlock(&r->lock);
#endif
	if(res) nfree(r);
	return res;
}
