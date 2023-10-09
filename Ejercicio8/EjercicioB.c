#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>    // O_CREAT
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>


#define SEM_A "/miSemaforoA"
#define SEM_B "/miSemaforoB"
#define SEM_C "/miSemaforoC"

#define MAX 6

int status;
sem_t *sem_A, *sem_B, *sem_C;
pid_t pid;


void escribirA (void){
	int num; 
	for(num=0;num<MAX;num++){
			
 		if ( sem_wait(sem_A) == -1 )
			error("Error bajando el semaforo B\n",pid);
		
		printf("A"); 
		fflush(stdout); 
		sleep(random() %3);
		
		if ( sem_post(sem_B) == -1)
			error("Error subiendo el semaforo A\n",pid);
	}
} 

void escribirB (void){
	int num; 
	for(num=0;num<MAX;num++){
		
 		if ( sem_wait(sem_B) == -1 )
			error("Error bajando el semaforo B\n",pid);
		
		printf("B"); 
		fflush(stdout); 
		sleep(random() %3);
		
		if ( sem_post(sem_C) == -1)
			error("Error subiendo el semaforo A\n",pid);
	}
} 

void escribirC (void){
	int num; 
	for(num=0;num<MAX;num++){
		if ( sem_wait(sem_C) == -1 )
			error("Error bajando el semaforo B\n",pid);
		
		printf("C"); 
		fflush(stdout); 
		sleep(random() %3);
		
		if ( sem_post(sem_A) == -1)
			error("Error subiendo el semaforo A\n",pid);

	}
}

void liberar_recursos(int pid){
	sem_close(sem_A);
	sem_close(sem_B);
	sem_close(sem_C);

	if ( sem_unlink(SEM_A) == -1){
		fprintf(stderr, "Error eliminando semaforo A. %d\n",pid);
	}
	if ( sem_unlink(SEM_B) == -1){
		fprintf(stderr, "Error eliminando semaforo B. %d\n",pid); 
	}
	if ( sem_unlink(SEM_C) == -1){
		fprintf(stderr, "Error eliminando semaforo C. %d\n",pid);
	}
}

int error( char *msg, int pid ){
	fprintf(stderr, msg, pid);
	liberar_recursos(pid);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	sem_A = sem_open(SEM_A, O_CREAT, 0666, 1);
	if (sem_A == SEM_FAILED){
		perror("No se pudo crear el semaforo A\n");
		exit(EXIT_FAILURE);
	}
	sem_B = sem_open(SEM_B, O_CREAT, 0666, 0);
	if (sem_B == SEM_FAILED){
		perror("No se pudo crear el semaforo B\n");
		exit(EXIT_FAILURE);
	}
	sem_C = sem_open(SEM_C, O_CREAT, 0666, 0);
	if (sem_C == SEM_FAILED){
		perror("No se pudo crear el semaforo C\n");
		exit(EXIT_FAILURE);
	}
	
	

	if ( (pid = fork()) == -1) error("Error creando procesos\n",0);

	if (pid == 0) {
		
		escribirC();
		return(0);
	} 
	
	if ( (pid = fork()) == -1) error("Error creando procesos\n",0);

	if (pid == 0) {
		escribirB();
		return(0);
	} 

	escribirA();


	waitpid(pid, &status, 0); 
	liberar_recursos(0);
	printf("\n");
    return(EXIT_SUCCESS);
}
