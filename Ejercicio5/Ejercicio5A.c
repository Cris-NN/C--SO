#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
int contadorSIGUSR1 = 0;
int contadorSIGUSR2 = 0;

void manejadorSIGUSR1(int sig);
void manejadorSIGUSR2(int sig);

int main(int argc, char *argv[]) { 
	int contador = 0;

	if ( signal(SIGUSR1, manejadorSIGUSR1) == SIG_ERR) {
		perror("Error instalando manejador de SIGUSR1");
		exit (-1); 
	}
       if ( signal(SIGUSR2, manejadorSIGUSR2) == SIG_ERR){
       		perror("Error instalando manejador de SIGUSR2");
		exit (-1);
       }
	if ( signal(SIGTERM, manejadorSIGUSR2) == SIG_ERR){
		exit(-1);
	}       
	printf("Proceso %d esperando..\n",getpid());
	printf("Ejecutar kill -10/-12  %d\n",getpid());
	while(1)
	sleep(5);
	exit(0); 
} 

void manejadorSIGUSR1(int sig) {
	if(sig == SIGUSR1)
		contadorSIGUSR1++;
	printf(":p \n");
}

void manejadorSIGUSR2(int sig){
	
	if(sig == SIGUSR2)
		contadorSIGUSR2++;

	printf(":p \n");

	if(sig == SIGTERM){
		printf("Programa finalizado \n");
		printf("Señales SIGUSR1: %d \n", contadorSIGUSR1);
		printf("Señales SIGUSR2: %d \n", contadorSIGUSR2);
		exit(0);
	}	
	 
}
