//cs170.cpp
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <cstdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <csignal>
#include <cstring>

sig_atomic_t exit_counter = 0;

using namespace std;

struct command
{
  const char **argv;
};

void echo(char **tokens) //probably need to account for meta characters
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
	int i;
	pid_t pid;
	int fd[2];
	int input = 0; //first process will get input from og file desc

for(int i =0; i < n-1; i++){
	pipe(fd);
	spawn_proc(input, fd[1], cmd +i); //f[1] is the write end of the pipe
	close(fd[1]); //child begins the write
	input = fd[0]; //keep the read end of pipe, next child will begin to read
}
if(input !=0){ //last stage of pipe -> stdin be the read end of prev pipe
	dup2(input, 0);
}
	return execvp(cmd[i].argv[0], (char * const*)cmd[i].argv); //exec the last stage with current process
if (fd[0] != 1){
	dup2(fd[0], 1); //close file
	close(fd[0]);
}
}



/*int
test ()
{
  const char *ls[] = { "ls", "-l", 0 };
  const char *awk[] = { "awk", "{print $1}", 0 };
  const char *sort[] = { "sort", 0 };
  const char *uniq[] = { "uniq", 0 };

  struct command cmd [] = { {ls}, {awk}, {sort}, {uniq} };

  return fork_pipes (4, cmd);
}
*/

int find_in_args(char** args, char* target){
	int found = -1;
	int i = 0;
	while(args[i] != NULL){
		if(strcmp(args[i],target) != 0){
			i++;
		}
		else{
			found = i;
			break;
		}
	}

}

void handler_function(int parameter){
	if(exit_counter == 1){
		exit(EXIT_SUCCESS);
	}
	else if(exit_counter == 0){
		exit_counter++;
	}
}

void launch(char** args){
	pid_t pid = fork();
	if(pid){
		waitpid(pid, NULL, 0);
	}
	else{
		execvp(*args, args);
	}
}

int execute(char **args_list){
	signal (SIGINT, handler_function);
	//1 means keep looping, 0 means stop execution
	int iterator;
	char key[] = "exit";
	
	if (args_list[0] == NULL){
		return 1;
	}
	
	else if (!strcmp(args_list[0], key)){
		//std::cout<<"successfully exited\n";
		return 0;
	}
	
	launch(args_list);
	
	/*
	//temp loop for testing
	for (iterator = 0; iterator < 10; iterator++){
		std::cout << "Print first 10 args, or until null char: \n";
		if (args_list[iterator] != NULL){
			std::cout << args_list[iterator] << "\n";
		}
	}
	* */
	//check for built-ins
	//if built-in, run it
	//else skip to end
	return 1; //change to launch program
}

char *parse(){
	signal (SIGINT, handler_function);
	std::string read;
	std::getline(std::cin, read); //thank you c++ gods for making this function
	char *read_c = new char[read.length() + 1];
	std::strcpy (read_c, read.c_str());
	
	//for testing
	//	std::cout << "Parsed successfully \n";
	return read_c;
}

char **tokenize(char *line){
	signal (SIGINT, handler_function);
	int buf = 640; //maybe reallocate when full later?
	char *token;
	char **tokens = (char**) malloc(buf * sizeof(char*));
	char delimeter[] = " \t;\n";
	int i = 0;
	
	token = strtok(line, delimeter);
	while (token != NULL){
		tokens[i] = token;
		i++;
		token = strtok(NULL, delimeter);
	}
	tokens[i] = NULL;
	
	//for testing
	//std::cout << "Tokenized successfully \n";
	return tokens;
}

void shell_loop(){
	
	signal (SIGINT, handler_function);
	char **split_args;
	char *read_line;
	//used for terminating while loop
	int running;
	
	do{
		//idk... because that's what shells do for some reason.. 
		//looks fresh I guess
		std::cout << "> ";
		
		//read line
		read_line = parse();
		
		//separate line into command and args
		split_args = tokenize(read_line);
		
		//execute command
		running = execute(split_args);
		
	}while(running);
}



int main(int argc, char **argv){
	signal (SIGINT, handler_function);
	
	//run command loop
	shell_loop();
	
	return EXIT_SUCCESS;
}