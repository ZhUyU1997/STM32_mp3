#include "music.h"
#include "explorer.h"
#include "eeprom.h"
#include "vs1003.h"
#include "tft_lcd.h"
#include "touch.h"
#include "bmpDecode.h"
#include "ff.h"
#include "lib.h"
#include<string.h>
#include<rtl.h>

extern u8 musicFile[MAX_FILENUM][MAX_FILENAME_LEN];
extern u8 musicFileNum;
extern u8 backKey;	//PC6
extern PenHolder penPoint; 

u32 currIndex=0;
char currFile[40]="/music/";	//��ǰ���ŵ��ļ�·����
u8 play=0;			 			//����״̬��1���ţ�0��ͣ
u8 freshPlayTime=0;
u8 volume=0;
OS_TID task1,task2;
OS_SEM semaphore;

//�õ�������EEPROM���������ֵ������ֵ��0,��ȡʧ�ܣ�>0�ɹ���ȡ����
u8 GetVolume(void)
{					  
	u8 ReadBuffer[4];
	u8 temp;
	I2C_ReadByte(ReadBuffer,4,20, ADDR_24LC08);
	temp=ReadBuffer[1]; 		 
	if(temp&0x01)	//��¼�ϴ�����			   
	{    				   
		return ReadBuffer[0];	 
	}
	else return 0;
}

//����������EEPROM��FM24C16�� 20~21��ε�ַ���䣬�ܹ�2���ֽ�
void SaveVolume(u8 vl)
{
	I2C_WriteByte(vl, 20, ADDR_24LC08);
	I2C_WriteByte(0xff, 21, ADDR_24LC08);							 		 
}
//���ò���ʾ��������Χ0-100
void SetVolume(u8 volume)
{	
	VS1003_SetVol(volume);

	volume*=2;
	LCD_FillRect(20,215-volume,volume,20,Blue);
	LCD_FillRect(20,15,200-volume,20,White);
}

