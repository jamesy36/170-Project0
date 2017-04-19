// Richard Young, James Yang, Jordan Ang
// CS170 SPRING 2017
// C++ implementation of a basic UNIX shell
// shell.cpp

char *parse(){
	//TODO: implement...
	size_t buf = 0;
	char *line;
	
}

void tokenize(char *line){
	//TODO: implement
}

void shell_loop(){
	
	char **split_args;
	char *read_line;
	//used for terminating while loop
	int running;
	
	do{
		//idk... because that's what shells do for some reason..\
		looks fresh
		cout << "> ";
		
		//read line
		read_line = parse();
		
		//separate line into command and args
		split_args = tokenize(read_line);
		
		//execute command
		running = execute(split_args);
		
	}while(running)
}

int main(int argc, char **argv){
	
	//run command loop
	shell_loop();
	
	return EXIT_SUCCESS
}
