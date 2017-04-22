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
struct commands{
  const char **argv;
};

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

//TODO:

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
      /*
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
    } */

      pipeHandler(args);
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
// Function below for multiple pipes, but I thought it was easier to just make a function for pipes
void pipeHandler(char * args[]){
  // File descriptors
  int file[2];
  int file2[2];
  
  int cmds = 0;
  
  char *command[256];
  
  pid_t pid;
  
  int err = -1;
  int end = 0;
  
  // Variables used for the different loops
  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;
  
 
  while (args[l] != NULL){
    if (strcmp(args[l],"|") == 0){
      cmds++;
    }
    l++;
  }
  cmds++;

  while (args[j] != NULL && end != 1){
    k = 0;

    while (strcmp(args[j],"|") != 0){
      command[k] = args[j];
      j++;  
      if (args[j] == NULL){

        end = 1;
        k++;
        break;
      }
      k++;
    }

    command[k] = NULL;
    j++;    
    
    if (i % 2 != 0){
      pipe(file); // for odd i
    }else{
      pipe(file2); // for even i
    }
    
    pid=fork();
    
    if(pid==-1){      
      if (i != cmds - 1){
        if (i % 2 != 0){
          close(file[1]); // for odd i
        }else{
          close(file2[1]); // for even i
        } 
      }     
      printf("Child process could not be created\n");
      return;
    }
    if(pid==0){
      // If we are in the first command
      if (i == 0){
        dup2(file2[1], STDOUT_FILENO);
      }
      
      else if (i == cmds - 1){
        if (cmds % 2 != 0){ 
          dup2(file[0],STDIN_FILENO);
        }else{ 
          dup2(file2[0],STDIN_FILENO);
        }
      }else{ 
        if (i % 2 != 0){
          dup2(file2[0],STDIN_FILENO); 
          dup2(file[1],STDOUT_FILENO);
        }else{ 
          dup2(file[0],STDIN_FILENO); 
          dup2(file[1],STDOUT_FILENO);          
        } 
      }
      
      if (execvp(command[0],command)==err){
        kill(getpid(),SIGTERM);
      }   
    }
        
    // Closing desc on parents
    if (i == 0){
      close(file2[1]);
    }
    else if (i == cmds - 1){
      if (cmds % 2 != 0){         
        close(file[0]);
      }else{          
        close(file2[0]);
      }
    }else{
      if (i % 2 != 0){          
        close(file2[0]);
        close(file[1]);
      }else{          
        close(file[0]);
        close(file2[1]);
      }
    }
        
    waitpid(pid,NULL,0);
        
    i++;  
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
      int meta_count = 0;
      int pipe_count = 0;
    	char* token = strtok(line, " ");
    	while(token) {
      		if(!command) command = token;
      		arguments[argument_count] = token;
	      	argument_count++;
          if((arguments[argument_count] == "<")||(arguments[argument_count] == ">")){
            meta_count++;
          }
          if(arguments[argument_count] == "|"){
            pipe_count++;
          }
      		token = strtok(NULL, " ");
    	}
    	arguments[argument_count] = NULL;
	if((argument_count>0)&& (meta_count <= 1)&&(pipe_count <= 1)){
		if (strcmp(arguments[0], "exit") == 0)
            		exit(0);
    		//runcommand(command, arguments);
        launch(arguments);
	}
    else if((argument_count>0)&& (meta_count > 1)&&(pipe_count > 1)){
      //struct commands cmd[] = {{command}};
     // return fork_pipes(pipe_count, cmd);
    }
        //printf("shell: "); 
    }
    return 0;
}
