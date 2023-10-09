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

#define BUFSZ 4096
#define SEM_A "/miSemaforoA"


int shmid, status;
char *shmbuf;
char *shmbuf2;
sem_t *sem_a;

void liberar_recursos(int pid){
	sem_close(sem_a);

	if ( sem_unlink(SEM_A) == -1){
		fprintf(stderr, "Error eliminando semaforo productor. %d\n",pid);
		//exit(EXIT_FAILURE);
	}
	if (shmdt(shmbuf2) == -1) {
		fprintf(stderr, "shmdt failed. %d\n",pid);
		//exit(EXIT_FAILURE);
	}
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		fprintf(stderr, "shmctl(IPC_RMID) failed. %d\n",pid);
		//exit(EXIT_FAILURE);
	}
}

int error( char *msg, int pid ){
	fprintf(stderr, msg, pid);
	liberar_recursos(pid);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	pid_t pid;
	int *cont;

	sem_a = sem_open(SEM_A, O_CREAT, 0666, 1);
	if (sem_a == SEM_FAILED){
		perror("No se pudo crear el semaforo productor\n");
		exit(EXIT_FAILURE);
	}

	shmid = shmget((key_t)1234, sizeof(char), 0666 | IPC_CREAT);

	if (shmid == -1) error("shmget failed\n",0);

	if (( shmbuf = shmat(shmid, 0, 0)) < (int *)0) error("shmat failed\n",0);

	shmbuf2 = shmbuf;

	printf("Memory id: %d\n", shmid);fflush(stdout);


	for(int x=2; x<argc; x++){
		if ( (pid = fork()) == -1) error("Error creando procesos\n",0);

		if (pid == 0) {

			char linea[260];
			int contador = 0;
			FILE* fichero;
			
			printf("\nLETRA: %s   archivo: %s\n",argv[1], argv[x]);

			fichero = fopen(argv[x],"rb");
			if(!fichero)
				printf("\n El archivo no se a podido abrir\n");
			else{
				while(!feof(fichero)){
					fscanf(fichero,"%c",linea);
					for(int i=0; i<strlen(linea); i++){
						if(strcmp(linea,argv[1])==0)
							contador++;
					}	
				}
				fclose (fichero);
			printf("\nCantidad de letras : %d\n", contador);	
			}
 			if ( sem_wait(sem_a) == -1 )
				error("Error bajando el semaforo A",pid);
		
			
			
			if ( sem_post(sem_a) == -1)
				error("Error subiendo el semaforo A",pid);		
			return(0);
		} 
	}
	waitpid(pid, &status, 0);
	if((shmbuf = shmat(shmid, 0, SHM_RDONLY)) < (char *)0){
		perror("shmat");
		exit(EXIT_FAILURE);
	}
	write(STDOUT_FILENO,shmbuf,BUFSZ);
	if((shmdt(shmbuf)) < 0){
		perror("shmdt");
		exit("EXIT_FAILURE");
	}

	liberar_recursos(0);
	printf("Fin.\n");fflush(stdout);
    return(EXIT_SUCCESS);
}
