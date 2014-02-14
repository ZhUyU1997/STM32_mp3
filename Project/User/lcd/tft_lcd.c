/****************************************************************************
*
* �ļ���: tft_lcd.c
* ���ݼ���: ��ģ�����TFT LCD��ʾ��������������
*
*	����Ƽ�����������TFT��ʾ��������оƬΪ SPFD5420A���ֱ���Ϊ400x240
*	����оƬ�ķ��ʵ�ַΪ
*		(1) 0x60000000
*/

#include "stm32f10x.h"
#include "delay.h"
#include "tft_lcd.h"


static __IO uint16_t s_TextColor = 0x0000;
static __IO uint16_t s_BackColor = 0xFFFF;

/* ���͸����־
	s_Transparent = 1 ��ʾ��ʾ����ʱ�����޸ı�����ɫ
*/
static __IO uint8_t s_Transparent = 0;

/*******************************************************************************
*	������: LCD_Init
*	��  ��: ��
*	��  ��: ��
*	��  ��: ��ʼ��TFT��ʾ��
*/
void LCD_Init(void)
{
	/* ����LCD���ƿ���GPIO */
	LCD_CtrlLinesConfig();

	/* ����FSMC�ӿڣ��������� */
	LCD_FSMCConfig();

	/* FSMC���ú������ӳٲ��ܷ��������豸  */
	DelayMS(20);

	/* ��ʼ��LCD��дLCD�Ĵ����������� */
	LCD_WriteReg(0x0000, 0x0000);	 //ID Read Register
	LCD_WriteReg(0x0001, 0x0100);	 //Driver Output Control Register
	LCD_WriteReg(0x0002, 0x0100);	 //LCD Driving Waveform Control
	LCD_WriteReg(0x0003, 0x1018);	 //Entry Mode	  0x1018 1030

	LCD_WriteReg(0x0007, 0x0001);	 //Display Control 1
	LCD_WriteReg(0x0008, 0x0808);	 //Display Control 2
	LCD_WriteReg(0x0009, 0x0001);	 //Display Control 3

	LCD_WriteReg(0x000B, 0x0010);	 //Low Power Control
	LCD_WriteReg(0x000C, 0x0000);	 //External Display Interface Control 1
	LCD_WriteReg(0x000F, 0x0000);	 //External Display Interface Control 2
	LCD_WriteReg(0x0010, 0x0013);	 //Panel Interface Control 1
	LCD_WriteReg(0x0011, 0x0501);	 //Panel Interface Control 2
	LCD_WriteReg(0x0012, 0x0300);	 //Panel Interface control 3
	LCD_WriteReg(0x0020, 0x021E);	 //Panel Interface control 4
	LCD_WriteReg(0x0021, 0x0202);	 //Panel Interface Control 5
	LCD_WriteReg(0x0090, 0x8000);	 //Frame Marker Control
	LCD_WriteReg(0x0100, 0x17B0);	 //Power Control 1
	LCD_WriteReg(0x0101, 0x0147);	 //Power Control 2
	LCD_WriteReg(0x0102, 0x0135);	 //Power Control 3
	LCD_WriteReg(0x0103, 0x0700);	 //Power Control 4
	LCD_WriteReg(0x0107, 0x0000);	 //Power Control 5
	LCD_WriteReg(0x0110, 0x0001);	 //Power Control 6
	LCD_WriteReg(0x0210, 0x0000);
	LCD_WriteReg(0x0211, 0x00EF);
	LCD_WriteReg(0x0212, 0x0000);
	LCD_WriteReg(0x0213, 0x018F);
	LCD_WriteReg(0x0280, 0x0000);	 //NVM read data 1
	LCD_WriteReg(0x0281, 0x0004);	 //NVM read data 2
	LCD_WriteReg(0x0282, 0x0000);	 //NVM read data 3
	LCD_WriteReg(0x0300, 0x0101);
	LCD_WriteReg(0x0301, 0x0B2C);
	LCD_WriteReg(0x0302, 0x1030);
	LCD_WriteReg(0x0303, 0x3010);
	LCD_WriteReg(0x0304, 0x2C0B);
	LCD_WriteReg(0x0305, 0x0101);
	LCD_WriteReg(0x0306, 0x0807);
	LCD_WriteReg(0x0307, 0x0708);
	LCD_WriteReg(0x0308, 0x0107);
	LCD_WriteReg(0x0309, 0x0105);
	LCD_WriteReg(0x030A, 0x0F04);
	LCD_WriteReg(0x030B, 0x0F00);
	LCD_WriteReg(0x030C, 0x000F);
	LCD_WriteReg(0x030D, 0x040F);
	LCD_WriteReg(0x030E, 0x0300);
	LCD_WriteReg(0x030F, 0x0701);
	LCD_WriteReg(0x0400, 0x3500);	 //Base Image Number of Line
	LCD_WriteReg(0x0401, 0x0001);	 //Base Image Display Control
	LCD_WriteReg(0x0404, 0x0000);	 //Based Image Vertical Scroll Control
	LCD_WriteReg(0x0500, 0x0000);	 //Display Position - Partial Display 1
	LCD_WriteReg(0x0501, 0x0000);	 //RAM Address Start �C Partial Display	1
	LCD_WriteReg(0x0502, 0x0000);	 //RAM Address End �C Partial Display 1
	LCD_WriteReg(0x0503, 0x0000);	 //Display Position �C Partial Display 2
	LCD_WriteReg(0x0504, 0x0000);	 //RAM Address Start �C Partial Display 2
	LCD_WriteReg(0x0505, 0x0000);	 //RAM Address End �C Partial Display 2
	LCD_WriteReg(0x0600, 0x0000);
	LCD_WriteReg(0x0606, 0x0000);	 //Pin Control
	LCD_WriteReg(0x06F0, 0x0000);	 //NVM Access Control
	LCD_WriteReg(0x07F0, 0x5420);	 //
	LCD_WriteReg(0x07DE, 0x0000);	 //
	LCD_WriteReg(0x07F2, 0x00DF);
	LCD_WriteReg(0x07F3, 0x0810);
	LCD_WriteReg(0x07F4, 0x0077);
	LCD_WriteReg(0x07F5, 0x0021);
	LCD_WriteReg(0x07F0, 0x0000);
	LCD_WriteReg(0x0007, 0x0173);

	/* ������ʾ���� WINDOWS */
	LCD_WriteReg(0x0210, 0);	/* ˮƽ��ʼ��ַ */
	LCD_WriteReg(0x0211, 239);	/* ˮƽ�������� */
	LCD_WriteReg(0x0212, 0);	/* ��ֱ��ʼ��ַ */
	LCD_WriteReg(0x0213, 399);	/* ��ֱ������ַ */
}

