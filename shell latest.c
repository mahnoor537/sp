#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "mahnoor@ubuntu"
int job[200];
int jobId=0;
int a1,a2;
int execute(char* arglist[], bool bg);
char** tokenize(char* cmdline);
char** pipetokenize(char* cmdline);
char* read_cmd(char*, FILE*);
void handler1(int n);
void handler2(int n);
int countChars( char* s, char c );
int main(int argc,char *argv[]){
  a1=dup(0);
  a2=dup(1);
  char *cmdline;
  char** arglist;
  char* prompt = PROMPT;  
  bool bg=false;
  /*if((strcmp(argv[1],"-f")==0) && (argc==3) )
  {
      arglist=tokenize(argv[2]);
      execute(arglist, false);
  }
  else
  {*/
  while((cmdline = read_cmd(prompt,stdin)) != NULL){  
    char* re = strchr(cmdline, '|');
  if(re==NULL)
  { 
    if((arglist = tokenize(cmdline)) != NULL){   
      if(strcmp(arglist[0],"cd")==0)
      {
        if (arglist[1] == NULL ) 
            chdir(getenv("HOME")); 
        else if (chdir(arglist[1])==-1) 
          printf(" %s: no such directory\n", arglist[1]);
      }
      else if(strcmp(arglist[0],"exit")==0)
      {
        return 0;
      }
      else if(strcmp(arglist[0],"job")==0)
      {
        if(jobId>0){
          for(int i=0;i<jobId;i++){
            printf("[%d]    %d\n",i+1, job[i]);
          }
        }
      }
      else if(strcmp(arglist[0],"kill")==0)
      {
        int n=atoi(arglist[1]);
        kill(n, SIGKILL);
      }
      else if(strcmp(arglist[0],"help")==0)
      {
        printf("The availale built-in commands are:\n 1)cd (syntax : cd <directory name>)\n 2)jobs (syntax:)\n 3)exit (syntax:exit)\n 4)kill (syntax:)\n");
      }
      else if (strchr(cmdline, '&'))
      {
      int siz=sizeof(arglist[1])/sizeof(char);
      char **argbg = (char **)malloc(1 * sizeof(char *)); 
         argbg[0] = (char *)malloc((siz-2) * sizeof(char));  

      for(int j=0; j < (siz-2); j++){
        argbg[0][j]=arglist[1][j];
      }      
      bg=true;
      execute(argbg,bg);
      } 
      else
      {
        execute(arglist,bg);
      }
    }
  }
  else
  {
    pipetokenize(cmdline);
  }
  free(arglist);
  free(cmdline); 
  }
//}
  //end of while loop
  printf("\n");
  return 0;
}
int execute(char* arglist[], bool bg){
   if(bg==true)
   {
        signal(SIGTTIN,handler1);
        signal(SIGTTOU,handler1);
   }
   int status;
   int cpid = fork();
   
   switch(cpid){
      case -1:
        perror("fork failed");
         exit(1);
      case 0:
        if(bg==true)
         setpgrp();
         signal(SIGINT,SIG_DFL);
         execvp(arglist[0],arglist);
         perror("command not found");
         exit(1);
        //printf("%s%s\n",arglist[0],arglist[1] );
        //execvp(arglist[0], arglist);
        //perror("Command not found...");
        //exit(1);
      default:
      if (bg == false){
          waitpid(cpid, &status, 0);
          //printf("child exited with status %d \n", status >> 8);
          dup2(a1,0);
          dup2(a2,1);
      }
      else{
        printf("Process created with PID: %d\n",cpid);
        signal(SIGCHLD,handler2);
        job[jobId]=cpid;
        jobId++;
        }
        //waitpid(cpid, &status, 0);
        //printf("child exited with status %d \n", status >> 8);
        return 0;
   }
}
char** pipetokenize(char* cmdline){
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
  int count=0;
  char* tok = strchr(cmdline, '|');
  int index=tok-cmdline;
  char* token;
  int len,ind=0,j=0;
  token=strtok(cmdline," ");
  arglist1[ind]=token;
  len=strlen(token);
  j=j+len+1;ind++;
  while(token!=NULL){
    if(j==index){
      arglist1[ind]=NULL;
    break;
  }
  token=strtok(NULL," ");
  arglist1[ind]=token;
  len=strlen(token);
  j=j+len+1;ind++;}
  ind=0;
  token=strtok(NULL," ");
  while(token!=NULL){
    token=strtok(NULL," ");
    if(token==NULL)
      break;
    arglist2[ind]=token;
    len=strlen(token);
    ind++;
  }
  arglist2[ind]=NULL;
  int fd[2];
  pipe(fd);
  pid_t cpid = fork();
  if (cpid != 0){  //parent process
    dup2(fd[1], 1); //redirect stdout to write end of pipe
    close(fd[0]); //not required so better close it
    execvp(arglist1[0],arglist1);
  }
  else{  //child process
    dup2(fd[0], 0); //redirect stdin to read end of pipe
    close(fd[1]);  //not required so better close it
    execvp(arglist2[0],arglist2);
  }
  wait(NULL);
  wait(NULL);
  arglist1[count] = NULL;
  arglist2[count] = NULL;
}      
char** tokenize(char* cmdline){
 if (strchr(cmdline,';'))
 {
    int count = countChars( cmdline, ';' );
    printf("%n", count);
    char** arglist1 = (char**)malloc(sizeof(char*)* (count+1));
    for(int j=0; j < count+1; j++){
      arglist1[j] = (char*)malloc(sizeof(char)* ARGLEN);
      bzero(arglist1[j],ARGLEN);
    }   
    char* tkn1=strtok(cmdline,";");
      int count1=0;
      while(tkn1!=NULL){
        arglist1[count1]=tkn1;
        printf("%s",tkn1);
        count++;
        tkn1=strtok(NULL,";");
      }
      for (int n=0; n<count;n++)
      {
         tokenize(arglist1[n]);
      }
 }
 else
 {
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
    else 
    {if (strstr(tkn,">"))
    {
      tkn=strtok(NULL," ");
      outfile=open(tkn,O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR,S_IWUSR,S_IXUSR);
      dup2(outfile,STDOUT_FILENO);
      close(outfile);
      tkn=strtok(NULL," ");
      continue;
    }}
    arglist[count]=tkn;
    count++;
    tkn=strtok(NULL," ");
  }

  arglist[count] = NULL;
  return arglist;
}
}      
void handler1(int n){
  close(0);
  int fp=open("/dev/tty",O_RDONLY);
  printf("Stopped %d",getpid());
  kill(getpid(),SIGSTOP);
}

void handler2(int n){
  while(waitpid(-1,NULL,WNOHANG)>0){}
  printf("\n");
}
char* read_cmd(char* prompt, FILE* fp){
  char buf[200];
  getcwd(buf, sizeof(buf));
  printf("\033[32m\033[1m%s\033[39m\033[m",prompt);
  printf("\033[34m\033[1m%s\033[39m\033[m:- ",buf);
  int c; //input character
  int pos = 0; //position of character in cmdline
  char* cmdline = (char*) malloc(sizeof(char)*MAX_LEN);
  signal(SIGINT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
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
int countChars( char* s, char c )
{
    return *s == '\0'
              ? 0
              : countChars( s + 1, c ) + (*s == c);
}
