// Richard Young, James Yang, Jordan Ang
// CS170 SPRING 2017
// C++ implementation of a basic UNIX shell
// shell.cpp
#include <iostream>
#include <cstdio> 
#include <cstdlib>
#include <string> //getline
#include <cstring>

int execute(char **args){
	return 0;
}

char *parse(){
	//TODO: test?
	size_t buf = 0;
	std::string read;
	std::getline(std::cin, read); //thank you c++ gods for making this function
	char *read_c = new char[read.length() + 1];
	std::strcpy (read_c, read.c_str());
	
	return read_c;
}

char **tokenize(char *line){
	//TODO: implement
	char *token;
	char **tokens;
	char delimeter[] = " \t;\n";
	
	return tokens;
}

void shell_loop(){
	
	char **split_args;
	char *read_line;
	//used for terminating while loop
	int running;
	
	do{
		//idk... because that's what shells do for some reason..\
		looks fresh
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