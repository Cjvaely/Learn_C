#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<time.h>
#include<pwd.h>

int main(int argc,char* argv[])
{
	DIR* dp;								//存储地址指针
	struct dirent* currentdp;
	struct stat currentstat;
	
	if(argc != 4)
	{
		printf("Do not hava enough options!\n");
		exit(1);
	}
	if((dp = opendir(argv[1])) == NULL)		//判断文件能否打开,argv[1]存储第一条命令行，opendir返回类型就是DIR* 
	{
		printf("Open directory fail！\n");
		exit(1);	
	}
	while((currentdp = readdir(dp)) != NULL)
	{
		if( currentdp->d_name[0] != '.' )
		{
			if(lstat(currentdp->d_name, &currentstat) == -1)
			{
				printf("Get stat error!\n");
				continue;
			}
			if(S_ISDIR(currentstat.st_mode))	//判断当前所读为目录,并且开始调用pid_t fork(void)函数创建一个子进程;
			{
				printf("This is a directory:\n");
				char path[1024];
				pid_t pid;
				
				snprintf(path,sizeof(path),"%s/%s",argv[1],currentdp->d_name);		//补全待复制的目录路径
				pid = fork();		//创建子进程
				if(pid == -1)
				{
					printf("fork() failed!\n");
					continue;
				}
				if(pid == 0)
				{
					int test;
					printf("In child process:%ld\n",(long)getpid());
					test = execl("/home/cjv/文档/CFile/Lab3",argv[2],path,argv[3],NULL);
					if(test == -1)
					{
						printf("Execl has error!\n");
					}
				}
				else if(pid > 0)
				{
					printf("in parent process: Child_pid = %ld\n",(long)getpid());
					int status=0;
        			wait(&status);
        			if(WIFEXITED(status))
        			{
            			printf("子进程返回信息码:%d\n",WEXITSTATUS(status));
        			}
        			else if(WIFSIGNALED(status))
        			{
            			printf("子进程信号中断返回信息码:%d\n",WTERMSIG(status));
        			}
        			else if(WIFSTOPPED(status))
        			{
            			printf("子进程暂停返回信息码:%d\n",WSTOPSIG(status));
       		 		}
       		 	else
        			{
            			printf("其他退出信息！\n");
        			}
				}
			}	
		}
	}
	closedir(dp);
	return 0;
}