#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

#define MAX_CHARS 80
#define MAX_LINES 100
#define SHMKEY 314159
#define BUFF_SZ sizeof(char)*80  

int shmid;
char (*shm)[MAX_LINES][MAX_CHARS] = NULL;

int isPalindrome(int, int);
void palindrome(int);
void noPalindrome(int);

int main(int argc, char **argv){

	srand(time(0)); //random seed
	int index = atoi(argv[1]);	  
	int length = atoi(argv[2]);

	//create shared memory
	shmid = shmget(SHMKEY, BUFF_SZ, 0666);

	if(shmid < 0){
		
		perror("ERROR: shmget");
	} 

	//attach shared memory to address space of calling process
	shm = shmat(shmid, (void*)0, 0);
	
	//printf("DATA read from memory: %s\n", ((*shm)[index]));

	if(isPalindrome(length, index)){
		palindrome(index);
	}
	else
		noPalindrome(index);

	shmdt(shm);

	//shmctl(shmid, IPC_RMID, NULL);
	return 0;
}//end main

int isPalindrome(int length, int index){
	int begin = 0; 
	while(length > begin){ 
		if((*shm)[index][length--] != (*shm)[index][begin++]){ 
			//printf("WE ARE NOt PALINDROME\n");
			return 0;
		}
	}//end while
	return 1;
}

void palindrome(int index){
	srand(time(0));
	printf("[PID]%i %s", getpid(), (*shm)[index]);
	sem_t *palinSem = sem_open("SemPalin", O_CREAT, 0644, 1); 
	
	sem_wait(palinSem);
	fprintf(stderr, "Enter CS [%i]\n", time(NULL));
	sleep(rand()%3 + 1);
	FILE *output = fopen("palin.out", "a");
	fprintf(output, "%i\t%i\t%s", getpid(), index, ((*shm)[index]));
	fclose(output);
	sleep(rand()%3 + 1);
	fprintf(stderr, "Exit CS [%i]\n\n", time(NULL));
	sem_post(palinSem);
}

void noPalindrome(int index){
	srand(time(0));
	printf("[PID]%i %s", getpid(), (*shm)[index]); 
	sem_t *noPalinSem = sem_open("SemNoPalin", O_CREAT, 0644, 1); 

	sem_wait(noPalinSem);                                                          
	fprintf(stderr, "Enter CS [%i]\n", time(NULL));                                        
	sleep(rand()%3 + 1);                                                             
	FILE *output = fopen("noPalin.out", "a");                                      
	fprintf(output, "%i\t%i\t%s", getpid(), index, ((*shm)[index]));             
	fclose(output);                                                              
	sleep(rand()%3 + 1);                                                             
	fprintf(stderr, "Exit CS [%i]\n\n", time(NULL));                                         
	sem_post(noPalinSem);
}



