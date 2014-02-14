#ifndef _BMPDECODE_H
#define _BMPDECODE_H
#include "stm32f10x.h"
#include "sdcard.h"

#pragma pack(2)	 //2�ֽڶ���

typedef struct 
{
	//BMPλͼ�ļ�ͷ
    u16 bfType;         //Offset:0x0000,�ļ���־.ֻ��'BM',����ʶ��BMPλͼ���� 
    u32 bfSize;	        //Offset:0x0002,�ļ���С,ռ�ĸ��ֽ�;ע���Ǵ��ֽ�����С�ֽ���,ʵ��BMP��ʽ����С�ֽ���  
    u16 bfReserved1;    //Offset:0x0006,���� 
	u16 bfReserved2;    //Offset:0x0008,���� 
    u32 bfOffBits;      //Offset:0x000A,���ļ���ʼ��λͼ����(bitmap data)��ʼ֮��ĵ�ƫ����,�������ͼ����Ϣͷ�Ķ��ٶ�,ͼ����Ϣͷ��СΪ0x28���ֶ�ֵΪ0x36,���ֶε�ֵʵ����Ϊ"ͼ����Ϣͷ����+14+��ɫ����Ϣ����" 

	//BMPλͼ��Ϣͷ 
	u32 biSize;	    	 //Offset:0x000E,λͼ��Ϣͷ�ֽ�������һ��Ϊ0x28,���ȡ��ʵ��ֵ 
    s32 biWidth;         //Offset:0x0012,˵��ͼ��Ŀ�ȣ�������Ϊ��λ  
    s32 biHeight;	     //Offset:0x0016,˵��ͼ��ĸ߶ȣ�������Ϊ��λ���������һ��������˵��ͼ�������Ǵ�ͼ�����½ǵ����Ͻ����еġ� 
    u16 biPlanes;	     //Offset:0x001A,ΪĿ���豸˵��λ��������ֵ�����Ǳ���Ϊ1  
    u16 biBitCount;	     //Offset:0x001C,˵��������/���أ���ֵΪ1��4��8��16��24����32 
    u32 biCompression;   //Offset:0x001E,˵��ͼ������ѹ�������͡���ֵ����������ֵ֮һ�� 
                                        //BI_RGB��û��ѹ���� 
                                        //BI_RLE8��ÿ������8���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����(�ظ����ؼ�������ɫ����)��   
                                        //BI_RLE4��ÿ������4���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ���� 
  	                                    //BI_BITFIELDS��ÿ�����صı�����ָ������������� 
    u32 biSizeImage;    //Offset:0x0022,˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0   
    s32 biXPelsPerMeter;//Offset:0x0026,˵��ˮƽ�ֱ��ʣ�������/�ױ�ʾ,�з������� 
    s32 biYPelsPerMeter;//Offset:0x002A,˵����ֱ�ֱ��ʣ�������/�ױ�ʾ,�з������� 
    u32 biClrUsed;	   	//Offset:0x002E,˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ������ 
    u32 biClrImportant; //Offset:0x0032,˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ��  
}BMP_FILEHEADER;

typedef struct {
	u16 B:5;
	u16 G:5;
	u16 R:5;
	u16 reserve:1;
}SRGB16;

typedef struct {
	u8 R;
	u8 G;
	u8 B;
}SRGB24;
void AutoShowImage(char *fileName);
void ShowBigImage(char *fileName);
bool ShowImage(char *fileName,u16 x,u16 y);
#endif
