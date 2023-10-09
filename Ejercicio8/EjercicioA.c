#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>

#define BUFSZ 4096
#define SEM_PRODUCTOR "/miSemaforoProductor"
#define SEM_CONSUMIDOR "/miSemaforoConsumidor"

int shmid, status;
sem_t *sem_prod, *sem_cons;
char *shmbuf;
char *shmbuf2;
int i,c;


void liberar_recursos(int pid){
	sem_close(sem_prod);
	sem_close(sem_cons);

	if( sem_unlink(SEM_PRODUCTOR) == -1 ){
		fprintf(stderr, "Error eliminando el semaforo productor. %d\n", pid);
		exit(EXIT_FAILURE);
	}

	if( sem_unlink(SEM_CONSUMIDOR) == -1){
		fprintf(stderr, "Error eliminando el semaforo consumidor. %d\n", pid);
		exit(EXIT_FAILURE);
	}

	if(shmdt(shmbuf2) == -1){
		fprintf(stderr, "shmdt failed. %d\n", pid);
		exit(EXIT_FAILURE);
	}

	if(shmctl(shmid, IPC_RMID, 0) == -1){
		fprintf(stderr, "shmctl(IPC_RMID) failed. %d\n", pid);
		exit(EXIT_FAILURE);
	}


}

int error( char *msg, int pid){
	fprintf(stderr, msg, pid);
	liberar_recursos(pid);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
	pid_t pid;
//	liberar_recursos(0);
	sem_prod = sem_open(SEM_PRODUCTOR, O_CREAT, 0666, 1);
	if( sem_prod == SEM_FAILED){
		perror("No se pudo crear el semaforo productor\n");
		exit(EXIT_FAILURE);
	}

	sem_cons = sem_open(SEM_CONSUMIDOR, O_CREAT, 0666, 0);
	if(sem_cons == SEM_FAILED){
		perror("No se pudo crear el semaforo consumidor\n");
		exit(EXIT_FAILURE);
	}

	shmid = shmget((key_t)1234, sizeof(char), 0666 | IPC_CREAT);
        if(shmid == -1) error("shmget failed\n", 0);	



	if( (pid = fork()) == -1) error("Error creando proceso\n", 0);

	if(pid == 0){
		//HIJO
		if(sem_wait(sem_cons) == -1)
			error("Error bajando el semaforo consumidor\n",pid);
		if((shmbuf = shmat(shmid, 0, SHM_RDONLY )) < (char *)0){
			perror("shmat");
			exit(EXIT_FAILURE);
		}

		printf("\nHijo muestra contenido de la memoria:\n");
		write(STDOUT_FILENO,shmbuf,BUFSZ);

		if((shmdt(shmbuf)) < 0 ){
			perror("shmdt");
			exit(EXIT_FAILURE);
		}


		if(sem_post(sem_prod) == -1)
			error("Error subiendo el semaforo productor\n",pid);
		return(0);


	}else{
		//PADRE
		if(sem_wait(sem_prod) == -1)
			error("Error bajando el semaforo productor\n", pid);

		if((shmbuf = shmat(shmid, 0, 0)) < (char *)0){
			perror("shmat");
			exit(EXIT_FAILURE);
		}

		shmbuf2 = shmbuf;

		memset(shmbuf, 0, BUFSZ);

		i = 0;
		while((c = getchar()) != EOF){
			*(shmbuf++) = c;
			i++;
		}

		write(STDOUT_FILENO, shmbuf, i);			

		if(sem_post(sem_cons) == -1)
		        error("Error subiendo el semaforo consumidor\n",pid);

		

	}

	//if((shmdt(shmbuf2)) < 0){
	//	perror("shmat");
	//	exit(EXIT_FAILURE);
	//}


	waitpid(pid, &status, 0);
	liberar_recursos(0);
	return(EXIT_SUCCESS);

}










































