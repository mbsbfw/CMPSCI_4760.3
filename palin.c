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

	//char (*shm)[MAX_LINES][MAX_CHARS] = NULL; 

	//attach shared memory to address space of calling process
	shm = shmat(shmid, (void*)0, 0);
	
	printf("DATA read from memory: %s\n", ((*shm)[index]));

	isPalindrome(length, index);

	shmdt(shm);

	//shmctl(shmid, IPC_RMID, NULL);
	return 0;
}//end main

int isPalindrome(int length, int index){
	int begin = 0; 
	while(length > begin){ 
		if((*shm)[index][length--] != (*shm)[index][begin++]) 
			//printf("WE ARE NOt PALINDROME\n");
			noPalindrome(index);
	}//end while

	if((*shm)[index][length] == (*shm)[index][begin]){ 
		//printf("PALINDROME\n");
		palindrome(index);
	}

}

void palindrome(int index){
	printf("[PID]%i PALINDROME\n", getpid());
}

void noPalindrome(int index){
	printf("WE ARE NOt PALINDROME\n"); 
}