/*******************************************************************************
*	������: LCD_SetTextColor
*	��  ��: Color : �ı���ɫ
*	��  ��: ��
*	��  ��: �����ı���ɫ��������ȫ������s_TextColor
*/
void LCD_SetTextColor(__IO uint16_t Color)
{
	s_TextColor = Color;
}

/*******************************************************************************
*	������: LCD_SetBackColor
*	��  ��: Color : ������ɫ
*	��  ��: ��
*	��  ��: ���ñ�����ɫ��������ȫ������ s_BackColor
*/
void LCD_SetBackColor(__IO uint16_t Color)
{
	s_BackColor = Color;
}

/*******************************************************************************
*	������: LCD_ClearLine
*	��  ��: Line : �ı��к�0 - 9
*	��  ��: ��
*	��  ��: ���ѡ�����ı���
*/
void LCD_ClearLine(uint8_t Line)
{
	LCD_DisplayString(0, Line, "                          ");
}

/*******************************************************************************
*	������: LCD_Clear
*	��  ��: Color : ����ɫ
*	��  ��: ��
*	��  ��: �����������ɫֵ����
*/
void LCD_Clear(uint16_t Color)
{
	uint32_t index = 0;

	LCD_SetCursor(0, 0);	/* ���ù��λ�� */

	LCD_WriteRAM_Prepare(); 	/* ׼��д�Դ� */

	for (index = 0; index < 400 * 240; index++)
	{
		LCD->LCD_RAM = Color;
	}
}

