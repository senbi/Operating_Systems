#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
#define MAXLINE 1000
#define DEBUG 0

/* Function declarations and globals */
int parent_pid,exitflag=0 ; // parent id is id of main process 
char ** tokenize(char*) ;
int execute_command(char** tokens) ;
int seq_fail =0 ;
const char * const path = "";

void child_handler(int signum) //this the signal handler when child recieves any signal
{
	return ;
}
void seq_handler(int signum) // this is used to check whether there is an sequtial fail in system
{
	seq_fail=1;
	return ;
}

int main(int argc, char** argv){
	parent_pid = getpid() ;					
	struct sigaction sam;					// signal handler for SIGINT SIQUIT
	sam.sa_flags=0;
	sam.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sam,NULL);
	sigaction(SIGQUIT, &sam ,NULL);
	struct sigaction sam3;					// signal handler for sequential interrupt
	sam3.sa_flags=0;
	sam3.sa_handler = &seq_handler;
	sigaction(SIGUSR1,&sam3,NULL);
	//if (sigaction(SIGINT, &sam,NULL) == 0){
	//	kill(-parent_pid,SIGQUIT);};
	//sigaction(SIGQUIT, &sam ,NULL);
	//if (sigaction(SIGQUIT, &sam,NULL) == 0){
	//	kill(-parent_pid,SIGQUIT);};

	/* Set (and define) appropriate signal handlers */
	/* Exact signals and handlers will depend on your implementation */
	// signal(SIGINT, quit);
	// signal(SIGTERM, quit);

	char input[MAXLINE];
	char** tokens;
	
	while(1) { 
		printf("$ "); // The prompt
		fflush(stdin);

		char *in = fgets(input, MAXLINE, stdin); // Taking input one line at a time
		//Checking for EOF
		if (in == NULL){
			if (DEBUG) printf("jash: EOF found. Program will exit.\n");
			break ;
		}

		// Calling the tokenizer function on the input line    
		tokens = tokenize(input);	
		// Executing the command parsed by the tokenizer
		execute_command(tokens) ; 
		
		// Freeing the allocated memory	
		int i ;
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
		
		if(exitflag) break;
	}
	
	/* Kill all Children Processes and Quit Parent Process */
	return 0 ;
}

/*The tokenizer function takes a string of chars and forms tokens out of it*/
char ** tokenize(char* input){
	int i, doubleQuotes = 0, tokenIndex = 0, tokenNo = 0 ;
	char *token = (char *)malloc(MAXLINE*sizeof(char));
	char **tokens;

	tokens = (char **) malloc(MAXLINE*sizeof(char*));

	for(i =0; i < strlen(input); i++){
		char readChar = input[i];

		if (readChar == '"'){
			doubleQuotes = (doubleQuotes + 1) % 2;
			if (doubleQuotes == 0){
				token[tokenIndex] = '\0';
				if (tokenIndex != 0){
					tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
					strcpy(tokens[tokenNo++], token);
					tokenIndex = 0; 
				}
			}
		} else if (doubleQuotes == 1){
			token[tokenIndex++] = readChar;
		} else if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0; 
			}
		} else {
			token[tokenIndex++] = readChar;
		}
	}

	if (doubleQuotes == 1){
		token[tokenIndex] = '\0';
		if (tokenIndex != 0){
			tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
			strcpy(tokens[tokenNo++], token);
		}
	}

	tokens[tokenNo] = NULL ;
	return tokens;
}



