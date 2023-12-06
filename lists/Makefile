all:
	gcc main.c linked_list0.c -lpthread -O3 -Wall -DLOCK_MODE=0 -o pessimistic
	gcc main.c linked_list1.c -lpthread -O3 -Wall -DLOCK_MODE=1 -o chained
	gcc main.c linked_list2.c -lpthread -O3  -DLOCK_MODE=2 -o optimistic
	gcc main.c linked_list3.c -lpthread -O3  -DLOCK_MODE=3 -o lockfree

debug:
	gcc main.c linked_list0.c -lpthread -g3 -DLOCK_MODE=0 -o pessimistic
	gcc main.c linked_list1.c -lpthread -g3 -DLOCK_MODE=1 -o chained
	gcc main.c linked_list2.c -lpthread -g3 -DLOCK_MODE=2 -o optimistic
	gcc main.c linked_list3.c -lpthread -g3 -DLOCK_MODE=3 -o lockfree


clean:
	rm pessimistic chained optimistic lockfree
