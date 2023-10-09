#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
long long int factorial (int n) { 
	long long int resultado= 1; 
	int num;
	for (num=2; num<= n; num++) { 
		resultado= resultado*num;
		printf ("Factorial de %d, resultado parcial %lld\n", n, resultado);
		sleep (random()%3);
       	} 
	return resultado;
} 

int main (int argc, char *argv[]){
	pid_t hijo;
	int x;
	int status;

	printf("*** Inicio de Proceso Padre pid: %d -ppid: %d \n", getpid(), getppid());

	for(x=1; x<3; x++){
		hijo= fork();
		if(hijo == 0){
			printf("**Proceso Hijo pid: %d -ppid: %d \n", getpid(), getppid());
			printf ("El factorial de %s es %lld\n", argv[x], factorial (atoi (argv[x]))); 
			exit(0);
		}	
	}

	for(int i = 0; i < 2; i++){
		hijo = wait(&status);
	}
	printf("***Fin de Proceso Padre pid: %d -ppid: %d \n", getpid(), getppid());

	return 0;


} 
