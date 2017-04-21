#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_TOKEN_LENGTH 50
#define MAX_TOKEN_COUNT 100
#define MAX_LINE_LENGTH 512

sig_atomic_t exit_counter = 0;

// Simple implementation for Shell command
// Assume all arguments are seperated by space
// Erros are ignored when executing fgets(), fork(), and waitpid(). 

/**
 * Sample session
 *  shell: echo hello
 *   hello
 *   shell: ls
 *   Makefile  simple  simple_shell.c
 *   shell: exit
**/

void handler_function(int parameter){
  if(exit_counter == 1){
    exit(EXIT_SUCCESS);
  }
  else if(exit_counter == 0){
    exit_counter++;
  }
}

void runcommand(char* command, char** args) {
  pid_t pid = fork();
  if(pid) { // parent
    	waitpid(pid, NULL, 0);
  } else { // child
    	execvp(command, args);
  }
}

int find_arg(char** args, char* command){
  int found = -1;
  int i = 0;
  while(args[i] != NULL){
    if(strcmp(command, args[i])){
      ++i;
    }
    else{
      found = i;
      break;
    }
  }
  //testing
  //  printf("find_arg success\n");
  return found;
}

void split_arg(char** args, char** split_args, int split_point){
    int i = 0;
    for (i; i < split_point; ++i){
      split_args[i] = args[i];
    }
    split_args[i] = '\0';
    //testing
    // printf("split success\n");
    // int temp;
    //    for (temp = 0; temp < i; temp++){
    // printf("%s\n", split_args[temp]);
    // }
  }

void launch(char** args){
    signal (SIGTSTP, handler_function);
    int i = 0;
    pid_t pid;
    if (find_arg(args, ">")!= -1){
      //test
      // printf("'>' found\n");
      
      int pos = find_arg(args, ">");
      char* f = args[pos+1];
      char* new_arg[pos];
      split_arg(args, new_arg, pos);
      
      pid = fork();
      if(pid == 0){ //child
  int out = open(f, O_CREAT | O_WRONLY, S_IRWXU);
  dup2(out, STDOUT_FILENO);
  close(out);
  
  execvp(*new_arg, new_arg);
  exit(EXIT_FAILURE);
      }
      else{
  wait(0);
      }
    }
    else if(find_arg(args, "<") != -1){
      //test
      // printf("'<' found\n");
      
      int pos = find_arg(args, "<");
      char* f = args[pos+1];
      char* new_arg[pos];
      split_arg(args, new_arg, pos);
      
      pid = fork();
      if(pid == 0){//child
  int in = open(f, O_RDONLY, S_IRWXU);
  if (in == -1){
    printf("%s not found in current directory \n", f);
  }
  else{
    dup2(in, STDIN_FILENO);
    close(in);

    execvp(args[0], new_arg);
    exit(EXIT_FAILURE);
  }
      }
      else{
  wait(0);
      }
    }
    else if (find_arg(args, "|") != -1){
      int pos = find_arg(args, "|");
      int input = STDIN_FILENO;
      int output = STDOUT_FILENO;

      int t[2];
      pipe(t);
      output = t[1];

      args[pos] = '\0';
      pid_t pid1 = fork();
      if(pid1 == 0){//child
  if (input != STDIN_FILENO){
    dup2(input, STDIN_FILENO);
  }
  if(output != STDOUT_FILENO){
    dup2(output, STDOUT_FILENO);
  }
  execvp(*args, args);
  exit(EXIT_FAILURE);
      }
      else{
  wait(0);
      }
      close(t[1]);

      input = t[0];
      output = STDOUT_FILENO;

      pid_t pid2 = fork();
      if(pid2 == 0){//child
  if(input != STDIN_FILENO){
    dup2(input, STDIN_FILENO);
  }
  if(output!=STDOUT_FILENO){
    dup2(output, STDOUT_FILENO);
  }
  execvp((args + pos + 1)[0], args + pos + 1);
  exit(EXIT_FAILURE);
      }
      else{
  wait(0);
      }
    }
    else{
      pid = fork();
      if(pid){
  waitpid(pid, NULL, 0);
      }
      else{
  execvp(*args, args);
      }
    }
  }



int main(){
   signal (SIGTSTP, handler_function);
    char line[MAX_LINE_LENGTH];
    //printf("shell: "); 
    while(fgets(line, MAX_LINE_LENGTH, stdin)) {
    	// Build the command and arguments, using execv conventions.
    	line[strlen(line)-1] = '\0'; // get rid of the new line
    	char* command = NULL;
    	char* arguments[MAX_TOKEN_COUNT];
    	int argument_count = 0;

    	char* token = strtok(line, " ");
    	while(token) {
      		if(!command) command = token;
      		arguments[argument_count] = token;
	      	argument_count++;
      		token = strtok(NULL, " ");
    	}
    	arguments[argument_count] = NULL;
	if(argument_count>0){
		if (strcmp(arguments[0], "exit") == 0)
            		exit(0);
    		//runcommand(command, arguments);
        launch(arguments);
	}
        //printf("shell: "); 
    }
    return 0;
}