int execute_command(char** tokens) {
	/* 
	Takes the tokens from the parser and based on the type of command 
	and proceeds to perform the necessary actions. 
	Returns 0 on success, -1 on failure. 
	*/
	if (tokens == NULL) {
		return -1 ; 				// Null Command
	} else if (tokens[0] == NULL) {
		return 0 ;					// Empty Command
	} else if (!strcmp(tokens[0],"exit")) {
		exitflag=1;
		/* Quit the running process */
		return 0 ;
	} else if (!strcmp(tokens[0],"cd")) {
		/* Change Directory, or print error on failure */
		int ret;
		ret=chdir (tokens[1]);
		if(ret == -1) {
			//printf ("Changing directory to <%s>\n", tokens[1]);  
			printf ("cd failed - %s\n", strerror (errno));
			fflush(stdout);
			seq_fail = 1;
			//return 105;
		}else {
		 //system("pwd");
		 //printf ("Done \n");
		}		
		return 0 ;
	} else if (!strcmp(tokens[0],"parallel")) {
		/* Analyze the command to get the jobs */
		/* Run jobs in parallel, or print error on failure */

		//char** commands;
		int j=1,k=0;
		for(;tokens[j]!=NULL;j++) // this is where we are excracting the commands between :::
			{
				char command_now[MAXLINE];// =(char *)malloc(MAXLINE*sizeof(char));;
				strcpy(command_now,"");
				while(strcmp(tokens[j],":::")) //checking for ":::" token
				{			
						
					 	strcat(command_now,tokens[j]);
					 	strcat(command_now," ");
					 	j++;
					 	if (tokens[j]==NULL) break;				 
				}
				//printf("%s\n",command_now);
				char ** tokens3; // this where we found the command we are tokenizeing in token3
				tokens3 = tokenize(command_now);
				k++;
				if (fork()==0) // creating a new process for command obtained above
				{
					execute_command(tokens3); //executing the command 
					exit(0);
				}				

				int i ;
					for(i=0;tokens3[i]!=NULL;i++){ // we are feeing the tokens so that we can use that memory again
						free(tokens3[i]);
					}
					free(tokens3);
					
					//free(command_now);
					strcpy(command_now,"");
				}
			while(k){ // waiting all the childprocess to end
				waitpid(-1,0,0);
				k--;
			}

		return 0 ;
	} else if (!strcmp(tokens[0],"sequential")) {
		/* Analyze the command to get the jobs */
		/* Run jobs sequentially, print error on failure */
		/* Stop on failure or if all jobs are completed */

		int j=1;
		for(;tokens[j]!=NULL;j++)
			{	
				char command_now[MAXLINE];// =(char *)malloc(MAXLINE*sizeof(char));;
				strcpy(command_now,"");
				while(strcmp(tokens[j],":::")) // searching for commands between the tokens
				{			
						
					 	strcat(command_now,tokens[j]);
					 	strcat(command_now," ");
					 	j++;
					 	if (tokens[j]==NULL) break;				 
				}
				//printf("%s\n",command_now);
				char ** tokens3;
				tokens3 = tokenize(command_now); // tokenizing the obtained command
				/*printf("%s\n",command_now);
				for(int i=0;tokens3[i]!=NULL;i++){
						printf("\n",tokens3[i]);
					}*/		

				if (seq_fail==1){seq_fail=0;break;} //checking whether there is any sequential break ?
				execute_command(tokens3);

				int i ;
					for(i=0;tokens3[i]!=NULL;i++){ // feeing the tokens 
						free(tokens3[i]);
					}
					free(tokens3);
					
					//free(command_now);
					strcpy(command_now,"");
					
				}				
		return 0 ;					// Return value accordingly
	} else {
		/* Either file is to be executed or batch file to be run */
		/* Child process creation (needed for both cases) */
		int pid = fork(),w ;
		int *status;
		if (pid == -1)
		{
			printf ("child execution - %s\n", strerror (errno));
			fflush(stdout);
			seq_fail = 1;
			//return 110;
		}
		if (pid == 0) {
			pid_t parentid = parent_pid;
			struct sigaction sam2;
			sam2.sa_flags=0;
			sam2.sa_handler = SIG_DFL;
			sigaction(SIGINT, &sam2,NULL);
			sigaction(SIGQUIT, &sam2 ,NULL);
			//sam.sa_handler = &child_handler;
			if (!strcmp(tokens[0],"run")) {
				/* Locate file and run commands */
				/* May require recursive call to execute_command */
				/* Exit child with or without error */
				//system("pwd");
				FILE *pfile;
				pfile=fopen(tokens[1],"r");
				char input2[MAXLINE];
				char** tokens2;
	
				while(1) { 
					char *in2 = fgets(input2, MAXLINE, pfile); // Taking input one line at a time
					//Checking for EOF
						if (in2 == NULL){
							if (DEBUG) {printf ("run failed - %s\n", strerror (errno));fflush(stdout);}
							//seq_fail = 1;
							kill(parentid,SIGUSR1);
							break;
							//return 115;

						}
						// Calling the tokenizer function on the input line    
						tokens2 = tokenize(input2);	
						// Executing the command parsed by the tokenizer
						execute_command(tokens2) ; 		
						// Freeing the allocated memory	
						int i ;
						for(i=0;tokens2[i]!=NULL;i++){
							free(tokens2[i]);
						}
						free(tokens2);
				}
				fclose(pfile);
				
				exit (0) ;
			}
			else {
				/* File Execution */
				/* Print error on failure, exit with error*/
				int result = execvp(tokens[0],tokens);
				/* Print error on failure, exit with error*/
				if (result == -1)
				{
					printf ("file execution - %s\n", strerror (errno)); 

					//seq_fail = 1;
					fflush(stdout);
					kill(parentid,SIGUSR1);
					//exit(0) ;
					//return 120;
				}
				
				exit(0) ;
				
			}
		}
		else {
			/* Parent Process */
			/* Wait for child process to complete */
			w = waitpid(pid, status, WUNTRACED | WCONTINUED);
            
		}
	}
	return 1;
}
