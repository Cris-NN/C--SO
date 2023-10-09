#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h> 
#include <signal.h>
#include <errno.h>



#define BUFSZ 4096

int sig1;
int shmid;
char *shmbuf;
char *shmbuf2;
int c,i,s;

void manejador(int sig);

int main(int argc, char *argv[])
{	

	if ( signal(SIGUSR1, manejador) == SIG_ERR){
		perror("Error instalando manejador de SIGUSR1");
		exit(-1);
	}

	if(argc != 2) {
		puts("USAGE: wrshm <identifier>");
		exit(EXIT_FAILURE);
	}
	shmid = atoi(argv[1]);
    
	if((shmbuf = shmat(shmid, 0, 0)) < (char *)0) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}
	shmbuf2=shmbuf;
	memset(shmbuf, 0, BUFSZ); 
	i = 0;
	while((c = getchar()) != EOF) {
		*(shmbuf++) = c;
		++i;
	}
	write(STDOUT_FILENO, shmbuf, i);

    	if((shmdt(shmbuf2)) < 0) {
     	  	 perror("shmdt");
       		 exit(EXIT_FAILURE);
    }
	//exit(EXIT_SUCCESS);

}

void manejador(int sig){
	pid_t hijo;
                if((hijo = fork()) == -1){
                         perror("fork");
                         exit(1);
                 }
                if(hijo == 0){
			if((shmbuf = shmat(shmid, 0, SHM_RDONLY))<(char *)0){
				perror("shmat");
				exit(EXIT_FAILURE);
			}	

           	   printf("\n Contenido de la shmem:\n");
                write(STDOUT_FILENO, shmbuf, BUFSZ);
                if((shmdt(shmbuf)) < 0) {
                        perror("shmdt");
                        exit(EXIT_FAILURE);
                         }
                exit(0);                                                                
}
exit(EXIT_SUCCESS);
}


