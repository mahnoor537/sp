#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
extern int errno;
char permission(struct stat f)
{
        char f_type[0];
        if ((f.st_mode &  0170000) == 0010000)
                f_type[0]='p';
        else if ((f.st_mode &  0170000) == 0020000)
                f_type[0]='c';
        else if ((f.st_mode &  0170000) == 0040000)
                f_type[0]='d';
        else if ((f.st_mode &  0170000) == 0060000)
                f_type[0]='b';
        else if ((f.st_mode &  0170000) == 0100000)
                f_type[0]='-';
        else if ((f.st_mode &  0170000) == 0120000)
                f_type[0]='|';
        else if ((f.st_mode &  0170000) == 0140000)
                f_type[0]='s';
        printf("%c",f_type[0]);
return f_type[0];
}
void do_find(char* dir,char* file,char option)
{
        DIR* dp=opendir(dir);
        errno=0;
        struct dirent* entry;
        struct stat get_total;
        if (dp==NULL)
        {
                printf("Can't open direcory:%s\n",dir);
	}
	if (option=='t')
	{
		printf("%s\n",dir);
	}
	chdir(dir);
        while ((entry=readdir(dp))!=NULL)
        {
                if (entry ==NULL && errno!=0)
                {
                        perror("readdir failed");
                        exit(1);
                }
                if(option=='n')
                {
			if (strcmp(entry->d_name,file)==0)
			{
				printf("/%s/%s\n",dir,file);
				return;
			}
                }
                else if (option=='t')
		{
			int var=lstat(entry->d_name,&get_total);
			if (var==-1)
			{	
				perror("failed");
				exit(0);
			}
	
			if((strcmp("p",file)==0) && (permission(get_total)=='p'))
			{ 
				printf("/%s/%s\n",dir,file);
                                return;
			}
			if((strcmp("c",file)==0) && (permission(get_total)=='c'))
                        {
                                printf("/%s/%s\n",dir,file);
                                return;
                        }
			if((strcmp("d",file)==0) && (permission(get_total)=='d'))
                        {
                                printf("/%s/%s\n",dir,file);
                                return;
                        }
			if((strcmp("b",file)==0) && (permission(get_total)=='b'))
                        {
                                printf("/%s/%s\n",dir,file);
                                return;
                        }
			if((strcmp("-",file)==0) && (permission(get_total)=='-'))
                        {
                                printf("/%s/%s\n",dir,file);
                                return;
                        }
			if((strcmp("|",file)==0) && (permission(get_total)=='|'))
                        {
                                printf("/%s/%s\n",dir,file);
                                return;
                        }
			if((strcmp("s",file)==0) && (permission(get_total)=='s'))
                        {
                                printf("/%s/%s\n",dir,file);
                                return;

                        }
		}
		else 
		{
			printf("/%s/%s\n",dir,file);
		}
        }
        closedir(dp);
}
int main(int argc, char* argv[])
{
	if(argc==1)
	{
		do_find(".","NULL",' ');
	}
	else if (argc==2)
	{
		do_find(argv[1],"NULL",' ');
	}
	else if (argc=4)
	{
		if(strcmp(argv[2],"-name")==0)
		{
			do_find(argv[1],argv[3],'n');
		}
		else if(strcmp(argv[2],"-type")==0)
		{
			do_find(argv[1],argv[3],'t');
		}
		else
			printf("try another option\n");
	}
	else
	{
		do_find(argv[1],"NULL",' ');
		printf("enter correct arguments\n");
	}	
return 0;
}
		
