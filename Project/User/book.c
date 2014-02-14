#include "stm32f10x.h"
#include "tft_lcd.h"
#include "touch.h"
#include "book.h"
#include "ff.h"
#include "lib.h"
#include "explorer.h"

#define MAX_PAGE_NUM 100

u8 text[602]={0};
extern u8 backKey;	//PC6
extern PenHolder penPoint;
extern u8 bookFile[MAX_FILENUM][MAX_FILENAME_LEN];
extern u8 bookFileNum;

static u32 pageIndex[MAX_PAGE_NUM]; 

/***************��ʾһҳ�����ظ�ҳ��ʾ���ֽ���***********************/
u16 ShowPage(char *str)
{
	u16 i;
	u8 j,lineNum=0;
	u8 linetext[51]={0};

	if(str==NULL)return 0;

	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White);

	for(i=0,lineNum=0;lineNum<12&&str[i]!=0;lineNum++)
	{
		j=0;
		while(j<50&&str[i]!=0)
		{
			if(str[i]=='\r'&&str[i+1]=='\n')	   //����
			{
				i+=2;
				break;
			}		
			if(str[i]>=0x80)linetext[j++]=str[i++];	//���ı��룬�����ֽ�
		    linetext[j++]=str[i++];
		}
		linetext[j]=0;
		LCD_DisplayString(0,lineNum*20,linetext);
	}
	
	return i;			  //���ظ�ҳ��ʾ���ֽ���
}

/********************���������**************************************/
void BookApp(void)
{
	FRESULT result;
	FATFS fs;
	FIL file; 
	uint32_t bw;

	u8 index;
	u8 fresh=1;		//�Ƿ�ˢ��ҳ��
	u8 end=0;		//������־
	u16 words;		//��ǰҳ���ֽ���
	u8 pages=0;		//��¼��ǰ��ʾ��ҳ�����������·�ҳ
	char fileName[MAX_FILENAME_LEN]="/book/";

	FileCheck("/book",TXT);			//�����ļ�

	index = FileExplorer(bookFileNum,TXT); 	//��ʾ�ļ����
	if(index==0)return;						//���ؼ�
	
	strcat(fileName,bookFile[index]);
	
	result = f_mount(0, &fs);	/* �����ļ�ϵͳ */
	if (result != FR_OK)return ;//FALSE;

	/* ���ļ� */
	result = f_open(&file, fileName, FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)return ;//FALSE;
	backKey=0;

	/* ��ȡ�ļ� */
	while(backKey==0)
	{
		if(fresh==1)
		{
			pageIndex[pages]=f_tell(&file);		 //��¼ҳָ��

			result = f_read(&file, text, sizeof(text), &bw);
			if(bw<sizeof(text))end=1;
			if (result !=  FR_OK)return ;//FALSE;
			text[bw]=0;					 //���ַ���������־
			words=ShowPage(text);
			f_lseek(&file,f_tell(&file)-bw+words);	//�����ļ���ȡָ��
			fresh=0;
		}
		if(penPoint.keyState==Down)
		{
			if(GetTouchPoint())
			{
				if(penPoint.y<120)
				{ 
					if(pages>0)
					{	
						f_lseek(&file,pageIndex[--pages]);	//�����ļ���ȡָ��
						fresh=1;
						end=0;
					}
				}
				else 
				{	
					if(end==0)
					{
						pages++;
						fresh=1;
					}
				}
			}
		}
	}

    backKey=0;

	f_close(&file);	   /* �ر��ļ�*/
	f_mount(0, NULL);  /* ж���ļ�ϵͳ */
}

