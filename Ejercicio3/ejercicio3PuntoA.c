#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) { 
	int x;
	pid_t hijo;

	printf("PADRE pid: %d\n", getpid());
	printf("HIJOS\n");
	
	for(x=0; x<3; x++){
	hijo = fork();
		if(hijo ==  0){
			printf("pid: %d -ppid: %d\n", getpid(), getppid());
			exit(0);
		}
	}
	sleep(1);
	return 0;
}

