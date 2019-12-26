#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>  
#include <errno.h>   
#include <sys/wait.h> 
#include <sys/ioctl.h>

#define linelen 512
//#define pagelen 20

int file(FILE *,char *,int);
int get_input(FILE*,int,int);
int search_string(FILE* ,char *);


int main(int argc, char *argv[]){

	struct winsize ws;                //ye wali 3 lines page ki line count krne k lia ha
	ioctl(2,TIOCGWINSZ,&ws);			//k kitni show ho ri terminal pe
	int pagelen=ws.ws_row;
	//printf("length::::::::::::::::::::::::::%d/n",pagelen);


	system("stty cbreak -echo <&2");			//ye non canonical me jane k lia
	FILE *fp;
	if(argc==1){
		file(stdin,"NULL",pagelen);
	}
	else{
		int i=0;
		int file_count;
		while(++i < argc){
			fp=fopen(argv[i],"r");
			if(fp==NULL){
				perror("can't open file");
				exit(0);
			}
			file(fp,argv[i],pagelen);
			fclose(fp);
		}	
	}

		system("stty -cbreak echo <&2");			//ye non canonical se exit hone k lia

}
	int file(FILE *fp,char *file_name,int pagelen){
 		
 		int ch=0;				// ye wala hissa total lines count kr le ga program ki aik bar phr isse percentage nikle gi
  		int lines=0;
  		if (fp == NULL){
  			return 0;
  		}
  		while(!feof(fp))
		{
		  ch = fgetc(fp);
		  if(ch == '\n')
		  {
		    lines++;
		  }
		}


		fseek(fp, 0, SEEK_SET);

		int line_count = 0;
		int t_lcount=0;
		int inp;
		char arr[linelen];
		while (fgets(arr,linelen,fp)){
			FILE* fp_tty=fopen("/dev/tty","r");
			fputs(arr,stdout);
			line_count++;
			t_lcount++;
			if(line_count >= pagelen){
				inp=get_input(fp_tty,lines,t_lcount);
				if(inp == 0 ){
					printf("\033[1A \033[2K \033[1G");
					//return EXIT_SUCCESS;
					break;
				}
				else if(inp == 1){
					printf("\033[1A \033[2K \033[1G");
					line_count=line_count-1;	//one line
				}
				else if(inp == 2){
					printf("\033[1A \033[2K \033[1G");
					line_count=0; //space
				}

				else if(inp==3){						//ye hissa search krne k lia aik bar non canonical lgta to wo pure program
					printf("\033[1A \033[2K \033[1G");	//k lia rehta is lia pehle yha us ko khtm kia phr input lia
					printf("/");						
					system("stty -cbreak echo <&2");	// jo aik assignment thi jisme me string match krni thi wo wala hi function utha k copy paste kia ha yha
					char str[100];
					fgets(str,500,fp_tty);
					int countt;
					fseek(fp, 0, SEEK_SET);
					countt=search_string(fp,str);		//search ki working smjhne k lia more se search kr k dekho phr smjh ae ga k kese kia me ne
					//int t_lcount3=t_lcount;
					if(countt!=-1){
						int i=0;
						int skip_count=countt-2;
						printf(" Skipping.... \n");
						//printf("count:::%d\n",countt );
						//printf("total count:::%d\n",t_lcount);
						if(countt>=t_lcount){
							//printf("still coming\n");
							line_count=0;
							fseek(fp, 0, SEEK_SET);
							char arr[200];
							t_lcount=0;
							int t_lcount2=0;
							while(i!=skip_count){
								fgets(arr,512,fp);
								i++;
								t_lcount2++;
							}
							t_lcount=t_lcount2;
							char ar2[200];
							int j=0;
							//printf("page length is :::::::::::::::; %d\n", pagelen);
							//for(int j=0;j<pagelen;j++){
							while(j<pagelen){
								fgets(ar2,512,fp);
								fputs(ar2,stdout);
								j++;
								t_lcount++;
							}
						}
						else{
							//t_lcount=t_lcount3;
							int k=0;
							printf("%d\n", t_lcount);
							fseek(fp, 0, SEEK_SET);
							while(k!=t_lcount){
								fgets(arr,512,fp);
								k++;
								//t_lcount2++;
							}
							printf("nothing\n");
						}
				}
					else{
						printf("Pattern not found \n");
							int k=0;
							printf("%d\n", t_lcount);
							fseek(fp, 0, SEEK_SET);
							while(k!=t_lcount){
								fgets(arr,512,fp);
								k++;
								//t_lcount2++;
							}
					}
						system("stty cbreak -echo <&2");
				}
				else if(inp == 4){
					printf("\033[1A \033[2K \033[1G");                      //ye 2 tareeko se ho skta kam aik direct system call
//					    char buf[20];                                       //aur dusra fork aur child kr k me ne usse kr k vim ka process chlaya                  					                                        
//					    snprintf(buf, sizeof(buf), "vim %s",file_name);                                   
//					    system(buf); 
					pid_t  f;
					int status;
					f=vfork(); 
					if(f==-1){
						printf("error\n");
					}
					if(f==0){
						execlp("vim","vim",file_name, NULL);
						exit(0);
					}
					else{
						wait(NULL);
					}
			}
				
				else if(inp == 5){
					printf("\033[1A \033[2K \033[1G");
					break;
//				}
			}
		}
		//printf("%d\n",line_count );
	//	return EXIT_SUCCESS;
	}
}
	int search_string(FILE *fp,char *find)
	{
		int count=0;
		char arr[500];
		int flag=-1;
		while(!feof(fp))
		{
			fgets(arr,1000,fp);
			if(strstr(arr,find)){
				flag=0;
				break;
			}
			count++;
		}
		if(flag==0)
			return count;		
		else
			return -1;
	}

	int get_input(FILE *cmdstream,int count,int lcount){
		float per;
		per = (float)lcount / (float)count * 100.0;
		printf("\033[7m --more--(%.2f%%) \033[m",per);
		//printf("\033[1B");
		//printf("here::::::::::::::::\n");
		char u_input; //=getc(stream);
		printf("\n ");
		//read (STDIN_FILENO, &u_input, 1);
		u_input=getc(cmdstream);
		if(u_input=='q')
			return 0;
		if(u_input=='\n')
			return 1;
		if(u_input==' ')
			return 2;
		if(u_input=='/')
			return 3;
		if(u_input=='v' || 'V')
			return 4;
		else 
			return 5;
	}