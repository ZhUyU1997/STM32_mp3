#include "eeprom.h"

void I2C_delay(void)		  //�����ʱ���Ǿ�ȷ��
{	
   uint8_t i=50;     
   while(i) 
   { 
     i--; 
   } 
}

u8 I2C_Start(void)	 //I2C��ʼλ
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)
	  return 0;	                    //SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) 
	  return 0;	                    //SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L;							//SCLΪ�ߵ�ƽʱ��SDA���½��ر�ʾֹͣλ
	I2C_delay();
	return 1;
}

void I2C_Stop(void)			   //I2Cֹͣλ
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;					   //SCLΪ�ߵ�ƽʱ��SDA�������ر�ʾֹͣλ
	I2C_delay();
}

static void I2C_Ack(void)		//I2C��Ӧλ
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

static void I2C_NoAck(void)		//I2C����Ӧλ
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

u8 I2C_WaitAck(void) 	  //I2C�ȴ�Ӧ��λ
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
      return 0;
	}
	SCL_L;
	return 1;
}

 /*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : ���ݴӸ�λ����λ
* Input          : - SendByte: ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_SendByte(u8 SendByte) 
{
    u8 i;
    for(i = 0;i < 8; i++) 
    {
      SCL_L;
      I2C_delay();
      if(SendByte & 0x80)
        SDA_H;  			  //��SCLΪ�͵�ƽʱ������SDA���ݸı�
      else 
        SDA_L;   
      SendByte <<= 1;
      I2C_delay();
      SCL_H;
      I2C_delay();
    }
    SCL_L;
}

/*******************************************************************************
* Function Name  : I2C_ReceiveByte
* Description    : ���ݴӸ�λ����λ
* Return         : I2C���߷��ص�����
*******************************************************************************/
u8 I2C_ReceiveByte(void)  
{ 
    u8 i,ReceiveByte = 0;

    SDA_H;				
    for(i = 0;i < 8; i++) 
    {
      ReceiveByte <<= 1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)				   //��SCLΪ�ߵ�ƽʱ��SDA�ϵ����ݱ��ֲ��䣬���Զ�����
      {
        ReceiveByte |= 0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}

/*******************************************************************************
* Function Name  : I2C_WriteByte
* Description    : дһ�ֽ�����
* Input          : - SendByte: ��д������
*           	   - WriteAddress: ��д���ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�д��,=0ʧ��
*******************************************************************************/           
u8 I2C_WriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress)
{		
    if(!I2C_Start()) return 0;

    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE); //���ø���ʼ��ַ+������ַ

    if(!I2C_WaitAck())
	{
	  I2C_Stop(); 
	  return 0;
	}

    I2C_SendByte((u8)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
    DelayMS(10); 		  //ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
    return 1;
}									 

/*******************************************************************************
* Function Name  : I2C_ReadByte
* Description    : ��ȡһ������
* Input          : - pBuffer: ��Ŷ�������
*           	   - length: ����������
*                  - ReadAddress: ��������ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�����,=0ʧ��
*******************************************************************************/          
u8 I2C_ReadByte(u8* pBuffer,   u16 length,   u16 ReadAddress,  u8 DeviceAddress)
{		
    if(!I2C_Start()) return 0;

    I2C_SendByte(((ReadAddress & 0x0700) >> 7) | DeviceAddress & 0xFFFE); //���ø���ʼ��ַ+������ַ

    if(!I2C_WaitAck())
	{
	  I2C_Stop(); 
	  return 0;
	}

    I2C_SendByte((u8)(ReadAddress & 0x00FF));   //���õ���ʼ��ַ   
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(((ReadAddress & 0x0700) >>7) | DeviceAddress | 0x0001);
    I2C_WaitAck();

    while(length)
    {
      *pBuffer = I2C_ReceiveByte();
      if(length == 1)
	    I2C_NoAck();
      else 
	    I2C_Ack(); 
      pBuffer++;
      length--;
    }

    I2C_Stop();

    return 1;
}
/*************************************************
����: void Eeprom_Init(void)
����: EEPROM�ܽ�����
**************************************************/
void Eeprom_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //����GPIOBʱ��
	
	/* Configure I2C2 pins: PB6->SCL and PB7->SDA */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  		 //��©���
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
