#ifndef _TASK_H
#define _TASK_H


#include "rtthread.h"

typedef struct 
{
	//��̬�����߳�ʱʹ�õ��̲߳����ṹ��
	char *name;
    void (*entry)(void *parameter);
    void       *parameter;
    rt_uint32_t stack_size;
	rt_uint8_t  priority;
	rt_uint32_t tick;
}TaskStruct;

void task_Init(void);

#endif


