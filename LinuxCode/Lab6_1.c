#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pwd.h>

pthread_mutex_t mut[5];		//五个互斥量
pthread_t pid[5];				//五个线程
void thread_create(int number);
//任务
void *philosopher(int number);
//拿筷子
int takechopstick(int number);
//放筷子
int putchopstick(int number);
//等待线程
void thread_wait(int number);

int main()
{
	int i;
	if((i = pthread_mutex_init(mut,NULL)) == 0)
	{
		printf("互斥量初始化成功！\n");
	}
	else
	{
		printf("互斥量初始化失败！\n");
		exit(1);
	}
	thread_create(0);
	thread_create(1);
	thread_create(2);
	thread_create(3);
	thread_create(4);
	
	thread_wait(0);
	thread_wait(1);
	thread_wait(2);
	thread_wait(3);
	thread_wait(4);
	
	return 0;
}
void thread_create(int number)
{
	int temp;
	memset(&pid, 0, sizeof(pid));
	/*创建线程*/
	temp = pthread_create(&pid[number], NULL, (void *)philosopher, number);
	if(temp != 0)
	{
		printf("线程%d创建失败!\n",number);
	}
}
void *philosopher(int number)
{  
	while(1)
	{
		int i, j;
		printf("Philosopher %d is thinking\n",number);		//哲学家开始思考
		sleep(2);		//思考一段时间
		while(1)
		{
			if((i=takechopstick(number))&&(j=takechopstick((number+1)%5)))
			{
				printf("philosopher %d is eating\n",number);		//哲学家进餐
				sleep(2);		//进餐一段时间
				if((i=putchopstick(number))&&(j=putchopstick((number+1)%5)))
				{
					printf("philosopher %d do successfully!\n",number);
					continue;
				}
			}
			else
			{
				continue;
			}
		}
     }
}
//拿筷子
int takechopstick(int number)
{
	int j;
	if((j = pthread_mutex_lock(&mut[number])) == 0)
	{
		printf("philosopher %d take one chopstick!\n",number);
	}
	else
	{
		printf("philosopher %d take one chopstick failed!\n",number);
	}
	return 1;
}
//放筷子
int putchopstick(int number)
{
	int j;
	if((j = pthread_mutex_unlock(&mut[number])) == 0)
	{
		printf("philosopher %d put the other chopstick!\n",number);
	}
	else
	{
		printf("philosopher %d put the other chopstick failed!\n",number);
	}
	return 1;
}
void thread_wait(int number)
{
	if(pid[number] != 0)
	{    
		pthread_join(pid[number],NULL);
		printf("thread %d has finished!/n",number);
	}
}
