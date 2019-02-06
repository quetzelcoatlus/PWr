#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>


#define MAX_LENGTH_OF_COMMAND 128
#define MAX_NUMBER_OF_WORDS 64
#define MAX_NUMBER_OF_COMMANDS 16

int myLastCharacterLocationFinder(char* a) {
	int i = 0;

	while (a[i] != '\0') {
		i++;
	}

	return i - 1;
}


char* iRed(char myLine[]){
	char* templine1 = myLine;
	
	int index=0;
	bool iRedirect = false;
	while (templine1[index] != '\0'){
		if (templine1[index] == '<'){
			iRedirect=true;
			break;
		}
		index++;
	}
	
	char* in = NULL;
	if (iRedirect){
		in = strtok(templine1, "<");
		in = strtok(NULL, " ");
	}
	
	return in;
}

char* oRed(char myLine[]){
	char* templine2 = myLine;
	bool oRedirect = false;
	int index=0;
	while (templine2[index] != '\0'){
		if (templine2[index] == '>'){
			oRedirect=true;
			break;
		}
		index++;
	}
	
	char* out=NULL;
	if  (oRedirect){
		out = strtok(templine2, ">");
		out = strtok(NULL, " ");
	}
	
	return out;
}


char*** mySplitLine(char* myline) {	
	char*** splitted = malloc(sizeof(char**) * MAX_NUMBER_OF_COMMANDS);
	char** commands = malloc(sizeof(char*) * MAX_NUMBER_OF_COMMANDS);
	
	char* line = strtok(myline,"<>");
	
	int numberOfCommands=0;
	char* temp = strtok(line,"|");
	
	while (temp != NULL){
		commands[numberOfCommands]=temp;
		temp = strtok(NULL,"|");
		numberOfCommands++;
	}
	
	
	for (int i=0; i < numberOfCommands; i++) {
		int numberOfWords=0;
		
		char** words = malloc(sizeof(char*) * MAX_NUMBER_OF_WORDS);
		
		char* temp2 = strtok(commands[i]," ");

		while (temp2 != NULL){
			words[numberOfWords]=temp2;
			temp2 = strtok(NULL," ");
			numberOfWords++;
		}
		
		splitted[i] = words;
	}
	splitted[numberOfCommands]=NULL;
	
	return splitted;
  
}


void myCommandExecutor(char ***cmd, bool amp,char* in,char* out){
	int p[2];
	pid_t pid;
	int fd_in = 0;
	int status;
	
	int counter=0;
	
	while (*cmd != NULL){
		counter++;
		pipe(p);
		if ((pid = fork()) == -1){
			exit(1);
		} else if (pid == 0) {
			if (counter == 1 && in != NULL){
				int in2 = open(in,O_RDONLY);
				dup2(in2,STDIN_FILENO);
				close(in2);
			} else 
				dup2(fd_in, 0); //Wczytujemy dane z pipe'a (został już użyty, jeśli nie czytamy ze standardowego wejścia)
			if (*(cmd + 1) != NULL) {
				dup2(p[1], 1); //Wyjście ustawiamy na pipe
			} else {
				if (out != NULL){
					int out2 = open(out,O_WRONLY|O_CREAT,0666); 
					dup2(out2,STDOUT_FILENO);
					close(out2);
				}
			}
			close(p[0]); //Zamykamy wejście
			execvp((*cmd)[0], *cmd);
			exit(0);
		} else {
			if (!amp)
				wait(&status); //Czekamy na skończenie pracy dziecka
			close(p[1]);  //Zamykamy wyjście
			fd_in = p[0]; //Ustawiamy wejście na pipe
			cmd++;
		}
	}
}


char* myGetLine(){
	char* line = NULL;
  	ssize_t bufsize = 0; 
 	if (getline(&line, &bufsize, stdin) == EOF) {
        printf("\n");
        exit(0);
    }
  	return line;
}

int main() {
	
	char* line;
	char*** commands;
	char location[512];
	
	getcwd(location, 512);
	do {
		printf("%s: (> ",location);
		line = myGetLine();
		
		if (line[myLastCharacterLocationFinder(line)] == '\n')
			line[myLastCharacterLocationFinder(line)] = '\0';
		
		bool amp=false;
		if (line[myLastCharacterLocationFinder(line)] == '&') {
			line[myLastCharacterLocationFinder(line)] = '\0';
			amp=true;
		}
		
		char line3[MAX_LENGTH_OF_COMMAND];
		strcpy(line3,line);
		
		char line4[MAX_LENGTH_OF_COMMAND];
		strcpy(line4,line);
		
		char* iRedirect = iRed(line3);
		char* oRedirect = oRed(line4);
		
		commands = mySplitLine(line);
		
		if (strcmp(commands[0][0],"cd")==0){
			chdir(commands[0][1]);
			getcwd(location, 512);
			continue;
		} else
		if (strcmp(commands[0][0],"exit")==0){
			printf("\n");
			exit(0);
		} else
		
		myCommandExecutor(commands,amp,iRedirect,oRedirect);
		
		free(line);

	} while (1);
	
	return 0;	
}