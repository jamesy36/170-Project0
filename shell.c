
// Richard Young, James Yang, Jordan Ang
// CS170 SPRING 2017
// C++ implementation of a basic UNIX shell
// shell.cpp
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> //getline
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

sig_atomic_t exit_counter = 0;


void handler_function(int parameter){
	if(exit_counter == 1){
		exit(EXIT_SUCCESS);
	}
	else if(exit_counter == 0){
		exit_counter++;
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
  
int execute(char **args_list){
	signal (SIGTSTP, handler_function);
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

void shell_loop(){
	
	signal (SIGTSTP, handler_function);
	char **split_args;
	char *read_line;
	//used for terminating while loop
	int running;
	
	do{
		//idk... because that's what shells do for some reason.. 
		//looks fresh I guess
	  //  printf("shell> ");
		
		//read line
		read_line = parse();
		
		//separate line into command and args
		split_args = tokenize(read_line);
		
		//execute command
		running = execute(split_args);
		
	}while(running);
}



int main(int argc, char **argv){
	signal (SIGTSTP, handler_function);
	
	//run command loop
	shell_loop();
	
	return EXIT_SUCCESS;
}
