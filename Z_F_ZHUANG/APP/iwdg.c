#include "iwdg.h"


IWDG_HandleTypeDef  IWDG_HandleStructure;

//******************************************************************************
// ����         : User_Iwdg_Init()
// ��������     : 2018-06-08
// ����         : MMX
// ����         : ���Ź�����
// �������     : ��
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : ��
// �޸�����     : �� 
//******************************************************************************
void User_Iwdg_Init(void)
{
    IWDG_HandleStructure.Init.Prescaler = IWDG_PRESCALER_8;                     //LSI��32.768��8��Ƶλ4.096K
    IWDG_HandleStructure.Init.Reload = 0x0FA0;                                  //��װ��ֵΪ4000��Լ1s
    IWDG_HandleStructure.Init.Window = 0x0FA0;
    IWDG_HandleStructure.Instance = IWDG;
    HAL_IWDG_Init(&IWDG_HandleStructure);
}


//******************************************************************************
// ����         : User_Iwdg_Feed()
// ��������     : 2018-06-08
// ����         : MMX
// ����         : ι��
// �������     : ��
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : ��
// �޸�����     : �� 
//******************************************************************************
void User_Iwdg_Feed(void)
{
    HAL_IWDG_Refresh(&IWDG_HandleStructure);
}

