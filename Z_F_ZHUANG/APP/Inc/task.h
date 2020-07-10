#ifndef _TASK_H
#define _TASK_H


#include "rtthread.h"

typedef struct 
{
	//动态创建线程时使用的线程参数结构体
	char *name;
    void (*entry)(void *parameter);
    void       *parameter;
    rt_uint32_t stack_size;
	rt_uint8_t  priority;
	rt_uint32_t tick;
}TaskStruct;

void task_Init(void);

#endif


