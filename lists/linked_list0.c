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


node* search(ll_set *set, int k, node** r){
	node *l, *t, *t_next;
	t = set->head;
	t_next = set->head->next;


	while(1){
		l = t;
		t = t_next;
		t_next = t->next;

		if(t == set->tail || t->key >= k) break;

	}	
	*r = t;
	return l;
}

bool find(ll_set *set, int key){
	bool res;
	node *r;
	LOCK(&set->g_lock);
	search(set, key, &r);
	res = r->key == key;
	UNLOCK(&set->g_lock);

	return res;
}

bool insert(ll_set *set, int key){
	bool res = true;
	node *r, *l;
	node *n = nalloc();
	n->key = key;
	LOCK(&set->g_lock);
	l = search(set, key, &r);
	n->next = r;
	if(r != set->tail && r->key == key) res = false;
	else l->next = n;

	UNLOCK(&set->g_lock);
	if(!res) nfree(n);
	return res;
}

bool delete(ll_set *set, int key){
	bool res = false;
	node *r, *l;
	LOCK(&set->g_lock);
	l = search(set, key, &r);
	if(r != set->tail && r->key == key){
		l->next = r->next;
		res = true;
	}
	UNLOCK(&set->g_lock);
	
	if(res) nfree(r);
	return res;
}
