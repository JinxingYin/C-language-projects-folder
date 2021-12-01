#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

const int max = 10000000; //number of message to be sent
pthread_mutex_t lock;
int length = 0; //number of entries in the linked list
pthread_t tid[2];

struct node* head;
struct node* tail;

struct node
{
	struct node* next;
	int data;
};


void *consumer(void *vargp)
{
	int count = 0;
	while(count < max)
	{
		if(length>0){
   // consume messages (data from 0 to max-1, throw error if data out of order), invoke free on the head
	if(head->data != count) {printf("ERROR! data %d should be %d!\n", head->data, count);}
	     pthread_mutex_lock(&lock);
        if(length == 1){
			//pthread_mutex_lock(&lock);	
			free(head);
			head = NULL;    // free a memory block does not mean make the pointer be NULL, thus make it NULL here
			count++;
			length --;
		//pthread_mutex_unlock(&lock);
		}
		else{
			//pthread_mutex_lock(&lock);
			struct node* temp = malloc(sizeof(struct node)) ;
			 temp = head;
			head = head->next;
			free(temp);
			count++;
			length -- ;	
			//pthread_mutex_unlock(&lock);
		}
	//printf("node data value in consumer %d\n" , head->data);
	pthread_mutex_unlock(&lock);
	}
	}
	//printf("counter value at the end  in consumer %d\n" , count);
	return NULL;
}

void *producer(void *vargp)
{
	int count = 0;
	head = malloc(sizeof(struct node));
	tail = malloc(sizeof(struct node));
	while(count < max)
	{
		
	//produce messages (data from 0 to max-1), malloc new tails
	if(length < max){
		pthread_mutex_lock(&lock);
		struct  node* node_temp = malloc(sizeof(struct node));
				node_temp->data = count ;
				node_temp->next = NULL;
		if(length == 0){
				//pthread_mutex_lock(&lock);
				head = node_temp;
				tail = node_temp;
				count ++;
				length++;				
		//pthread_mutex_unlock(&lock);
		}
		else{
		//	pthread_mutex_lock(&lock);
			tail->next = node_temp;
			tail = tail->next;
			count ++;
			length++;	
	//	pthread_mutex_unlock(&lock);
		}
		pthread_mutex_unlock(&lock);
	}
	}
	//printf("counter value at the end  in producer %d\n" , count);
	return NULL;
}

int main()
{
	pthread_mutex_init(&lock, NULL);
	pthread_create(&tid[0], NULL, &producer, NULL);
	pthread_create(&tid[1], NULL, &consumer, NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[0], NULL);
	if(head != NULL) {printf("ERROR! List not empty\n" );}
	printf("The sending is successful\n");
	exit(0);
}

/*
Useful commands:
pthread_mutex_init(&lock, NULL)
pthread_create(&tid[0], NULL, &producer, NULL);
pthread_join(&tid[1], NULL);
pthread_mutex_lock(&lock);
pthread_mutex_unlock(&lock);
*/
