#include "stm32f10x.h"
#include "delay.h"
#include "backLight.h"
#include "tft_lcd.h"
#include "calendar.h"
#include "eeprom.h"
#include "touch.h"
#include "key.h"
#include "lib.h"
#include "bmpDecode.h"
#include "sdio.h"
#include "vs1003.h"
#include "music.h"
#include "photo.h"
#include "book.h"
#include "setting.h"

#include<rtl.h>

extern PenHolder penPoint;
Menu_BUTTON mb=DEFAULT;

/*******************************************************************************
* Function Name  : EXTIConfig_Init
* Description    : ��ʼ���ж���������ж����ȼ�����
*******************************************************************************/
void EXTIConfig_Init(void)
{
#ifdef  VECT_TAB_RAM                            //���C/C++ Compiler\Preprocessor\Defined symbols�еĶ�����VECT_TAB_RAM 
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);   //����RAM����
#else                                           //���û�ж���VECT_TAB_RAM, ����VECT_TAB_FLASH
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //����Flash�����
#endif 
	//�����ж����ȼ����飬����1λ������ռʽ���ȼ���3λ������Ӧ���ȼ���������ռ���ȼ���2������Ӧ���ȼ���8��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}
/*******************************************************************************
* Function Name  : GetMainIndex
* Description    : ��ȡ����
*******************************************************************************/
Menu_BUTTON GetMainIndex(void)
{
	if(!GetTouchPoint())return DEFAULT;
	if(penPoint.y>35)
	{
		if(penPoint.y<140)
		{
			if(penPoint.x<130)return MUSIC;
			else if(penPoint.x<270)	return PHOTO;
			else return BOOK;
		}
		else
		{
			if(penPoint.x<130) return SETTING;
			else if(penPoint.x<270)	return GAME;
			else  return TIME;
		} 
	}
	else return DEFAULT;
}
/*******************************************************************************
* Function Name  : ShowMainMenu
* Description    : ��ʾ���˵�
*******************************************************************************/
void ShowMainMenu(void)
{
	ShowImage("/icon/main.bmp",0,0);
	LCD_SetBackColor(BackColor);
	LCD_SetTextColor(Black);
	LCD_DisplayString(45,120,"����");
	LCD_DisplayString(175,120,"ͼ��");
	LCD_DisplayString(300,120,"������");
	LCD_DisplayString(45,210,"����");
	LCD_DisplayString(175,210,"��Ϸ");
	LCD_DisplayString(300,210,"ʱ��");
	
	mb=MAIN;		//��ʾʱ��
}

__task void MainApp (void) 
{
	while(1)
	{
		ShowMainMenu();
		while(penPoint.keyState==Up);
		if(penPoint.keyState==Down)
		{
			mb=GetMainIndex();
			switch (mb)
			{
			case MUSIC :
				MusicApp();
				break; 
			case PHOTO :
				PhotoApp();
				break; 
			case BOOK :
				BookApp();
				break; 
			case SETTING :
				SettingApp();
				break;
			case GAME :
			
				break; 
			case TIME :
				TimeApp();
				break; 
			}
		}
	}
}
/*******������********/
int main (void) 
{
	SystemInit();		 //��ʼ��ʱ��
	EXTIConfig_Init();	 //�����ж����ȼ����鼰�ж�����
	Delay_Init();		 //��ʼ����ʱ��ʹ��TIM2����ʱ
	Eeprom_Init(); 		 //Eeprom��ʼ����EEPROM��¼������У׼���ݣ������ͱ���
	LCD_Init();			 //LCD��ʾ����ʼ��
	BackLight_Init();	 //LCD�����ʼ��
	LCD_Clear(White);	 //
	VS1003_Init();		 //��ʼ��VS1003
	Touch_Init();		 //��ʼ������������У׼
	Key_Init();			 //��ʼ������
	NVIC_SDIOConfiguration(); //��ʼ��SD�ж�
	Calendar_Init();	 //��ʼ�� ����


	os_sys_init (MainApp);   /* Initialize RTX and start init    */                 
}