/*******************************************************************************
*	������: LCD_SetCursor
*	��  ��: Xpos : X����; Ypos: Y����
*	��  ��: ��
*	��  ��: ���ù��λ��
*/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	/*
		px��py ���������꣬ x��y����������
		ת����ʽ:
		py = 399 - x;
		px = y;
	*/
	LCD_WriteReg(0x0200, Ypos);  		/* px */
	LCD_WriteReg(0x0201, 399 - Xpos);	/* py */
}

/*******************************************************************************
*	������: LCD_SetTransparent
*	��  ��: _mode ; ͸����־��1��ʾ͸�� 0 ��ʾ��͸��
*	��  ��: ��
*	��  ��: ���ù��λ��
*/
void LCD_SetTransparent(uint8_t _mode)
{
	if (_mode == 0)
	{
		s_Transparent = 0;	/* ����Ϊ������͸�� */
	}
	else
	{
		s_Transparent = 1;	/* ����Ϊ����͸�� */
	}
}

/*******************************************************************************
*	������: LCD_DrawChar
*	��  ��:
*		Xpos : X����;
*		Ypos: Y���ꣻ
*		c : ָ���ַ������ָ��
*		width : ����Ŀ�ȣ��ַ���8��������16
*	��  ��: ��
*	��  ��: ��LCD����ʾһ���ַ�(16x24)
*/
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c, uint8_t width)
{
	uint32_t index = 0, i = 0;
	uint8_t Yaddress;

	Yaddress = Ypos;

	LCD_SetCursor(Xpos, Ypos);

	if (s_Transparent == 0)
	{
		for (index = 0; index < 16; index++)	/* �ַ��߶� */
		{
			LCD_WriteRAM_Prepare();
			for (i = 0; i < width; i++)	/* �ַ���� */
			{
				if ((c[index * (width / 8) + i / 8] & (0x80 >> (i % 8))) == 0x00)
				{
					LCD_WriteRAM(s_BackColor);
				}
				else
				{
					LCD_WriteRAM(s_TextColor);
				}
			}
			Yaddress++;
			LCD_SetCursor(Xpos, Yaddress);
		}
	}
	else	/* ʵ�����ֵ�����ͼƬ�ϵĹ��� */
	{
		for (index = 0; index < 16; index++)	/* �ַ��߶� */
		{
			uint16_t x = Xpos;

			for (i = 0; i < width; i++)	/* �ַ���� */
			{
				if ((c[index * (width / 8) + i / 8] & (0x80 >> (i % 8))) != 0x00)
				{
					LCD_SetCursor(x, Yaddress);

					LCD->LCD_REG = 0x202;
					LCD->LCD_RAM = s_TextColor;
				}
				x++;
			}
			Yaddress++;
		}
	}
}

/*******************************************************************************
*	������: LCD_DisplayString
*	��  ��:
*		Xpos : X���� 0 - 399
*		Ypos : Y���� 0 - 239
*		ptr  : �ַ���ָ�룻
*	��  ��: ��
*	��  ��: ��LCDָ���ı�����ʾһ���ַ���
*/
void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, uint8_t *ptr)
{
	uint32_t i = 0;
	uint8_t code1;
	uint8_t code2;
	uint32_t address;


	while ((*ptr != 0) & (i < 50))
	{
		code1 = *ptr;	/* ascii���� ���ߺ��ִ���ĸ��ֽ� */
		if (code1 < 0x80)
		{
			LCD_DrawChar(Xpos, Ypos, &Ascii16[code1 * 16], 8);	/* 16 ��ʾ1���ַ���ģ���ֽ��� */
			Xpos += 8;		/* �е�ַ+8 */
		}
		else	/* �������� */
		{
			code2 = *++ptr;
			if (code2 == 0)
			{
				break;
			}
			/* ����16�����ֵ����ַ
				ADDRESS = [(code1-0xa1) * 94 + (code2-0xa1)] * 32
				;
			*/
			address = ((code1-0xa1) * 94 + (code2-0xa1)) * 32 + HZK16_ADDR;
			LCD_DrawChar(Xpos, Ypos, (const uint8_t *)address, 16);
			Xpos += 16;		/* �е�ַ+16 */
		}
		ptr++;			/* ָ����һ���ַ� */
		i++;
	}
}

