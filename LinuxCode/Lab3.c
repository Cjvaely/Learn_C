#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

//复制文件
void copy_file(char* source_path,char* destination_path);
//复制文件夹
void copy_folder(char* source_path,char* destination_path);
//判断是否是目录
int is_dir(char* path);
//补充完整文件名
int add_filename(char *source, char *destination);

 int main(int argc,char* argv[])
 {
	DIR* dp;								//存储地址指针
	//struct dirent* currentdp;
	//struct stat currentstat;
	
	if(argc != 3)
		exit(1);
	if((dp = opendir(argv[1])) == NULL)		//判断文件能否打开,argv[1]存储第一条命令行，opendir返回类型就是DIR* 
	{
		printf("Open directory fail！\n");
		exit(1);	
	}
	add_filename(argv[1], argv[2]);		//补充完整文件名
	if(is_dir(argv[1]))		//判断是否是目录
	{
		copy_folder(argv[1],argv[2]);
	}
	else
	{
		copy_file(argv[1],argv[2]);
	}		
	closedir(dp);
	return 0;
 }

//复制文件
void copy_file(char* source_path,char* destination_path)
{
    char buffer[1024]; 
    FILE *in;		//定义两个文件流，分别用于文件的读取和写入
    FILE *out;
    size_t len;	//len为fread读到的字节长

    if((in = fopen(source_path,"r")) == NULL)	//打开源文件的文件流
    {			
		printf("源文件打开失败！\n");
		exit(1);
    } 
    if((out = fopen(destination_path,"w")) == NULL)		//打开目标文件的文件流
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
void copy_folder(char* source_path,char* destination_path)
{
	struct dirent* filename;
	//用DIR指针指向这个源文件夹和目标文件夹
    DIR* dp = opendir(source_path);
	DIR* des_dir =  opendir(destination_path);

	if(!des_dir)	//如果不存在就用mkdir函数来创建
	{  
		if (mkdir(destination_path,0777))
        {  
            printf("创建文件夹失败！");  
        }  
    }
  	while((filename = readdir(dp)) != NULL)		//遍历DIR指针指向的文件夹，也就是文件数组
	{ 
		char file_source_path[1024] = {0};  
		snprintf(file_source_path,sizeof(file_source_path),"%s/%s",source_path,filename->d_name);
        char file_destination_path[1024] = {0};  
        snprintf(file_destination_path,sizeof(file_destination_path),"%s/%s",destination_path,filename->d_name);
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

















