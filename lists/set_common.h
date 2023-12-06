#define BUFF_SIZE 10000000
__thread node *array;
__thread int idx = 0;

node* nalloc(){
	node *res;
	if(array == NULL) array = (node*) malloc(BUFF_SIZE*sizeof(node));
	if(idx >= 0 && idx < BUFF_SIZE){
		res = array + idx++;	
	}
	else{
		if(idx >= 0) printf("ARGH\n");
		idx = -1;
		res = (node*) malloc(sizeof(node));

	}
	res->mark = false;
	INIT(&res->lock);
	return res;
}

void nfree(void* ptr){
	//free(ptr);
}

ll_set* init_ll_set(){
	ll_set *res = malloc(sizeof(ll_set));
	if(!res) {printf("No memory\n"); exit(1);}
	res->head = nalloc();
	if(!res->head) {printf("No memory\n"); free(res); exit(1);}
	res->tail = nalloc();
	if(!res->tail) {printf("No memory\n"); free(res->tail); free(res); exit(1);}
	INIT(&res->g_lock);
	res->head->next = res->tail;
	res->tail->key = 0xffffff;
	return res;
}