//�����Ÿ���������������30��ʼ�ĵ�ַ��
void SaveIndex(u32 index)
{
	u8 temp,i;
	u32 andNum;
	for(i=0,andNum=0xFF;i<4;i++,andNum<<=8)
	{
		temp = index&&andNum;
		I2C_WriteByte(temp, 30+i, ADDR_24LC08);
	}
}
u32 GetIndex()
{
	u8 i,ReadBuffer[4];
	u32 temp=0;
	I2C_ReadByte(ReadBuffer,4,30, ADDR_24LC08);

	for(i=0;i<4;i++)
	{
		temp<<=8;
		temp+=ReadBuffer[i];
	}
	
	return temp;
}
/**********************��������ļ�ͷ����****************************************
���ļ�ͷ����ǰʮ���ֽڣ��ж���������ֽ��Ƿ���ID3�ı�ʶ�����û�У�֤����ǩͷ�����ڡ�
Ȼ������ǩ��С��һ���ĸ��ֽڣ���ÿ���ֽ�ֻ��7λ�����λ��ʹ�ú�Ϊ0�����Ը�ʽ���� 
0xxxxxxx 0xxxxxxx 0xxxxxxx 0xxxxxxx �����СʱҪ��0ȥ�����õ�һ��28λ�Ķ������������Ǳ�ǩ��С��
*********************************************************************************/
u32 CalHeaderLen(u8 buf[32])
{
	u32 result;
	if(buf[0]=='I'&&buf[1]=='D'&&buf[2]=='3')
	{
		result=(buf[6]&0x7F)*0x200000+(buf[7]&0x7F)*0x4000+(buf[8]&0x7F)*0x80+(buf[9]&0x7F);
		return result;
	}
	else return 0;	//�ļ�ͷ�޷�ʶ��
			
}
/***********************�������������********************************  
ֻ�ǶԹ̶������ʣ�CBR����MP3��ʱ���ļ��㷽ʽ�����ڱ������(VBR)��MP3��
����ÿ֡�ı����ʿ��ܲ�ͬ�������ϵĹ�ʽ���޷�׼ȷ�����mp3��ʱ��
	����ʱ�� = ��֡bytes �� ������ �� 8000
********************************************************************/
u16 CalBitRate(u8 buf[32])
{
	const u16 bitrate[6][15]={								 //��λKbps
	{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448},//	 MPEG-1  Layer I
	{0,32,48,56,64,80,96,112,128,160,192,224,256,320,384},	 //			 Layer II
	{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320},	 //			 Layer III
	{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448},//	 MPEG-2  Layer I
	{0,32,48,56,64,80,96,112,128,160,192,224,256,320,384},	 //			 Layer II
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160}};		 //			 Layer III
	u8 layer,mpegVersion,index;
	u8 *i=strstr(buf,"\xFF");
	if(i==0)return 0;
	else
	{
		i++;
		layer=(*i>>1)&0x03;
		mpegVersion=(*i>>3)&0x03;
		i++;
		index=*i>>4;
		switch(layer)
		{
		case 1:
			if(mpegVersion==2)return bitrate[5][index];
			else if(mpegVersion==3) return bitrate[2][index];
			break;
		case 2:
			if(mpegVersion==2)return bitrate[4][index];
			else if(mpegVersion==3) return bitrate[1][index];
			break;
		case 3:
			if(mpegVersion==2)return bitrate[3][index];
			else if(mpegVersion==3) return bitrate[0][index];
			break;
		}
		return 0;			
	} 
}
/****************************************************************
			��ʾ����ʱ��
*****************************************************************/
void ShowTime(u16 time,u16 decodeTime)
{
	u16 temp;
	u16 timev=time;
	u16 decodev=decodeTime;
	u8 timeStr[]="00:00/00:00";
	timeStr[4]=decodeTime%10+'0';
	decodeTime/=10;
	timeStr[3]=decodeTime%6+'0';
	decodeTime/=6;
	timeStr[1]=decodeTime%10+'0';
	decodeTime/=10;
	timeStr[0]=decodeTime%10+'0';

	timeStr[10]=time%10+'0';
	time/=10;
	timeStr[9]=time%6+'0';
	time/=6;
	timeStr[7]=time%10+'0';
	time/=10;
	timeStr[6]=time%10+'0';

	LCD_DisplayString(180,200,timeStr);

	if(timev>=decodev)
	{
		temp=decodev*200;
		temp/=timev;	
		LCD_FillRect(180,220,10,temp,Red);
		LCD_FillRect(180+temp,220,10,200-temp,White);
	}
}
void LoadGUI(void)
{
	/**************��ʾ����**********************/
	ShowImage("/icon/music.bmp",0,0);
	LCD_SetBackColor(ButtonColor);
	LCD_SetTextColor(Black);
	if(play==1)LCD_DisplayString(255+9,160+4,"��ͣ");
	else LCD_DisplayString(255+9,160+4,"����");
	LCD_DisplayString(180+6,160+4,"��һ��");
	LCD_DisplayString(320+6,160+4,"��һ��");
	LCD_FillRect(100,200,25,60,ButtonColor);
	LCD_SetTextColor(Black);
	LCD_DrawRect(100,200,25,60);
	LCD_DisplayString(100+14,200+5,"�б�");
	
	SetVolume(volume);

	LCD_FillRect(100,60,16,250,ButtonColor);
	LCD_DisplayString(100,60,musicFile[currIndex]);			//��ʾ������
		
}
/****************************************************************
		�������ֲ��ŵĽ���	 �����Ŀ��Լ����ź�������
*****************************************************************/
__task void MusicPlay (void)
{
	FRESULT result;
	FATFS fs;
	FIL file;
	u8 buf[32];
	u32 bw;
	u8 tempIndex;  //��ǰ���Ÿ������������л��ĸ��������ֿ�
	u32 headerLen; //�ļ�ͷ����
	u16 bitRate;   //������
	u16 time;	   //������ʱ��
	u16 decodeTime;//��ǰ����ʱ��
	
		
	while(1)							   //�������Ÿ���
	{
CS:		tempIndex=currIndex;
		FillName(currFile,musicFile[tempIndex]);

		result = f_mount(0, &fs);			/* �����ļ�ϵͳ */	
		if (result != FR_OK)return;
		result = f_open(&file, currFile, FA_OPEN_EXISTING | FA_READ); 	/* ���ļ� */
		if (result != FR_OK)return;

		LCD_FillRect(100,60,16,250,ButtonColor);
		LCD_DisplayString(100,60,musicFile[tempIndex]);			//��ʾ������

		f_read(&file, buf, sizeof(buf), &bw);  //��ȡͷ��Ϣ������ͷ���ȣ�Ϊ�˺������ʱ��
		headerLen = CalHeaderLen(buf);

		if(headerLen!=0)f_lseek(&file,headerLen+10); //��ȷ�ҵ�
		else f_lseek(&file,0);

		f_read(&file, buf, sizeof(buf), &bw);  //��ȡ֡ͷ��Ϣ�����������
		bitRate=CalBitRate(buf);
		if(bitRate!=0) time=(file.fsize-headerLen-10-128)/(bitRate/8)/1000;
		else time=0;

		ShowTime(time,0);			//��ʼ��ʾʱ��

		VS1003_Reset();
		VS1003_SoftReset();			//reset����Ҫ��������
		SetVolume(volume);

		//����һ�׸���
		while(1)	
		{
			if(play==1&&tempIndex==currIndex)
			{	
				result = f_read(&file, buf, sizeof(buf), &bw);
				if (result != FR_OK)break;
				 	
				while( MP3_DREQ ==0 ){}	 /* �ȴ����� */

				tsk_lock ();	     
				VS1003_WriteData(buf);
				tsk_unlock ();

				if(bw<31)break;
			}
			else if(tempIndex!=currIndex)
			{
				f_close(&file);		// �ر��ļ�
				f_mount(0, NULL);  /* ж���ļ�ϵͳ */
				goto CS;			//Change song �л�����
			}
			if(freshPlayTime==1&&play==1)						   //ˢ��ʱ�䣬��RTC���жϴ���
			{
				freshPlayTime = 0;
				decodeTime = VS1003_GetDecodeTime();
				ShowTime(time,decodeTime);
			}				   	
		}
		if(currIndex==musicFileNum)currIndex = 1;	  //ѭ������
		else currIndex++;
		f_close(&file);		// �ر��ļ�
		f_mount(0, NULL);  /* ж���ļ�ϵͳ */
	}
}


