#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>

#define MAX_LINES 100
#define MAX_CHARS 80
#define SHMKEY 314159
#define BUFF_SZ sizeof(char)*MAX_CHARS

static void myhandler(int s);
static int setupinterrupt(void);
static int setupitimer(void);

int main(int argc, char **argv){
		  
	if(setupinterrupt() == -1){
		perror("Failed to set up handler for SIGPROF");
	 	return 1;
  	}//end if

  	if(setupitimer() == -1){
  		perror("Failed to set up the TIMER_PROF interval timer");
		return 1;
 	}//end if
	
	//most variables
	int maxTotalForks = 4;
	int option;
	int i = 0;
	char readLine[MAX_CHARS];

	char * input = malloc(80 * sizeof(char) + 1);

	strcpy(input, "palin.in"); 

	while((option = getopt(argc, argv, "hn:i:")) != -1){

		switch(option){
			case 'h' :
				printf("\t\t---Help Menu---\n");
				printf("\t-h Display Help Menu\n");
				printf("\t-n x indicate the maximum total of child processes\n");
				printf("\t-i x specify input File name\n");
				exit(1);

			case 'n' :
				maxTotalForks = atoi(optarg);
				if(maxTotalForks > 20) {
					maxTotalForks = 20;
				}
				break;

			case 'i' :
				strcpy(input, optarg);
				break;

			case '?' :
				printf("ERROR: Improper arguments");
				break;

		}//end switch
	}//end while

	//open input file
	FILE * fPointer = fopen(input, "r");

	//if input file failed to open
	if(fPointer == NULL){
		perror("ERROR: Input file failed to open");
	}

	//create shared memory
	int shmid = shmget(SHMKEY, BUFF_SZ, IPC_CREAT | 0666);
	
	if(shmid < 0){
		perror("ERROR: shmget");
	}

	char (*shm)[MAX_LINES][MAX_CHARS] = NULL;
	
	//attach shared memory to address space of calling process
	shm = shmat(shmid, (void*)0, 0);

	while(!feof(fPointer)){

		if(fgets(readLine, MAX_CHARS, fPointer)){

			printf("DATA written in memory: %s\n", readLine);
			strcpy((*shm)[i], readLine);
			i++; 	
		}
	}//end while

	printf("COPIED DATA: %s", &shm[0]);
	shmdt(shm);
	return 0;
}//end main

static void myhandler(int s){
	
	char aster = '*';
	int errsave;
	errsave = errno;
	write(STDERR_FILENO, &aster, 1);
	printf("Terminate after 2 REAL life seconds\n");
	exit(1);
	errno = errsave;

}//end myhandler

static int setupinterrupt(void){
	
	struct sigaction act;
	act.sa_handler = myhandler;
	act.sa_flags = 0;
	return(sigemptyset(&act.sa_mask) || sigaction(SIGPROF, &act, NULL));

}//end setupinterrupt

static int setupitimer(void){
	
	struct itimerval value;
	value.it_interval.tv_sec = 2;
	value.it_interval.tv_usec = 0;
	value.it_value = value.it_interval;
	return (setitimer(ITIMER_PROF, &value, NULL));

}//end setupitimer


