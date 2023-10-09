#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{
	int     fd[2];
	pid_t   chilpid;
	char	file;
	int	status1;
	int	status2;
	pipe(fd);

	if((chilpid = fork()) == -1)
	{
		perror("fork");
		exit(1);
		}      
	if(chilpid == 0){
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
    		execlp("/bin/grep","grep","-o", argv[1], argv[2], NULL);
    		exit(0);
		}
        if((chilpid = fork()) == -1){
                perror("fork");
                exit(1);
      		}
        if(chilpid == 0){	
       		close(fd[0]);
      		dup2(fd[1], STDOUT_FILENO);
       		close(fd[1]);
     		execlp("/bin/grep","grep","-o", argv[1], argv[3], NULL);
       		exit(0);
        }
        if((chilpid = fork()) == -1){
                perror("fork");
                exit(1);
  		}
        if(chilpid == 0){
		close(fd[1]);
		file = open(argv[4], O_WRONLY);	
        	dup2(fd[0], STDIN_FILENO);
        	close(fd[0]);
		dup2(file, STDOUT_FILENO);
      		execlp("/usr/bin/wc", "wc", "-l", NULL);
      		exit(0);
        	}
	close(fd[0]);
	wait(&status1);
	wait(&status2);
	return(0);
}
