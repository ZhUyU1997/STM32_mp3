#include "stm32f10x.h"
#include "BackLight.h"
#include "eeprom.h"

uint8_t autosetBackLight;
uint8_t backLightValue;
/*******************************************************************************
	������: InitBackLight
	��  ��: _bright ���ȣ�0����255������
	��  ��:
	����˵������ʼ��GPIO,����ΪPWMģʽ

	��������� PB1, ���ù���ѡ�� TIM3_CH4

	(1)���رձ���ʱ��
		��CPU IO����Ϊ��������ģʽ���Ƽ�����Ϊ������������������͵�ƽ)
		��TIM3�ر� ʡ��

	(2)
*/
void SetBackLight(uint8_t _bright)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* ��1������GPIOB RCC_APB2Periph_AFIO ��ʱ��	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	if (_bright == 0)
	{
		/* ���ñ���GPIOΪ����ģʽ */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* �ر�TIM3 */
		TIM_Cmd(TIM3, DISABLE);
		return;
	}
	else if (_bright == BRIGHT_MAX)	/* ������� */
	{
		/* ���ñ���GPIOΪ�������ģʽ */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_SetBits(GPIOB, GPIO_Pin_1);

		/* �ر�TIM3 */
		TIM_Cmd(TIM3, DISABLE);
		return;
	}

	/* ���ñ���GPIOΪ�����������ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ʹ��TIM3��ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* -----------------------------------------------------------------------

	TIM3 ARR Register = 1894 => TIM3 Frequency = TIM3 counter clock/(ARR + 1)
	TIM3 Frequency = 37.99 KHz (�ӽ�38KHz ���ⷢ��ܵĵ��ƻ�Ƶ)
	TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
	----------------------------------------------------------------------- */

	/*
		TIM3 ����: ����1·PWM�ź�;
		TIM3CLK = 72 MHz, Prescaler = 0(����Ƶ), TIM3 counter clock = 72 MHz
		���㹫ʽ��
		PWM���Ƶ�� = TIM3 counter clock /(ARR + 1)

		������������Ϊ100Hz

		�������TIM3CLKԤ��Ƶ����ô�����ܵõ�100Hz��Ƶ��
		�������÷�Ƶ�� = 1000�� ��ô  TIM3 counter clock = 72KHz
		TIM_Period = 720 - 1;
		Ƶ���²�����
	 */
	TIM_TimeBaseStructure.TIM_Period = 720 - 1;	/* TIM_Period = TIM3 ARR Register */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	/*
		_bright = 1 ʱ, TIM_Pulse = 1
		_bright = 255 ʱ, TIM_Pulse = TIM_Period
	*/
	TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period * _bright) / BRIGHT_MAX;	/* �ı�ռ�ձ� */

	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}
/*******************************************************************************
	��������ADC_Configuration
	����˵��������ADC, PC1 ,��������
*/
void ADC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

    /* ʹ�� ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);              //72M/6=12,ADC���ʱ�䲻�ܳ���14M

	/* ����PC1Ϊģ������(ADC Channel11) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ����ADC1, ����DMA, ������Լ����� */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		 //ADC1����ģʽ:����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			 //��ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		 //����ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	  //ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	 //ADC1�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	   //˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);		   //����ADC_InitStruct��ָ���Ĳ�������ʼ������ADC1�ļĴ���

	/* ADC1 regular channel11 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��11,�������˳��ֵΪ1,����ʱ��Ϊ239.5����

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);		  //ʹ��ADC1

	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC1);						//����ADC1��У׼�Ĵ���
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));		//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�

	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);					//��ʼADC1��У׼״̬
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));		//�ȴ�У׼���

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ADC1�����ת����������
}
uint16_t GetLightValue(void)
{
	uint16_t value;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	        //�������ADCת��
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )); //�ȴ�ת������
	value = ADC_GetConversionValue(ADC1);		//��ȡADCֵ
	return value;
}

//���浽FM24C16�� 30~31��ε�ַ���䣬�ܹ�2���ֽ�,
//��һ���ֽڼ�¼�Ƿ��Զ����ã��ڶ�����¼����ֵ
void SaveBackLight(void)
{
	I2C_WriteByte(autosetBackLight, 30, ADDR_24LC08);
	I2C_WriteByte(backLightValue, 31, ADDR_24LC08);
}
//��ȡ����ֵ����û�У��򷵻�Ĭ��ֵ200
u8 GetBackLight(void)
{
	u8 ReadBuffer[2];
	I2C_ReadByte(ReadBuffer,2,30, ADDR_24LC08);
	if(ReadBuffer[1]==0)return 200;
	else return 	ReadBuffer[1];	 
}
//��ȡ�Ƿ��Զ�����
u8 GetAutoSet(void)
{
	u8 ReadBuffer[2];
	I2C_ReadByte(ReadBuffer,2,30, ADDR_24LC08);
	return 	ReadBuffer[0];	 
}

void BackLight_Init(void)
{
	uint16_t value;
	ADC_Configuration();

	autosetBackLight=GetAutoSet();
	if(autosetBackLight==1)
	{
		value=GetLightValue();
		backLightValue=255-(value>>7);
	}
	else  backLightValue=GetBackLight();
	SetBackLight(backLightValue);
}
