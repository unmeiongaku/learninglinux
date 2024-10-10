#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char const *argv[]){

	pid_t child_pid;
	int status;
	int counter =0;
	printf("zoombie process app\n");
	child_pid = fork();
	if(child_pid>=0){
		if(child_pid==0){
		printf("Child process termination, child pid: %d\n",getpid());	
		printf("Can not kill zoombie child process by using kill -9 %d\n",getpid());
		printf("You have to kill parents process by using kill -9 %d\n",getppid());
		printf("/****************************/\n");
		printf("Because when you kill parent process, the child process will be orphaned\n");
		printf("The grandpa process will be call wait() syscall function\n");
		printf("/****************************/\n");
			//do nothing	
 			//while(1);
		}
		else {                    /* Process cha */
			while(1);  
			//wait(&status);
	    }
	}	
    else{
        printf("fork() unsucessfully\n");
    }
	
	return 0;
}

