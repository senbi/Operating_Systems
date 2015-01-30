#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <fcntl.h>
#include <vector>

#include <map>

using namespace std;
#define MAXLINE 1000
#define DEBUG 0
map<pid_t,string> back_process;
/* Function declarations and globals */
int parent_pid,exitflag=0 ; // parent id is id of main process 
char ** tokenize(char*) ;
int execute_command(char** tokens) ;
bool contains(char** tokens,char* s);
const char * const path = "";
struct parseline{
	int hour;
	int minute;
	char* commands;
};
int gettime(char* s){
	if(!strcmp(s,"*"))return -1;
	return atoi(s);

}
parseline* f(char* s){
	char ** tokens = tokenize(s);
	parseline* ans;
	ans->hour = gettime(tokens[0]);
	ans->minute = gettime(tokens[1]);
	strcpy(ans->commands,"");
	for(int i=2;tokens[i] != NULL ; ++i){
		strcat(ans->commands,tokens[i]);
		strcat(ans->commands," ");
	}
	return ans;
}
void child_handler(int signum) //this the signal handler when child recieves any signal
{
	return ;
}
void background_handler(int signum){
	map<pid_t,string>::iterator b_it;
	switch(signum){
		case SIGCHLD :
			int cause;
			pid_t child_pid = waitpid(-1,&cause,WNOHANG);
			b_it=back_process.find(child_pid);
			if(b_it != back_process.end()){
				cout<<b_it->first<<"\t Done"<<"\t\t"<<b_it->second<<endl;
			//	fflush(stdout);
				back_process.erase(b_it);
			//	cout<<back_process.size()<<"\n";
			}

	}
	return;
}
int main(int argc, char** argv){
	parent_pid = getpid() ;					
	struct sigaction sam,sam1;					// signal handler for SIGINT SIQUIT
	sam.sa_flags=0;
	sam.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sam,NULL);
	sigaction(SIGQUIT, &sam ,NULL);
	sam1.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	sam1.sa_handler = &background_handler;
	sigaction(SIGCHLD,&sam1,NULL);
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
bool contains(char** tokens , char* s){
	unsigned int i;
	for (i = 0; tokens[i] != NULL; ++i)
	{
		if(!strcmp(tokens[i],s))return true;
	}
	return false;
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
	} else if(contains(tokens,"&")){
		char command_now[MAXLINE];// =(char *)malloc(MAXLINE*sizeof(char));;
		strcpy(command_now,"");
		int j =0;
		pid_t c_id,p_id = getpid();
		while(strcmp(tokens[j],"&")) //checking for ":::" token
		{			
				
			 	strcat(command_now,tokens[j]);
			 	strcat(command_now," ");
			 	j++;
			 	if (tokens[j]==NULL) break;				 
		}
		char ** tokens3; // this where we found the command we are tokenizeing in token3
		tokens3 = tokenize(command_now);
			pid_t child_pid = fork(),pp_id=p_id;
			if(child_pid>0){
				string pname(command_now);
				c_id = child_pid;
				back_process.insert(pair<pid_t,string>(c_id,pname));
				cout<<"["<<c_id<<"]\n";
			}
			if (child_pid == 0)
			{	//extern map<pid_t,string> back_process;
				extern void background_handler(int signum);
				struct sigaction sam1;
				sam1.sa_flags = SA_RESTART | SA_NOCLDSTOP;
				sam1.sa_handler = &background_handler;
				sigaction(SIGCHLD,&sam1,NULL);
				execute_command(tokens3);
				exit(0);
			}
	//cout<<"it has &"<<endl;
			return 0;
	}
	 else if (!strcmp(tokens[0],"exit")) {
		exitflag=1;
		/* Quit the running process */
		return 0 ;
	} else if(contains(tokens,"|")){
		//cout<<"contains | \n";
		int fds[2];
		int check = pipe (fds);
		if(check == -1){
			perror("in pipe");
			exit(1);
		}
		//cout << "aaaaaaaaaaaa\n";
		char command_now[MAXLINE];// =(char *)malloc(MAXLINE*sizeof(char));;
		strcpy(command_now,"");
		int j =0;
		while(strcmp(tokens[j],"|")) //checking for ":::" token
		{			
				
			 	strcat(command_now,tokens[j]);
			 	strcat(command_now," ");
			 	j++;
			 	if (tokens[j]==NULL) break;				 
		}
		char ** tokens3; // this where we found the command we are tokenizeing in token3
		tokens3 = tokenize(command_now);
		/*for (int i = 0; tokens3[i] != NULL; ++i)
		{
			cout<<tokens3[i]<<endl;
		}*/
		strcpy(command_now,"");
		j++;
		while(tokens[j] != NULL) //checking for ":::" token
		{			
				
			 	strcat(command_now,tokens[j]);
			 	strcat(command_now," ");
			 	j++;
			 	if (tokens[j]==NULL) break;				 
		}
		char ** tokens4;
		tokens4 = tokenize(command_now);
		/*for (int i = 0; tokens4[i] != NULL; ++i)
		{
			cout<<tokens4[i]<<endl;
		}*/
		int pid_1, pid_2 ;
		pid_1 = fork() ;
		if (pid_1==0) // creating a new process for command obtained above
		{
			
			close(fds[0]);
			int verify = dup2 (fds[1], 1);
			if(verify == -1){
				perror("dup2 error");
			}
			//execvp(tokens3[0], tokens3);
			execute_command(tokens3); //executing the command 
			exit(0);
		}
		pid_2 = fork() ;
		if (pid_2==0) // creating a new process for command obtained above
		{
			close (fds[1]);
			int verify = dup2 (fds[0], 0);
			if(verify == -1){
				perror("dup2 error");
			}
			//execvp(tokens4[0], tokens4);
			
			execute_command(tokens4); //executing the command 
			close(0);
			exit(0);
		}
		close(fds[0]) ;
		waitpid(-1,NULL,0);
		close(fds[1]) ;
		waitpid(-1,NULL,0);

	} else if (!strcmp(tokens[0],"cd")) {
		/* Change Directory, or print error on failure */
		int ret;
		ret=chdir (tokens[1]);
		if(ret == -1) {
			//printf ("Changing directory to <%s>\n", tokens[1]);  
			printf ("cd failed - %s\n", strerror (errno));
			fflush(stdout);
			return -1;
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

		int j=1,tag;
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
				tag = execute_command(tokens3);
	//			cout<<"tag::"<<tag<<endl;
				if (tag==-1){break;} //checking whether there is any sequential break ?
				

				int i ;
					for(i=0;tokens3[i]!=NULL;i++){ // feeing the tokens 
						free(tokens3[i]);
					}
					free(tokens3);
					
					//free(command_now);
					strcpy(command_now,"");
					
				}				
		return 0 ;					// Return value accordingly
	}else if (!strcmp(tokens[0],"sequential_or")) {
		/* Analyze the command to get the jobs */
		/* Run jobs sequentially, print error on failure */
		/* Stop on failure or if all jobs are completed */

		int j=1,tag;
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
				tag = execute_command(tokens3);
	//			cout<<"tag::"<<tag<<endl;
				if (tag==0){break;} //checking whether there is any sequential break ?
				

				int i ;
					for(i=0;tokens3[i]!=NULL;i++){ // feeing the tokens 
						free(tokens3[i]);
					}
					free(tokens3);
					
					//free(command_now);
					strcpy(command_now,"");
					
				}				
		return 0 ;					// Return value accordingly
	}
	else if (!strcmp(tokens[0],"cron"))
	{		
			vector<parseline*> myvector;

				FILE *pfile;
				pfile=fopen(tokens[1],"r");
				if (pfile == NULL)
				{
					printf ("cron failed - %s\n", strerror (errno));
					fflush(stdout);
				}
				char input2[MAXLINE];
				char** tokens2;
	
				while(1) { 
					char *in2 = fgets(input2, MAXLINE, pfile); // Taking input one line at a time
					//Checking for EOF
						if (in2 == NULL){
							
							break;
						}
						// Calling the tokenizer function on the input line    
						parseline* x ;
						x=f(in2);
						myvector.push_back(x) ;
				}
				fclose(pfile);

				if (fork()==0)
				{
						while(1){
						time_t mytime;
	    				mytime = time(NULL);
						tm* nowt = localtime(&mytime);
						int min_now = nowt->tm_min ;
						int hour_now = nowt->tm_hour ;

						vector<parseline*>::iterator it ;
						for( it=myvector.begin();it!=myvector.end();it++)
						{	
							if( ( ((*it)->hour ==hour_now) && ((*it)->minute==min_now) )
								|| ( ((*it)->hour ==-1) && ((*it)->minute==min_now))
								|| ( ((*it)->hour ==-1) && ((*it)->minute==-1)) 
								)
							{
								if(fork()==0)
								{
								   char ** tokens20 = tokenize((*it)->commands);
								   // Freeing the allocated memory	
								   execute_command(tokens20) ; 
									int i ;
									for(i=0;tokens20[i]!=NULL;i++){
										free(tokens20[i]);
									}
									free(tokens20);	
									exit(0);
								}
							// Executing the command parsed by the tokenizer						
							
							}
							}

							sleep(60);

					}

				// ikkada fork ?? fork();
				}
				
		
	}
	 else {
		/* Either file is to be executed or batch file to be run */
		/* Child process creation (needed for both cases) */
		int pid = fork(),w ;
		int status=0;
		if (pid == -1)
		{
			printf ("child execution - %s\n", strerror (errno));
			fflush(stdout);
			return -1;
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
				if (pfile == NULL)
				{
					printf ("run failed - %s\n", strerror (errno));fflush(stdout);
					//kill(parentid,SIGUSR1);
					exit(-1);
				}
				char input2[MAXLINE];
				char** tokens2;
	
				while(1) { 
					char *in2 = fgets(input2, MAXLINE, pfile); // Taking input one line at a time
					//Checking for EOF
						if (in2 == NULL){
							
							break;
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
				int i;
				int input = -1; //   <
				int output =-1; //   >
				int append =-1;  //  >>
				int errorrr =0;
				for(i=0;tokens[i]!=NULL;i++){
					if (!strcmp(tokens[i],"<"))
					{  tokens[i]=NULL; 
						if (input!=-1) errorrr++;
						input = i;
						
						//cout << "Workinf 123456789" << endl;
					}
					else if (!strcmp(tokens[i],">"))
					{
						tokens[i]=NULL; 
						if (output!=-1) errorrr++;
						output = i;
											}
					else if (!strcmp(tokens[i],">>"))
					{
						tokens[i]=NULL; 
						if (append!=-1) errorrr++;
						append = i;
						
					}
				}

				 //FILE *fdopen(int fd, const char *mode);	
				int fdsa,fdsi,fdso;
				 if (errorrr!=0)
				{
					fprintf(stderr,"More no of redirections\n");
					exit(EXIT_FAILURE);
				}		
				
					if (input!=-1)
				{   
					 fdsi =open(tokens[input+1],O_RDONLY );	
					dup2 (fdsi , STDIN_FILENO);
				}
				if (output!=-1)  // > 
				{	
					 fdso =open(tokens[output+1],O_WRONLY|O_CREAT|O_TRUNC , S_IRWXU);					
					dup2 (fdso, STDOUT_FILENO);
				}
				 if (append!=-1)
				{	
					 fdsa =open(tokens[append+1],O_APPEND|O_WRONLY|O_CREAT , S_IRWXU);
					dup2 (fdsa , STDOUT_FILENO);
				}
				

				
				int result = execvp(tokens[0],tokens);  
				
				if (input!=-1)
				{   
					close(fdsi);
				}
				if (output!=-1)  // > 
				{	
					close(fdso);
				}
				 if (append!=-1)
				{	
					close(fdsa);
				}
				/* Print error on failure, exit with error*/
				if (result == -1)
				{
					printf ("file execution - %s\n", strerror (errno)); 

					//seq_fail = 1;
					fflush(stdout);
					kill(parentid,SIGUSR1);
					exit(EXIT_FAILURE);
					//exit(0) ;
					//return 120;
				}
				
				exit(0) ;
				
			}
		}
		else {
			/* Parent Process */
			/* Wait for child process to complete */
			w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
		//	cout<<w<<":"<<WEXITSTATUS(status)<<endl;
			 if(WEXITSTATUS(status) == 0){
		//		cout<<"chid process failed\n";
			 	return 0;
             }else return -1;
		}
	}
	return 1;
}