/*******************************************************************************
*	������: LCD_SetDisplayWindow
*	��  ��:
*		Xpos : ��ʾ�к�
*		Ypos  : �ַ���ָ�룻
*		Height: ���ڸ߶�
*		Width : ���ڿ��
*	��  ��: ��
*	��  ��: ������ʾ����
*/
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width)
{
	/* ˮƽ��ʼ��ַ */
	if (Xpos >= Height)
	{
		LCD_WriteReg(R80, (Xpos - Height + 1));
	}
	else
	{
		LCD_WriteReg(R80, 0);
	}

	/* ˮƽ������ַ */
	LCD_WriteReg(R81, Xpos);

	/* ��ֱ��ʼ��ַ */
	if (Ypos >= Width)
	{
		LCD_WriteReg(R82, (Ypos - Width + 1));
	}
	else
	{
		LCD_WriteReg(R82, 0);
	}

	/* ��ֱ������ַ */
	LCD_WriteReg(R83, Ypos);

	LCD_SetCursor(Xpos, Ypos);
}

/*******************************************************************************
*	������: LCD_WindowModeDisable
*	��  ��: ��
*	��  ��: ��
*	��  ��: �˳�������ʾģʽ����Ϊȫ����ʾģʽ
*/
void LCD_WindowModeDisable(void)
{
	LCD_SetDisplayWindow(239, 399, 240, 400);
	LCD_WriteReg(R3, 0x1018);	/* ����Ĵ����ı�ɨ�跽�� */
}

/*******************************************************************************
*	������: LCD_DrawLine
*	��  ��: Xpos ��X����
*			YPos ��Y����
*			Length    ������
*			Direction ������(Horizontal��Vertical)
*	��  ��: ��
*	��  ��: ��LCD�ϻ�1����
*/
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
	uint32_t i = 0;

	/* �����ߵ���ʼ���� */
	LCD_SetCursor(Xpos, Ypos);

	if (Direction == Horizontal)	/* ˮƽ���� */
	{
		LCD_WriteRAM_Prepare();
		for (i = 0; i < Length; i++)
		{
			LCD_WriteRAM(s_TextColor);
		}
	}
	else	/* ��ֱ���� */
	{
		for (i = 0; i < Length; i++)
		{
			LCD_WriteRAM_Prepare();
			LCD_WriteRAM(s_TextColor);
			Ypos++;
			LCD_SetCursor(Xpos, Ypos);
		}
	}
}

