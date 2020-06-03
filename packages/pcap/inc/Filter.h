#ifndef __FILTER_H__
#define __FILTER_H__
typedef enum
{ 
  Bit_RESET = 0,
  Bit_SET
}BitAction;

void App_Filter_Task (void *p_arg);

#endif
