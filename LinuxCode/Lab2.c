#include<stdio.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<time.h>
#include<pwd.h>

	//打印文件属性
void print_type(mode_t stmode); 
	 //打印权限属性
void print_perm(mode_t stmode);
	//打印文件的硬链接数
void print_link(long stnlink);
	//打印所有者
void print_usrname(uid_t stuid);
	//打印所在组名
void print_grname(uid_t stgid);
	//打印字节大小
void print_size(off_t stsize); 
	//打印最后修改时间
void print_time(time_t stmtime);
	//打印文件名
void print_filename(struct dirent* current_dp);

int main(int argc,char* argv[])
{
	DIR* dp;								//存储地址指针
	struct dirent* currentdp;
	struct stat currentstat;
	
	if(argc != 2)
		exit(1);
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
			print_type(currentstat.st_mode);	//文件属性
			print_perm(currentstat.st_mode);	//权限属性 
			print_link(currentstat.st_nlink);	//文件硬链接数
			print_usrname(currentstat.st_uid);;	//所有者
			print_grname(currentstat.st_gid);	//所在组名 
			print_size(currentstat.st_size);	//字节大小 
			print_time(currentstat.st_mtime);	//修改时间 
			print_filename(currentdp);			//文件名
		}
	}
	closedir(dp);
	return 0;
}
	//打印文件属性
void print_type(mode_t stmode)
{
	if(S_ISREG(stmode))		//是否为一般文件
		printf("-");
	else if(S_ISDIR(stmode))	//是否为目录 
		printf("d");
	else if(S_ISCHR(stmode))	//是否为字符装置文件 
		printf("c");
	else if(S_ISBLK(stmode))	//是否是一个块设备
		printf("b");
	else if(S_ISFIFO(stmode))	//是否是FIFO文件
		printf("p");
	else if(S_ISLNK(stmode))	//是否是一个链接
		printf("l");
	else if(S_ISSOCK(stmode))	//是否是socket文件 
		printf("s");  
}
	 //打印权限属性	权限有3类，可读取 r，可写入w，可执行x
void print_perm(mode_t stmode)
{
/*文件所有者权限*/
	if(stmode&S_IRUSR)
	{
        printf("r_user");
	}
	else
	{
		printf("-"); 
	}
	if(stmode&S_IWUSR)
	{
        printf("w_user");
	}
	else
	{
		printf("-");
	}
	if(stmode&S_IXUSR)
	{
        printf("x_user");
	}
	else
	{
		printf("-");
	}
/*
	
//用户组权限
	if(stmode&S_IRGRP)
	{
        printf("r_GRP");
	}
	else
	{
		printf("-");
	}
	if(stmode&S_IWGRP)
	{
        printf("w_GRP");
	}
	else
	{
		printf("-");
	}
	if(stmode&S_IXGRP)
	{
        printf("x_GRP");
	}
	else
	{
		printf("-");
	}
//其他用户权限
	if(stmode&S_IROTH)
	{
        printf("r_OTH");
	}
	else
	{
		printf("-");
	}
	if(stmode&S_IWOTH)
	{
        printf("w_OTH");
	}
	else
	{
		printf("-");
	}
	if(stmode&S_IXOTH)
	{
        printf("x_OTH");
	}
	else
	{
		printf("-");
	}*/
}

	//打印文件的硬链接数
void print_link(long stnlink)
{
	printf("\nThe Hard_link_Num: %ld",stnlink);
} 
	//打印所有者
void print_usrname(uid_t stuid)		//short
{
	struct passwd *pd;
	pd = getpwuid(stuid);
	printf("The User_name: %s\n",pd->pw_name);
}
	//打印所在组名
void print_grname(uid_t stuid)		//short
{
	struct passwd *pd;
	pd = getpwuid(stuid);
	printf("The gid: %d\n",pd->pw_gid);
}
	//打印字节大小
void print_size(off_t stsize)
{
	printf("The bytes: %ld\n",stsize);
}
	//打印最后修改时间
void print_time(time_t stmtime)			//time_t long类型表示的是某个节点到某时刻的秒数
{
	struct tm *local;
	 
	//stmtime = time(NULL);				//获得日历时间		localtime是本地时间 	
	local = localtime(&stmtime);		//localtime函数返回类型为结构体struct tm
	printf("The time is: %d-%d-%d",(1900+local->tm_year),(1+local->tm_mon),local->tm_mday);
	printf(" %d:%d\n",(1+local->tm_hour),(local->tm_sec));
	printf("**********************************");
	//printf("\n%s\n",ctime(&stmtime));
}
//struct tm{ 
//	int tm_sec;			/* 秒 – 取值区间为[0,59] */ 
//	int tm_min;			/* 分 - 取值区间为[0,59] */ 
//	int tm_hour;		/* 时 - 取值区间为[0,23] */ 
//	int tm_mday;		/* 一个月中的日期 - 取值区间为[1,31] */ 
//	int tm_mon;			/* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */ 
//	int tm_year;		/* 年份，其值等于实际年份减去1900 */
//	int tm_wday;
//}
// 
	//打印文件名
void print_filename(struct dirent* current_dp)
{
	printf("The name of the file:%s\n",current_dp->d_name);
}

