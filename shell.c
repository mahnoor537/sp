#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "PUCITshell:- "
FILE *fd= fopen ("history.txt", "w+");
static int id=0;
int a1,a2;
int parsePipe(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "|"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else { 
        return 1; 
    } 
} 
int execute(char* arglist[]);
char** tokenize(char* cmdline);
char** pipetokenize(char* cmdline);
char* read_cmd(char*, FILE*);
int main(){
     a1=dup(0);
   a2=dup(1);
   char *cmdline;
   char** arglist;
   char* prompt = PROMPT;   
   while((cmdline = read_cmd(prompt,stdin)) != NULL){
     	char* strpiped[2]; 
        int piped = 0; 
        piped = parsePipe(cmdline, strpiped); 	
	if(piped==0)
	{ 
		if((arglist = tokenize(cmdline)) != NULL){
            	execute(arglist);
       //  	need to free arglist
         //	for(int j=0; j < MAXARGS+1; j++)
	         //free(arglist[j]);
         	free(arglist);
         	free(cmdline);
      		}	
	}
	else
	{
		pipetokenize(cmdline);
		free(arglist);
         	free(cmdline);
	}
  }//end of while loop
   printf("\n");
   return 0;
}
int execute(char* arglist[]){
   int status;
   int cpid = fork();
   switch(cpid){
      case -1:
         perror("fork failed");
	      exit(1);
      case 0:
              printf("%s%s\n",arglist[0],arglist[1] );
	      fprintf(fd, "%d %s\n", id, arglist[0]);
              id++;
	      execvp(arglist[0], arglist);
 	      perror("Command not found...");
	      exit(1);
      default:
	      waitpid(cpid, &status, 0);
         printf("child exited with status %d \n", status >> 8);
//                    dup2(a1,0);
  //        dup2(a2,1);
         return 0;
   }
}
char** pipetokenize(char* cmdline){
   char* tkn=strtok(cmdline,"|");
   char** arglist1 = (char**)malloc(sizeof(char*)* (MAXARGS+1));
   for(int j=0; j < MAXARGS+1; j++){
	   arglist1[j] = (char*)malloc(sizeof(char)* ARGLEN);
      	   bzero(arglist1[j],ARGLEN);
    } 
   char** arglist2 = (char**)malloc(sizeof(char*)* (MAXARGS+1));
   for(int j=0; j < MAXARGS+1; j++){
	   arglist2[j] = (char*)malloc(sizeof(char)* ARGLEN);
      	   bzero(arglist2[j],ARGLEN);
    } 
   int count=0,infile,outfile;
   while(tkn!=NULL)
   {
  	  arglist1[count]=tkn;
	  tkn=strtok(NULL,"|");	
	  arglist2[count]=tkn;
	  count++;
	  int fd[2];
   	  pipe(fd);
   	  pid_t cpid = fork();
          if (cpid != 0){  //parent process
          	dup2(fd[1], 1); //redirect stdout to write end of pipe
           	close(fd[0]); //not required so better close it
              	execlp(arglist1[0],arglist1[]);
   	  }
   	  else{  //child process
      		dup2(fd[0], 0); //redirect stdin to read end of pipe
      		close(fd[1]);  //not required so better close it
      		execlp(arglist2[0],arglist2[]);
   	  }
	
   }
   arglist1[count] = NULL;
   arglist2[count] = NULL;
}      

char** tokenize(char* cmdline){
   char* tkn=strtok(cmdline," ");
   char** arglist = (char**)malloc(sizeof(char*)* (MAXARGS+1));
   for(int j=0; j < MAXARGS+1; j++){
	   arglist[j] = (char*)malloc(sizeof(char)* ARGLEN);
      	   bzero(arglist[j],ARGLEN);
    } 
   int count,infile,outfile;
   while(tkn!=NULL)
   {
      if(strstr(tkn,"<")){
      tkn = strtok (NULL, " ");
      printf("file name %s\n",tkn );
      infile = open(tkn, O_RDONLY);
      dup2(infile, STDIN_FILENO);
      close(infile);
      tkn = strtok (NULL, " ");
      continue;
      }
	else if (strstr(tkn,">"))
	{
		tkn=strtok(NULL," ");
		outfile=open(tkn,O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR,S_IWUSR,S_IXUSR);
		dup2(outfile,STDOUT_FILENO);
		close(outfile);
		tkn=strtok(NULL," ");
		continue;
	}
        else if (strstr(char(tkn[0]),"!"))
        {
        	int n=0;
		for (int i=1;i<strlen(tkn);i++)
		{
			 
		}
        }
        arglist[count]=tkn;
	count++;
	tkn=strtok(NULL," ");
   }
   arglist[count] = NULL;
   return arglist;
}      

char* read_cmd(char* prompt, FILE* fp){
   printf("%s", prompt);
  int c; //input character
   int pos = 0; //position of character in cmdline
   char* cmdline = (char*) malloc(sizeof(char)*MAX_LEN);
   signal(SIGINT, SIG_IGN);
   while((c = getc(fp)) != EOF){
       if(c == '\n')
	  break;
       cmdline[pos++] = c;
   }
//these two lines are added, in case user press ctrl+d to exit the shell
   if(c == EOF && pos == 0) 
      return NULL;
   cmdline[pos] = '\0';
   return cmdline;
}
