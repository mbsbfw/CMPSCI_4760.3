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
#define BUFF_SZ sizeof(char)*80

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
	int status;
	int index = 0;
	int children = 0;
	int length[MAX_LINES]; 
	char readLine[MAX_CHARS];
	sem_t *palinSem;
	sem_t *nopalinSem;

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
	int shmid = shmget(SHMKEY, BUFF_SZ, 0666 | IPC_CREAT);
	
	if(shmid < 0){
		perror("ERROR: shmget");
	}

	char (*shm)[MAX_LINES][MAX_CHARS] = NULL;
	
	//attach shared memory to address space of calling process
	shm = shmat(shmid, (void*)0, 0);
	
	//read from file and put in shared memory
	while(!feof(fPointer)){

		if(fgets(readLine, MAX_CHARS, fPointer)){
			
			length[index] = strlen(readLine) - 2;		
			strcpy((*shm)[index], readLine);
			printf("DATA written in memory: %s\n", ((*shm)[index]));
			index++; 	
		}
	}//end while

	//printf("%d", index);
	//exit(0);
	int i = 0;	
	pid_t pidHolder[index];

	while(i < index){

		char arg1[MAX_CHARS];  
		snprintf(arg1, MAX_CHARS, "%d", i); 
	
		char arg2[length[i]];
		snprintf(arg2, MAX_CHARS, "%d", length[i]);
		
		//printf("I=%d\n", i);
		if(children > maxTotalForks){
			wait(&status);
			printf("waiting");
		}//end if
		
		if((pidHolder[i] = fork()) == -1){
			perror("ERROR forking");
		}

		else if((pidHolder[i] = fork()) == 0){
			execl("./palin", "palin", arg1, arg2, NULL);
			exit(0);
		}

		else{
			//printf("increment the children\n");
			children++;

			if(waitpid(pidHolder[i], &status, 0) > 0){

				if (WIFEXITED(status) && !WEXITSTATUS(status)){
					//printf("program execution successfull\n");
					//printf("decrement the children\n");
					children--;
				}

				else if (WIFEXITED(status) && WEXITSTATUS(status)) {
					if (WEXITSTATUS(status) == 127) {

						// execv failed
						printf("execv failed\n"); 
					}//end if
					else
						printf("program terminated normally,"
						" but returned a non-zero status\n");
				}//end else if
				else
					printf("program didn't terminate normally\n");
			}//end if
			else{
				// waitpid() failed 
				printf("waitpid() failed\n");
			}//end else
			//exit(0);
		}//end else
		i++;
	}//end for
	
	//printf("CHILDREN = %d\n", children);
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


