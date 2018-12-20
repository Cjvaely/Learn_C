#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>
#include<pwd.h>

char destination_path[1024];
//复制文件
void copy_file(char* source_path,char* destination_path);
//复制文件夹
void copy_folder(char* source_path,char* destination_path);
//判断是否是目录
int is_dir(char* path);
//补充完整文件名
int add_filename(char *source, char *destination);
//线程开始函数
void *cp_r(char* source_path);
int main(int argc,char* argv[])
{
	DIR* dp;								//存储地址指针
	struct dirent* currentdp;
	struct stat currentstat;
	if(argc != 3)
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
			if(S_ISDIR(currentstat.st_mode))
			{
				printf("This is a directory:\n");
				strcpy(destination_path,argv[2]);
				pthread_t thread;		//This is threadID
				int thd;				//This is pthread_return number
				char *path;
				int test;
				
				path = (char*)malloc(sizeof(char)*100);		
				test = snprintf(path,100,"%s/%s",argv[1],currentdp->d_name);
				printf("snprintf = %d\n",test);
				printf("The source_path is %s\n",path);
				memset(&thread,0,sizeof(thread));
				thd = pthread_create(&thread,NULL,(void *)cp_r,path);
				if(thd != 0)
				{
					printf("创建线程失败！\n");
				}
				else
				{
					printf("创建线程成功！\n");
				}
				pthread_join(thread,NULL);
				printf("Thread exit!\n");
			}	
		}
	}
	closedir(dp);
	return 0;
}

//复制文件
void copy_file(char* sourcepath,char* destinationpath)
{
    char buffer[1024];
    FILE *in;		//定义两个文件流，分别用于文件的读取和写入
    FILE *out;
    size_t len;		//len为fread读到的字节长

    if((in = fopen(sourcepath,"r")) == NULL)	//打开源文件的文件流
    {			
		printf("源文件打开失败！\n");
		exit(1);
    } 
    if((out = fopen(destinationpath,"w")) == NULL)		//打开目标文件的文件流
    {
        printf("目标文件创建失败！\n");  
        exit(1);  
    }
    while((len = fread(buffer,1,1024,in)) > 0)	//从源文件中读取数据并放到缓冲区中，第二个参数1也可以写成 sizeof(char)
	{
		fwrite(buffer,1,len,out);	//将缓冲区的数据写到目标文件中  
    }
    fclose(out);
    fclose(in);
}

//复制文件夹|目录
void copy_folder(char* sourcepath,char* destinationpath)
{
	printf("in copy_folder the destination_path:%s\n",destinationpath);
	struct dirent* filename;
	//用DIR指针指向这个源文件夹和目标文件夹
    DIR* dp = opendir(sourcepath);
	DIR* des_dir =  opendir(destinationpath);

	if(!des_dir)	//如果不存在就用mkdir函数来创建
	{  
		if (mkdir(destinationpath,0777))
        {  
            printf("创建文件夹失败！\n");  
        }  
    }
  	while((filename = readdir(dp)) != NULL)		//遍历DIR指针指向的文件夹，也就是文件数组
	{ 
		char file_source_path[1024] = {0};  
		snprintf(file_source_path,sizeof(file_source_path),"%s/%s",sourcepath,filename->d_name);
        char file_destination_path[1024] = {0};  
        snprintf(file_destination_path,sizeof(file_destination_path),"%s/%s",destinationpath,filename->d_name);
        if(is_dir(file_source_path))	//如果是目录
		{  
            if((strcmp(filename->d_name,".") != 0 )&& (strcmp(filename->d_name,"..") != 0 ))
			{	//同时并不以.结尾，因为Linux在所有文件夹都有一个.文件夹用于连接上一级目录，必须剔除，否则进行递归的话，后果无法想象
                copy_folder(file_source_path,file_destination_path);	//进行递归调用，相当于进入这个文件夹进行复制
            }
        }  
        else
		{  
            copy_file(file_source_path,file_destination_path);		//否则按照单一文件的复制方法进行复制 
            printf("复制%s到%s成功! \n",file_source_path,file_destination_path);  
        }  
    }
    if (!des_dir)
	{
		closedir(des_dir);
    }
    if (!dp)
	{
		closedir(dp); 
	}
}

//判断是否是目录
int is_dir(char* path)
{
	struct stat st;  
    stat(path,&st);
      
    if(S_ISDIR(st.st_mode))
    {  
        return 1;  
    }  
    else
    {  
        return 0;  
    }  
}

//补充完整文件名
int add_filename(char *source, char *destination)
{
	int i, n;
	for(i = 0; source[i] != '\0'; i++)
	{
		if(source[i] == '/')
		{
			n = i;
		}
	}
	int m;
	for(m = 0; destination[m] != '\0'; m++);
	for( ; source[n] != '\0'; n++, m++)
	{
		destination[m] = source[n];
	}
	destination[m] = '\0';
	return 0;
}
//线程开始函数
void* cp_r(char* source_path)		//线程中输入补全的路径path
{
	DIR* dp;
	
	if((dp = opendir(source_path)) == NULL)		//判断文件能否打开,opendir返回类型就是DIR* 
	{
		printf("Open directory fail！\n");
		exit(1);
	}
	add_filename(source_path,destination_path);		//补充完整文件名
	if(is_dir(source_path))		//判断是否是目录
	{
		copy_folder(source_path,destination_path);
	}
	else
	{
		copy_file(source_path,destination_path);
	}
	closedir(dp);
}