/*******************************************************************************
*	������: LCD_DrawPoints
*	��  ��: _points ������������
*			_offset : ƫ������
*			_Size : �����
*			_Color ����ɫ
*	��  ��: ��
*	��  ��: ��LCD�ϻ�һ���
*/
void LCD_DrawPoints(POINT_T _offset, POINT_T *_points, uint16_t _Size, uint16_t _Color)
{
#if 1
	uint16_t i;

	for (i = 0 ; i < _Size; i++)
	{
		/* �����ߵ���ʼ���� */
		LCD_SetCursor(_points[i].x + _offset.x, _points[i].y + _offset.y);
		/* �޸��Դ����� */
		LCD_WriteReg(0x202, _Color);
	}
#else
	/* �������Ż���Ĵ��� */
	for (i = 0 ; i < _Size; i++)
	{
		LCD->LCD_REG = 0x0200;
		LCD->LCD_RAM = _points[i]->x;

		LCD->LCD_REG = 0x0201;
		LCD->LCD_RAM = _points[i]->y;

		LCD->LCD_REG = 0x0202;
		LCD->LCD_RAM = _Color;
	}
#endif

}
/*******************************************************************************
*	������: LCD_DrawRect
*	��  ��: Xpos ��X����
*			YPos ��Y����
*			Height ���߶�
*			Width  �����
*	��  ��: ��
*	��  ��: ��LCD�����һ�����ο�
*/
void LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width,uint16_t color)
{
	u16 i;
	u16 temp=s_TextColor;
	s_TextColor=color;
	
	for(i=0;i<Height;i++)
		LCD_DrawLine(Xpos, Ypos+i, Width, Horizontal);
	
	s_TextColor= temp;
}
/*******************************************************************************
*	������: LCD_DrawRect
*	��  ��: Xpos ��X����
*			YPos ��Y����
*			Height ���߶�
*			Width  �����
*	��  ��: ��
*	��  ��: ��LCD�ϻ�һ�����ο�
*/
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
	/*
	 ---------------->---
	|(Xpos��Ypos)        |
	V                    V  Height
	|                    |
	 ---------------->---
		  Width
	*/

	LCD_DrawLine(Xpos, Ypos, Width, Horizontal);  				/* �� */
	LCD_DrawLine(Xpos, Ypos + Height, Width, Horizontal);		/* �� */

	LCD_DrawLine(Xpos, Ypos, Height, Vertical);					/* �� */
	LCD_DrawLine(Xpos + Width, Ypos, Height, Vertical);	/* �� */
}

