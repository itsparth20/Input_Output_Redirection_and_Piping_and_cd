//Developed by Parth Patel
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <wait.h>
#include <error.h>

int checkcd(char const *, char const *);

int gotocd(char *cdline);

void checkforothercommand(char*);

char** analysstring(char*);

int main(){

	char dataarray[200];
	char *a = " ";
	char *dolarsign = "$";

	char *tok;
	signal(SIGINT, SIG_IGN);
	tok = strtok (dataarray," ");

	while(dataarray != NULL){
		bzero(dataarray, 200);
		printf("%s%s",dolarsign,a);
		fgets(dataarray, 200, stdin);

		dataarray[strlen(dataarray)-1] = '\0';              

		if (strcmp(dataarray, "exit") == 0){         
			exit(0);
		}
		
		if(checkcd(dataarray,"cd") == 0){
			tok = strchr(dataarray,' '); 

			if(tok) {
				char *tempvalue = tok + 1;
				tok = tempvalue;
				char *checklast = strchr(tok, '\n');

				if(checklast) {
					*checklast = '\0';
				}
				gotocd(tok);
			}
		}

		else{
			checkforothercommand(dataarray);
		}
	}

	return 0;
}

int checkcd(char const *cdv, char const *cde)
{
	int i = 0;

	for(i = 0;cde[i];i++)
	{

		if(cdv[i] != cde[i])

		    return -1;

	}

	return 0;
}

int gotocd(char *cdline){

	char path[200];
	strcpy(path,cdline);

	char cwd[200];
	getcwd(cwd,sizeof(cwd));

	if(cdline[0] != '/')
	{
		strcat(cwd,"/");
	}
	strcat(cwd,path);
	chdir(cwd);

	return 0;
}

void checkforothercommand(char* commandline)
{

	int proces, filerror, i = 0;
	int pipevalue[2];

	char* pipeid = 0, *cara = 0, **argspi = 0, **argsci = 0, **mainargs, **backupS;

	backupS = malloc(strlen(commandline) + 1);
	strcpy(backupS, commandline);
	mainargs = analysstring(backupS);      

	proces = fork();       
    
	if (proces == -1)    
	{
		perror("Error -> fork\n");
		exit(0);
	}

	if (proces == 0)     
	{
		char symbolVal = '\0';

		while(mainargs[++i] != '\0')  
		{

			if (mainargs[i][0] == '>')
			{
				symbolVal = mainargs[i][0];
				break;
			}

			if(mainargs[i][0] == '<'){
				symbolVal = mainargs[i][0];
				break;
			}

			if(mainargs[i][0] == '|'){
				symbolVal = mainargs[i][0];
				break;
			}
		}

		if (symbolVal == '<')      
		{
			close(0);       
			filerror = open(mainargs[i+1], O_RDONLY );

			if (filerror == -1)
			{
				int temp11 = i+1;
				perror(mainargs[temp11]);
				exit(0);
			}

			mainargs[i] = '\0';
		}

		if (symbolVal == '>')  
		{
			close(1);   
			filerror = open(mainargs[i+1], O_WRONLY | O_CREAT, 0744);

			if (filerror == -1)
			{
				int temp21 = i+1;
				perror(mainargs[temp21]);
				exit(0);
			}

			mainargs[i] = '\0';
		}

		if (symbolVal == '|')  
		{

			if (pipe(pipevalue))
			{
				perror("pipe failed.\n");
				exit(0);
			}
			int id = fork();

			if( id > 0 )             
			{
				close(pipevalue[1]);
				dup2(pipevalue[0], 0);
				close(pipevalue[0]);
				pipeid = malloc(strlen(commandline) + 1);
				strcpy(pipeid, strchr(commandline, '|') + 1);
				argspi = analysstring(pipeid);
				execvp(argspi[0], argspi);   
				perror("Not able to create pipe.");
				exit(0);          
			}  

			else if(id == 0)    
			{	
				close(pipevalue[0]);
				dup2(pipevalue[1], 1);
				close(pipevalue[1]);
				cara = malloc(strlen(commandline) + 1);
				strcpy(cara, commandline);
				argsci = analysstring(cara);
				argsci[i]= '\0'; 
				execvp(argsci[0], argsci); 
				perror("Not able to create pipe.\n");
				exit(0);         
			}
			
			else
			{
				perror("Not able to create pipe.");
				exit(0);
			}
		}

		if(symbolVal != '|')
		{	
			execvp(mainargs[0], mainargs);
			perror("Some problem occure->try again\n"); 
			exit(0); 
		}
	}

	else                        
	{
		wait(&proces);                  
	}
}

char** analysstring(char* commandline)
{
	int i = 0;
	char** dataspace = malloc(260);
	dataspace[i] = strtok(commandline, " ");

	while(dataspace[i])
	{
		i=i+1;
		dataspace[i] = strtok(NULL, " ");
	}

	return dataspace;
}

