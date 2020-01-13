/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 信号量同步
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 F103-MINI STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 

/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"


/*
******************************************************************
*                               变量
******************************************************************
*/
/* 定义线程控制块 */
static rt_thread_t receive_thread = RT_NULL;
static rt_thread_t send_thread = RT_NULL;
/* 定义信号量控制块 */
static rt_sem_t test_sem = RT_NULL;

/************************* 全局变量声明 ****************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
uint8_t ucValue [ 2 ] = { 0x00, 0x00 };
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void receive_thread_entry(void* parameter);
static void send_thread_entry(void* parameter);

/*
*************************************************************************
*                             main 函数
*************************************************************************
*/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
    /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	rt_kprintf("这是一个[野火]-STM32F103-MINI-RTT二值信号量同步实验！\n");
  rt_kprintf("同步成功则输出Successful,反之输出Fail\n");
   /* 创建一个信号量 */
	test_sem = rt_sem_create("test_sem",/* 信号量名字 */
                     1,     /* 信号量初始值，默认有一个信号量 */
                     RT_IPC_FLAG_FIFO); /* 信号量模式 FIFO(0x00)*/
  if (test_sem != RT_NULL)
    rt_kprintf("信号量创建成功！\n\n");
    
	receive_thread =                          /* 线程控制块指针 */
    rt_thread_create( "receive",            /* 线程名字 */
                      receive_thread_entry, /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (receive_thread != RT_NULL)
        rt_thread_startup(receive_thread);
    else
        return -1;
    
  send_thread =                          /* 线程控制块指针 */
    rt_thread_create( "send",              /* 线程名字 */
                      send_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      2,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (send_thread != RT_NULL)
        rt_thread_startup(send_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void receive_thread_entry(void* parameter)
{	
  /* 任务都是一个无限循环，不能返回 */
  while(1)
	{
		rt_sem_take(test_sem,	/* 获取信号量 */
                RT_WAITING_FOREVER); 	/* 等待时间：一直等 */
		if ( ucValue [ 0 ] == ucValue [ 1 ] )
		{ 			
			rt_kprintf ( "Successful\n" );			
		}
		else
		{
			rt_kprintf ( "Fail\n" );			
		}
		rt_sem_release(	test_sem	);   //释放二值信号量 
		
		rt_thread_delay ( 1000 );  					      //每1s读一次		
  }
  
  
}

static void send_thread_entry(void* parameter)
{	
    /* 任务都是一个无限循环，不能返回 */
    while (1)
    {
			rt_sem_take(test_sem,	/* 获取信号量 */
                  RT_WAITING_FOREVER); /* 等待时间：一直等 */		
		ucValue [ 0 ] ++;		
		rt_thread_delay ( 100 );        	 /* 延时100ms */		
		ucValue [ 1 ] ++;		
		rt_sem_release(	test_sem	);	//释放二值信号量
		rt_thread_yield();  			//放弃剩余时间片，进行一次任务切换	
    }
}




/********************************END OF FILE****************************/