/*******************************************************************************
*	������: LCD_DrawCircle
*	��  ��: Xpos ��X����
*			Radius ��Բ�İ뾶
*	��  ��: ��
*	��  ��: ��LCD�ϻ�һ��Բ
*/
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		LCD_SetCursor(Xpos + CurX, Ypos + CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(s_TextColor);

		LCD_SetCursor(Xpos + CurX, Ypos - CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(s_TextColor);

		LCD_SetCursor(Xpos - CurX, Ypos + CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(s_TextColor);

		LCD_SetCursor(Xpos - CurX, Ypos - CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(s_TextColor);

		LCD_SetCursor(Xpos + CurY, Ypos + CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(s_TextColor);

		LCD_SetCursor(Xpos + CurY, Ypos - CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(s_TextColor);

		LCD_SetCursor(Xpos - CurY, Ypos + CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(s_TextColor);

		LCD_SetCursor(Xpos - CurY, Ypos - CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(s_TextColor);

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/*******************************************************************************
*	������: LCD_DrawMonoPict
*	��  ��: Pict �� ͼƬ����ָ��
*	��  ��: ��
*	��  ��: ��LCD�ϻ�һ����ɫͼƬ
*/
void LCD_DrawMonoPict(const uint32_t *Pict)
{
	uint32_t index = 0, i = 0;

	LCD_SetCursor(0, 400);

	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
	for (index = 0; index < 2400; index++)
	{
		for (i = 0; i < 32; i++)
		{
			if ((Pict[index] & (1 << i)) == 0x00)
			{
				LCD_WriteRAM(s_BackColor);
			}
			else
			{
				LCD_WriteRAM(s_TextColor);
			}
		}
	}
}

/*******************************************************************************
*	������: LCD_WriteBMP
*	��  ��: ptr �� ͼƬ����ָ��
*	��  ��: ��
*	��  ��: ��LCD����ʾһ��BMPλͼ
*/
void LCD_WriteBMP(const uint16_t *ptr)
{
	uint32_t index = 0;

	LCD_WriteRAM_Prepare();

	for (index = 0; index < 96000; index++)
	{
		LCD->LCD_RAM = ptr[index];//*p++;
	}
}

/*******************************************************************************
*	������: LCD_WriteReg
*	��  ��: LCD_Reg ���Ĵ�����ַ;  LCD_RegValue : �Ĵ���ֵ
*	��  ��: ��
*	��  ��: �޸�LCD�������ļĴ�����ֵ
*/
void LCD_WriteReg(__IO uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
	/* Write 16-bit Index, then Write Reg */
	LCD->LCD_REG = LCD_Reg;
	/* Write 16-bit Reg */
	LCD->LCD_RAM = LCD_RegValue;
}

/*******************************************************************************
*	������: LCD_ReadReg
*	��  ��: LCD_Reg ���Ĵ�����ַ
*	��  ��: �Ĵ�����ֵ
*	��  ��: ��LCD�������ļĴ�����ֵ
*/
uint16_t LCD_ReadReg(__IO uint16_t LCD_Reg)
{
	/* Write 16-bit Index (then Read Reg) */
	LCD->LCD_REG = LCD_Reg;
	/* Read 16-bit Reg */
	return (LCD->LCD_RAM);
}

/*******************************************************************************
*	������: LCD_WriteRAM_Prepare
*	��  ��: ��
*	��  ��: ��
*	��  ��: д�Դ�ǰ��׼�����������Դ�Ĵ�����ַ��
*/
void LCD_WriteRAM_Prepare(void)
{
	LCD->LCD_REG = 0x202;
}

/*******************************************************************************
*	������: LCD_WriteRAM
*	��  ��: RGB_Code : ��ɫ����
*	��  ��: ��
*	��  ��: д�Դ棬�Դ��ַ�Զ�����
*/
void LCD_WriteRAM(uint16_t RGB_Code)
{
	/* Write 16-bit GRAM Reg */
	LCD->LCD_RAM = RGB_Code;
}

/*******************************************************************************
*	������: LCD_ReadRAM
*	��  ��: ��
*	��  ��: �Դ�����
*	��  ��: ���Դ棬��ַ�Զ�����
*/
uint16_t LCD_ReadRAM(void)
{
#if 0
  /* Write 16-bit Index (then Read Reg) */
  LCD->LCD_REG = 0x202; /* Select GRAM Reg */
#endif

  /* Read 16-bit Reg */
  return LCD->LCD_RAM;
}

/*******************************************************************************
*	������: LCD_DisplayOn
*	��  ��: ��
*	��  ��: ��
*	��  ��: ����ʾ
*/
void LCD_DisplayOn(void)
{
	/* Display On */
	LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */
}

/*******************************************************************************
*	������: LCD_DisplayOff
*	��  ��: ��
*	��  ��: ��
*	��  ��: �ر���ʾ
*/
void LCD_DisplayOff(void)
{
	/* Display Off */
	LCD_WriteReg(R7, 0x0);
}

/*******************************************************************************
*	������: LCD_CtrlLinesConfig
*	��  ��: ��
*	��  ��: ��
*	��  ��: ����LCD���ƿ��ߣ�FSMC�ܽ�����Ϊ���ù���
*/
static void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* ʹ�� FSMC, GPIOD, GPIOE, GPIOF, GPIOG �� AFIO ʱ�� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                         RCC_APB2Periph_AFIO, ENABLE);

  /* ���� PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) Ϊ����������� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* ���� PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) Ϊ����������� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* ���� PF.00(A0 (RS))  Ϊ����������� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* ���� PG.12(NE4 (LCD/CS)) Ϊ����������� - CE3(LCD /CS) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

}

/*******************************************************************************
*	������: LCD_FSMCConfig
*	��  ��: ��
*	��  ��: ��
*	��  ��: ����FSMC���ڷ���ʱ��
*/
static void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTimingInitStructure;

  /*-- FSMC Configuration ------------------------------------------------------*/
  /*----------------------- SRAM Bank 4 ----------------------------------------*/
  /* FSMC_Bank1_NORSRAM4 configuration */
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 1;
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 2;
  FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_B;

  /* Color LCD configuration ------------------------------------
     LCD configured as follow:
        - Data/Address MUX = Disable
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable
        - Extended Mode = Enable
        - Asynchronous Wait = Disable */
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

  /* - BANK 3 (of NOR/SRAM Bank 0~3) is enabled */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

