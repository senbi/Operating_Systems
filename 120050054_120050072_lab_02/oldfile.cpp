#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <set>
#include <string.h>
#include <map>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#define MAXLINE 1000
using namespace std;


char ** tokenize(char*);
char ** get_input() ;

void sig_usr (int signum)
{
	if (signum==SIGUSR1)
		printf("signal 1 caught\n");
	fflush(NULL);
	return;
}

int main(){
	map<string,pid_t> cprocesses;
	map<string,pid_t>::iterator it;
	map<string,char*> sharedmemory;
	map<string,char*>::iterator sit;
	string s,domain;
	int key = IPC_PRIVATE;
	int sharekey = shmget(key,sizeof(char)*2048,IPC_CREAT | 0777);
	size_t position;
	set<string> domainset;

	while(true)
	{
	char ** tokens = get_input() ;
	
	string command;
	
		

	for(int i=0;tokens[i]!=NULL;i++){
//		getline(cin,s);
		//cin>>s;
		string s1(tokens[i]);

		if (s1=="add_email"){
				
			i++;
			string s(tokens[i]);	
			position = s.find('@');
		if (position>=0)
		{
			domain = s.substr(position+1);
		it = cprocesses.find(domain);
		if(it != cprocesses.end()){
			/*char* addr1 ;
			
			//for(int i=0;i<10;i++)cout<<addr1[i]<<endl;
			// Child process xyz.com - Email address already exists
			
			
			printf("%s\n",addr1);*/
			//string s01;// = "Child process " + domain + " - Email address "+s+" added successfully";
			char* addr1= (char *)shmat(sharekey,NULL,0);
			strcpy(addr1,s.c_str());
			kill(it->second,SIGUSR1);
			sleep(4);
		}
			else{
			cout<<"new child\n";
			pid_t temp=fork();
			pid_t temp1=getpid();
			if(temp>0){cprocesses.insert(pair<string,pid_t>(domain,temp));
				}
			if(temp==0){
			 set <string> email_ids; //userid
			 set<string>::iterator s_it;
			 pid_t parentid = temp1;
			 string domain_name=domain;
			 char* addr1 = (char *)shmat(sharekey,0,0);
			 string s01 = "Child process_ " + domain + " - Email address "+ s + " added successfully";
			 strcpy(addr1,s01.c_str());
			 email_ids.insert(s);
			 printf("%s\n",addr1);

			 while(true)
			{
			 	signal(SIGUSR1,sig_usr);
			 	pause();
			 	addr1= (char *)shmat(sharekey,NULL,0);
			 	string emailid(addr1);
			 	size_t pos = emailid.find(" ");
			 	if(pos==-1){
			 	s_it=email_ids.find(emailid);
			 	if(s_it!=email_ids.end())
			 	{
			 		s01 = "Child process  " + domain_name + " - Email address "+ emailid + " already exists";
			 	}
			 	else
			 	{
			 		email_ids.insert(s);
			 		s01 = "Child process_ " + domain + " - Email address "+ s + " added successfully";
			 	}
			 	 strcpy(addr1,s01.c_str());
			 	 //printf("%s\n",addr1);
			 	 cout<<s01<<endl;
			    }
			   else if (emailid.substr(pos+1)=="0")
			   {
			   	 s_it=email_ids.find(emailid.substr(0,pos));

			   	 if(s_it!=email_ids.end())
			   	 {
			   	 	int i=0;
			   	 	emailid = emailid.substr(0,pos);
			   	 	s_it = email_ids.begin();
			   	 	for ( ; s_it != email_ids.end() ; ++s_it)
			   	 	{
			   	 		if(emailid == (*s_it)) break;
			   	 		else i++;

			   	 		
			   	 	}
			   	 		 string result;
			   	 		 stringstream convert;
			   	 		 convert<<i;
			   	 		 result=convert.str();


			   	 		strcpy(addr1,result.c_str());//intStr02);
						kill(parentid, SIGUSR1);

			   	 	s01 = "Parent process - found the email address "+emailid.substr(0,pos)+" at "+result;//+s_it-email_ids.begin() ; //index
			   	 }
			   	 else 
			   	 {
			   	 	string result ="-1";
			   	 	strcpy(addr1,result.c_str());
			   	 	kill(parentid, SIGUSR1);
			   	 	s01= "Parent process - could not find the email address "+ emailid.substr(0,pos);
			   	 }
			   }
			   else if (emailid.substr(pos+1)=="1")
			   {

			   	s_it=email_ids.find(emailid.substr(0,pos));

			   	 if(s_it!=email_ids.end())
			   	 {
			   	 	int i=0;
			   	 	emailid = emailid.substr(0,pos);
			   	 	s_it = email_ids.begin();
			   	 	for ( ; s_it != email_ids.end() ; ++s_it)
			   	 	{
			   	 		if(emailid == (*s_it)) 
			   	 			{ email_ids.erase(s_it);
			   	 				break;}
			   	 		else i++;

			   	 		
			   	 	}
			   	 		 string result;
			   	 		 stringstream convert;
			   	 		 convert<<i;
			   	 		 result=convert.str();


			   	 		strcpy(addr1,result.c_str());//intStr02);
						kill(parentid, SIGUSR1);
						sleep(4);

						cout << "Child process - child "<<domain_name <<" deleted "<< emailid.substr(0,pos) <<" from position" <<i<<endl;
			   	 	   // s01 = "Parent process - found the email address "+emailid.substr(0,pos)+" at ";//+s_it-email_ids.begin() ; //index
			   	 }
			   	 else 
			   	 {
			   	 	string result ="-1";
			   	 	strcpy(addr1,result.c_str());
			   	 	kill(parentid, SIGUSR1);
					sleep(4);
			   	 	//s01= "Parent process - could not find the email address "+ emailid.substr(0,pos);
			   	 }


			   }
			   	else if (emailid.substr(pos+1)=="1")
			   { 
				   break;
				}
//			 	 printf("%s\n",addr1);
			}
			 _exit(0);
		 }
		}
		
		}
		}

		else if(s1=="search_email")
		{
			i++;
			string s(tokens[i]);	
			position = s.find('@');
		if (position>=0)
		{
			domain = s.substr(position+1);
			it = cprocesses.find(domain);
		if(it != cprocesses.end()){
			
			char* addr1= (char *)shmat(sharekey,NULL,0);
			s=s+" 0";
			strcpy(addr1,s.c_str());
			kill(it->second,SIGUSR1);
			sleep(4);
			signal(SIGUSR1,sig_usr);
	//		pause();
			addr1= (char *)shmat(sharekey,NULL,0);
			string indexofsearch(addr1);
			if(indexofsearch == "-1")cout<<"Parent process - could not find the email address " <<s.substr(0,s.find(" "))<<endl;
			else 
			{
				cout << "Parent process - found the email address "<< s.substr(0,s.find(" ")) <<" at "<<indexofsearch<<endl; 

			}
		}
			else{
				
				cout <<"Parent process - Domain does not exist\n";

			 }
		}
	}

	else if(s1=="delete_email")
	{
		i++;
			string s(tokens[i]);	
			position = s.find('@');
		if (position>=0)
		{
			domain = s.substr(position+1);
			it = cprocesses.find(domain);
		if(it != cprocesses.end()){
			
			char* addr1= (char *)shmat(sharekey,NULL,0);
			s=s+" 1";
			strcpy(addr1,s.c_str());
			kill(it->second,SIGUSR1);
			sleep(4);
			signal(SIGUSR1,sig_usr);
			pause();
			addr1= (char *)shmat(sharekey,NULL,0);
			string indexofsearch(addr1);
			if(indexofsearch == "-1")
				cout<<"Parent process - child "+ domain +" could not find the email address " <<s.substr(0,s.find(" "))<<endl;
			
		}
			else{
				
				cout <<"Parent process - Domain does not exist\n";

			 }
		}
		}
	else if(s1=="delete_domain"){
		i++;
			string s(tokens[i]);	
		if (position>=0)
		{
			domain = s;
			it = cprocesses.find(domain);
		if(it != cprocesses.end()){
			
			char* addr1= (char *)shmat(sharekey,NULL,0);
			s=s+" 2";
			strcpy(addr1,s.c_str());
			kill(it->second,SIGUSR1);
			sleep(4);
			waitpid(-1,NULL,0);
			cout<<"Parent process - Domain "+ domain +" with PID " <<it->second<<endl;
			cprocesses.erase(it);
			
		}
			else{
				
				cout <<"Parent process - Domain does not exist\n";

			 }
		}
		}

}


			
	for(int i ;tokens[i]!=NULL;i++){
		//cout << s[0]<<endl;

		free(tokens[i]);
	}
	free(tokens);
	}
}


char ** tokenize(char* input){
	int i;
	int doubleQuotes = 0;
	
	char *token = (char *)malloc(1000*sizeof(char));
	int tokenIndex = 0;

	char **tokens;
	tokens = (char **) malloc(MAXLINE*sizeof(char*));

	int tokenNo = 0;
	
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
		}
		else if (doubleQuotes == 1){
			token[tokenIndex++] = readChar;
		}
		else if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0; 
			}
		}
		else{
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

char ** get_input() {
	FILE* stream = stdin;
	char input[MAXLINE];
	fflush(stdin) ;

	char *in = fgets(input, MAXLINE, stream); // taking input ;
	if (in == NULL){
		return NULL ;
	}
	// Calling the tokenizer function on the input line    
	char **tokens = tokenize(input);	
	
	// Comment to NOT print tokens
	int i ;
	  
	return tokens ;
}


