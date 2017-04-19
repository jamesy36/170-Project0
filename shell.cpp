// Richard Young, James Yang, Jordan Ang
// CS170 SPRING 2017
// C++ implementation of a basic UNIX shell
// shell.cpp
#include <iostream>
#include <cstdio> 
#include <cstdlib>
#include <string> //getline
#include <cstring>

int execute(char **args_list){
	//1 means keep looping, 0 means stop execution
	int iterator;
	
	if (args_list[0] == NULL){
		return 1;
	}
	
	//temp loop for testing
	for (iterator = 0; iterator < 10; iterator++){
		std::cout << "Print first 10 args, or until null char: \n";
		if (args_list[iterator] != NULL){
			std::cout << args_list[iterator] << "\n";
		}
	}
	//check for built-ins
	//if built-in, run it
	//else skip to end
	return 1; //change to launch program
}

char *parse(){
	//TODO: test? it compiles
	std::string read;
	std::getline(std::cin, read); //thank you c++ gods for making this function
	char *read_c = new char[read.length() + 1];
	std::strcpy (read_c, read.c_str());
	
	return read_c;
}

char **tokenize(char *line){
	//TODO: test? 
	//int buf = 640; //maybe reallocate when full later?
	char *token;
	char **tokens;
	char delimeter[] = " \t;\n";
	int i = 0;
	
	token = strtok(line, delimeter);
	while (token != NULL){
		tokens[i] = token; // potentially uninitialized but ignoring for now
		i++;
		token = strtok(NULL, delimeter);
	}
	tokens[i] = NULL;
	return tokens;
}

void shell_loop(){
	
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
	
	//run command loop
	shell_loop();
	
	return EXIT_SUCCESS;
}
