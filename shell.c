//cs170.c

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_CMD_SZ 512;
#define MAX_TKN_SZ 32;

const char* SHELL_NAME = "propShell> ";
sig_atomic_t exit_counter = 0;


struct command
{
   char **argv;
};



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

for(i=0; i < n-1; i++){
	pipe(fd);
	spawn_proc(input, fd[1], cmd + i); //f[1] is the write end of the pipe
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

  return  piping(4, cmd);
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
	return found;
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
	int i = 0;
	char in_key[] = "<";
	char out_key[] = ">";
	int meta_in = 0;
	int meta_out = 0;
	int input_i = 0;
	int output_i = 0;
	while (args[i] != NULL){
		if(strcmp(args[i], in_key)){
			meta_in = 1;
			input_i = i + 1;
		}
		if(strcmp(args[i], out_key)){
			meta_out = 1;
			output_i = i + 1;
		}
		i++;
	}
	
	pid_t pid;
	if ((pid = fork()) < 0){
		printf("ERROR FORKING\n");
		exit(EXIT_FAILURE);
	}
	else if(pid){
		waitpid(pid, NULL, 0);
	}
	else{
		if (meta_in){
			int fd0 = open(args[input_i], O_RDONLY);
			dup2(fd0, STDIN_FILENO);
			close(fd0);
		}
		if (meta_out){
			int fd1 = creat(args[output_i], 0644);
			dup2(fd1, STDOUT_FILENO);
			close(fd1);
		}
		execvp(*args, args);
	}
}

void execute(char **args_list){
	signal (SIGTSTP, handler_function);
	//1 means keep looping, 0 means stop execution
	int iterator;
	char key[] = "exit";
	
	if (args_list[0] == NULL){
		return 1;
	}

	else if(find_in_args(args_list, "|") != -1){

		struct command cmd [] = {args_list};
		return piping(2, cmd);

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
	signal (SIGTSTP, handler_function);
	char *read = NULL;
	size_t buf = 0;
	getline(&read, &buf, stdin); //thank you c++ gods for making this function
	//for testing
	//	std::cout << "Parsed successfully \n";
	return read;
}

char **tokenize(char *line){
	signal (SIGTSTP, handler_function);
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
/*
void shell_loop(){
	
	signal (SIGTSTP, handler_function);
	char **split_args;
	char *read_line;
	//used for terminating while loop
	int running;
	
	do{
		//idk... because that's what shells do for some reason.. 
		//looks fresh I guess
	  printf("> ");
		
		//read line
		read_line = parse();
		
		//separate line into command and args
		split_args = tokenize(read_line);
		
		//execute command
		running = execute(split_args);
		
	}while(running);
}

*/
/*
int main(int argc, char **argv){


	signal (SIGTSTP, handler_function);
	
	//run command loop
	shell_loop();
	
	return EXIT_SUCCESS;
}
*/

int main(){
char cmd [MAX_CMD_SZ];
char* args [MAX_TKN_SZ];
while(1){


printf("SHELL_NAME");
fgets(cmd, MAX_CMD_SZ, stdin);


signal (SIGTSTP, handler_function);
	char **split_args;
	char *read_line;


		read_line = parse();
		
		//separate line into command and args
		split_args = tokenize(read_line);
		
		//execute command
		execute(split_args);

}

}