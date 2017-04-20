//cs170.cpp
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>


using namespace std;

struct command
{
  const char **argv;
};

void echo(char **tokens)
{
 	for(int i = 1; tokens[i] != NULL; i++){
 		write(1,tokens[i], strlen(tokens[i]));
 		write(1," ",1);
 	}
 	write(1, "\n",1);
}


int spawn_proc(int input, int output, struct command *cmd){
	pid_t pid;
	if((pid = fork()) == 0){
		if(input != 0){
		dup2(input, 0);
		close(input);
	}
	if(output != 1){
		dup2(output, 1);
		close(output);
	}
	return execvp(cmd -> argv[0], (char * const *)cmd ->argv);

}
return pid;
}

int piping(int n, struct command *cmd){
	
	pid_t pid;
	int fd[2];
	int input = 0;

for(int i =0; i < n-1; i++){
	pipe(fd);
	spawn_proc(input, fd[1], cmd +i);
	close(fd[1]);
	input = fd[0];
}
if(input !=0){
	dup2(input, 0);
}
	return execvp(cmd[i].argv[0], char( * const)cmd[i].argv);
if (fd[0] != 1){
	dup2(fd[0], 1);
	close(fd[0]);
}
}


int
main ()
{
  const char *ls[] = { "ls", "-l", 0 };
  const char *awk[] = { "awk", "{print $1}", 0 };
  const char *sort[] = { "sort", 0 };
  const char *uniq[] = { "uniq", 0 };

  struct command cmd [] = { {ls}, {awk}, {sort}, {uniq} };

  return fork_pipes (4, cmd);
}