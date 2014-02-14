#include "delay.h"
#include "stm32f10x_tim.h"
uint32_t volatile gTimer_1ms=0;
/*******************************************************************************
* Function Name  : NVIC_Configuration	 
* Description    : ����TIM2�ж�
*******************************************************************************/
void NVIC_TIM2Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;  

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
}
/*************************************************
����: void TIM2_IRQHandler(void)
����: TIM2�жϴ�����
˵����1ms�ж�1��
**************************************************/
void TIM2_IRQHandler(void)
{
   if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
   {      
      TIM_ClearITPendingBit(TIM2,TIM_IT_Update);	 //����жϱ�־
      gTimer_1ms++;
   }
}
/*************************************************
����: void Timer2_Configuration(void)
����: TIM2 ����
��ʱ���㣺(1 /(72 / (36 - 1 + 1))) * 2000 us = 1000us  = 1ms
**************************************************/
void Timer2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	    //��TIM2��ʱ����ʱ��
    
  TIM_DeInit(TIM2);		                                    //TIMx�Ĵ�������Ϊȱʡֵ
  
  TIM_TimeBaseStructure.TIM_Period = 2000;		            //�Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseStructure.TIM_Prescaler=36 - 1;               //TIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  TIM_ARRPreloadConfig(TIM2, ENABLE);                       //�����Զ���װ�ؼĴ�����ARR��
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);	                //����TIM2����ж�
  
  TIM_Cmd(TIM2, ENABLE);	                                //TIM2����ʱ��
}
/*************************************************
����: void Delay(u32 nTime)
����: ����TIM2ʵ�־�ȷ����ʱ
����: nTime
����: ��
**************************************************/
void DelayMS(uint32_t nTime)
{
  uint32_t count;
  
  count = gTimer_1ms;

  while((gTimer_1ms - count) < nTime);//��ʱʱ�䵽���˳�
  
}
void Delay_Init(void)
{
	NVIC_TIM2Configuration();    //�����ж� 
	Timer2_Configuration();  //��ʱ��2��ʼ��
}
