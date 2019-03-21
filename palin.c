#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define MAX_CHARS 80
#define MAX_LINES 100
#define SHMKEY 314159
#define BUFF_SZ sizeof(char)*MAX_CHARS 


int main(int argc, char **argv){

	//create shared memory
	int shmid = shmget(SHMKEY, BUFF_SZ, 0666);

	if(shmid < 0){
		
		perror("ERROR: shmget");
	}
	
	//attach shared memory to address space of calling process
	char *shm_addr =(char*) shmat(shmid, (void*)0, 0);

	if (shm_addr == (char *) -1) { /* operation failed. */
	    perror("ERROR: shmat");
		 exit(1);
	}
	
	printf("DATA read from memory: %s\n", shm_addr);
	
	shmdt(shm_addr);

	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}//end main