void MusicApp(void)
{
	u8 temp;
	play=1;			

	/**************��ʼ��VS1003*********************/
	VS1003_Reset();
	VS1003_SoftReset();	

	/**************��ʼ������*********************/
	volume=GetVolume();
	if(volume==0)volume=70;

	LoadGUI();		//��ʾ����
	

	FileCheck("/music",MP3);
	currIndex = GetIndex();
	if(currIndex<=0||currIndex>musicFileNum)currIndex=1;
		
	task1 = os_tsk_create (MusicPlay, 2);
	

	while(backKey==0)
	{
		if(penPoint.keyState==Down)
		{
			if(GetTouchPoint())
			{
				if(IsInRect(255,160,50,25))			//������ͣ
				{
					if(play==0){	play=1;	LCD_DisplayString(255+9,160+4,"��ͣ");	}
					else {	play=0;	LCD_DisplayString(255+9,160+4,"����");	}
				}
				else if(IsInRect(180,160,60,25))	//��һ��
				{
					if(currIndex>1)
					{
						currIndex--;
					}
					else  currIndex = musicFileNum;
				}
				else if(IsInRect(320,160,60,25))	//��һ��
				{
					if(currIndex<musicFileNum)
					{
						currIndex++;
					}
					else currIndex = 1;
				}
				else if(IsInRect(20,15,20,200))		//����
				{
					volume=100-(penPoint.y-20)/2;
					SetVolume(volume);
				}
				else if(IsInRect(100,200,60,25))	//�б�
				{
					tsk_lock ();
					temp=FileExplorer(musicFileNum,MP3);
					if(temp!=0)currIndex=temp;
					LoadGUI();
					tsk_unlock ();
				}
			}
		}
		os_dly_wait (20);	
	}
	backKey=0;
	os_tsk_delete (task1);    	//ɾ�����ֲ�������

	SaveIndex(currIndex);
	SaveVolume(volume);	//�˳�ʱ����������

	f_mount(0, NULL);  	// ж���ļ�ϵͳ 
}

