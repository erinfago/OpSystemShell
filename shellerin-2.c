#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>



//checks to see if the command is built in
int isBuiltin(char *cmd){
	if(! strcmp(cmd, "exit")){
		return 1;
	}
	
	else if(! strcmp(cmd,"cd")){
		return 1;
	}
	
	else{
		return 0;
	}
}

//does the built in command
void doBuiltin(char *cmd, char *path){
	if(! strcmp(cmd, "exit")){
		exit(0);
	}
	
	else if(! strcmp(cmd,"cd")){
		chdir(path);
	}
}

//main loop
char *readParseEx(void){
	//set up
	while(1){
		//set size of expected input and output request for user input
		printf("pi@raspberrypi:~ $ ");
		int bufsize = 10;
		char *buffer = malloc(sizeof(char) * bufsize);
		int pos = 0;

////////////////////////////////////////////////////////////////////////
		//reading 
		char c = getchar();
		
		while(c !='\n'){	
			//take in each char and read in, if bigger than initially allocated, reallocate
			buffer[pos++] = c;
			c = getchar();
			if(pos >= bufsize){
				bufsize = bufsize * 10;
				buffer = realloc(buffer, bufsize);		
			}
		}
		buffer[pos] = '\0'; // null byte
				
///////////////////////////////////////////////////////////////////////	
		//parse
		
		//use to parse through input
		char *token;
		
		//array of params
		int arrbuf = 10;
		char **paramArr = malloc(sizeof(char*) * arrbuf);
			
		
		//get first strtok which will be the command
		char *cmd = strtok(buffer, " ");
		
		// make sure cmd is also in param arr
		int posit = 0;
		paramArr[posit] = cmd;
		posit++;
		
		//get next
		token = strtok(NULL, " ");
		
		//go through each token after command to see what inputs are given
		while(token != NULL){
			//store token in input array
			paramArr[posit] = token;
			
			//increase position in array where next will be stored
			posit++;
			
			//check to see if array keeping track of input parameters is big enough
			if (posit >= arrbuf){
				arrbuf = arrbuf * 5;
				paramArr = realloc(paramArr, arrbuf);
			}
			
			//get next token
			token = strtok(NULL, " ");
		}

		//now we have an array filled with the command and the different inputs that were parsed
			
///////////////////////////////////////////////////////////////////////
		//execute
		
		//for waitpid
		int status;
		
		// if no command given
		if(cmd == NULL){
			 continue;
		}
		
		//check if built in command and give first param (bc cd would only make use of the first one and exit does not require anything
		else if(isBuiltin(cmd)){
			doBuiltin(cmd, paramArr[1]);
		}
		
		//if not, try to run and see what execve returns
		else{
			if(fork() != 0){
				waitpid(-1, &status, 0); // Parent
			} 
			else {
				int IsSuccess = execvp(cmd, paramArr); // Child
				
				//execve will return -1 if the process fails/error occurred
				if (IsSuccess == -1) {
					printf("Unknown command.\n");
				}
			}
		}	
	}
}

int main(){
	char *out = readParseEx();
}
