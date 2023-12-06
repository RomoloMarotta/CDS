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

node* search(ll_set *set, int k, node** r_node){
	node *l, *t, *t_next, *l_next;

	t = set->head;
	t_next = set->head->next;

	while(1){
		if(!t->mark){
			l = t; l_next = t_next;
		}
		t = t_next;
		t_next = t->next;

		if(t == set->tail || 
		(!t->mark &&  t->key >= k)
		) break;
	}
	*r_node = t;
	return l;
}

bool find(ll_set *set, int key){
	bool res;
	node *l, *r;
	l = search(set, key, &r);
	res = r->key == key;
	return res;
}

bool insert(ll_set *set, int key){
	bool res = true;
	node *r, *l;
	node *n = nalloc();
	n->key = key;
begin:
	l = search(set, key, &r);
	LOCK(&l->lock);
	LOCK(&r->lock);
	if(l->mark || r->mark) goto retry;
	if(l->next != r) l->next = r;

	if(r != set->tail && r->key == key) {nfree(n); res = false;}
	else{
		n->next = r;
	
	//LOCK(&l->lock);
	//LOCK(&r->lock);
	//if(l->mark) goto retry;
	//else
		{ l->next = n;}
	}
	UNLOCK(&l->lock);
	UNLOCK(&r->lock);
	return res;
retry:
	UNLOCK(&l->lock);
	UNLOCK(&r->lock);
	goto begin;
}

bool delete(ll_set *set, int key){
	bool res = false;
	node *r, *l, *r_next;
begin:
	l = search(set, key, &r);	
	LOCK(&l->lock);
	LOCK(&r->lock);
	if(l->mark || r->mark) goto retry;
	if(l->next != r) l->next = r;

	if(r == set->tail || r->key != key) res = false;
	else{
//	LOCK(&l->lock);
//	LOCK(&r->lock);
	res = !r->mark;
	r->mark = true;
	//if(l->next != r) 
		l->next = r;}
	UNLOCK(&l->lock);
	UNLOCK(&r->lock);
	return res;
retry:
	UNLOCK(&l->lock);
	UNLOCK(&r->lock);
	goto begin;
}
