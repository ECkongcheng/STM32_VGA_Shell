#include "delay.h"
#include "stdio.h"
#include "stdlib.h"
#include "led.h"
#include "graphical.h"
#include "vga.h"
#define VTOTAL	VID_HSIZE+2					            /*ͨ��SPI���͵����ֽ���*/
extern uint8_t fb[VID_VSIZE][VID_HSIZE+2];	            /* ֡������ 200x(50+2)x8*/
volatile uint16_t vline = 0;				      /* ���ڻ��Ƶĵ�ǰ�� */
volatile uint32_t vflag = 0;				      /* ��1ʱ��SPI DMA�����������Ļ�ϻ��ơ� */
volatile uint32_t vdraw = 0;				      /* ����ÿ3����������V�� */ 


void vga_device_init(void)
{
	vga_GPIOinit();
	spi_config();
	vga_timer_init();
	ga_clear_screen();
}
void vga_loop(void)
{
	ga_clear_screen();
	test();
}
//======================================================== JUST A LINE =================================================================



//============================== JUST A LINE ==================================
/**
  * @brief   VGA���GPIO����.
	* @note    
  * @param   void.
  * @retval  None.
  */
void vga_GPIOinit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
}
/**
  * @brief   VGA��ض�ʱ������.
	* @note    VSYNC HSYNC
  * @param   void.
  * @retval  None.
  */
void vga_timer_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef nvic;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	uint32_t TimerPeriod = 0;
	uint16_t Channel1Pulse = 0, Channel2Pulse = 0, Channel3Pulse = 0;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*
		�ֱ��� SVGA   800x600 @ 56 Hz
		��ֱˢ��Ƶ��	35.15625 kHz
		����ˢ��Ƶ��	36.0 MHz
		ϵͳʱ��		@ 72Mhz = 0.0139 us
	*/
	
	/*
		ˮƽ��ʱ
		-----------------
	
		��ʱ��1���� = 35156 Hz
		
		��ʱ��1ͨ��1PWM����ߵ�ƽHSYNC�źų���ʱ��28.4 us.
		28.4 us	= ˮƽ�ع� + ˮƽͬ��B + ˮƽɨ�� + ˮƽͬ��A.
		HSYNC������Ϊ2΢��,���㷽ʽ:2us/0.0139us = 144��ϵͳʱ������.
		
	  ��ʱ��1ͨ��2����һ��ˮƽͬ��B������.
		���VFLAG��1�����жϽ�����DMA����������Ļ�ϻ��ơ�
		����DMA�Ĵ��ʹ�Լ��Ҫ800���룬���Խ�����һЩ�����ʱ�䡣
		HSYNC + ˮƽ�ع�B�ļ��㷽ʽ��:
		(2us + 3.55us - dma) / 0.0139 = +-350 ϵͳʱ������
	
		ˮƽɨ�趨ʱ��Ϣ
		----------------------

		��λ us
		--------------------------------------------		
		Visible area	800		22.222222222222
		Front porch		24		0.66666666666667
		Sync pulse		72		2
		Back porch		128		3.5555555555556
		Whole line		1024	28.444444444444
	
	*/

	TimerPeriod = 3770; /*2048(56HZ)*/
	Channel1Pulse = 320;/* HSYNC 144(56HZ)*/
	Channel2Pulse = 705;/* HSYNC(56HZ) + BACK PORCH 352*/
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
	TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);

	/* TIM1 �����������ʹ�� */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);

	/* TIM1��ģʽ */
	TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
	
	/*
		��ֱ��ʱ
		---------------
		
		��ֱͬ�����弫��Ϊ����

						��
		------------------------------
		Visible area	600
		Front porch		1
		Sync pulse		2
		Back porch		22
		Whole frame		625
		
	*/

	/* VSYNC (TIM2_CH2) �� VSYNC_BACKPORCH (TIM2_CH3) */
	/*PWMģʽ�е�ͨ��2��3���� */
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Gated);
	TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);              //����TIM1��Ϊ������
	
	TimerPeriod = 624;		/* Whole frame 625 */
	Channel2Pulse = 1;		/* Sync pulse 2*/
	Channel3Pulse = 24;		/* Back porch+Front porch 24*/
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	/*TIM2���������ú����ʹ��*/
	TIM_CtrlPWMOutputs(TIM2, ENABLE);

	/*�ж�TIM2*/
	nvic.NVIC_IRQChannel = TIM2_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 1;
  nvic.NVIC_IRQChannelSubPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&nvic);
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

	/*�ж�TIM1*/
	nvic.NVIC_IRQChannel = TIM1_CC_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 1;
  nvic.NVIC_IRQChannelSubPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&nvic);
	TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);
	
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}
/**
  * @brief   VGA �źŴ���.
	* @note    ����SPi��ʽ�����ź�.
  * @param   void.
  * @retval  None.
  */
void spi_config(void)
{
	NVIC_InitTypeDef nvic;
	SPI_InitTypeDef SPI_InitStructure;
	DMA_InitTypeDef	DMA_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA|
                         RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_Cmd(SPI1, DISABLE);
	DMA_DeInit(DMA1_Channel3);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &fb[0][0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = VTOTAL;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_CalculateCRC(SPI1, DISABLE);
	SPI_Cmd(SPI1, ENABLE);
	
	SPI1->CR2 |= SPI_I2S_DMAReq_Tx;
	
	nvic.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	DMA1_Channel3->CCR &= ~1;
	DMA1_Channel3->CNDTR = VTOTAL;
	DMA1_Channel3->CMAR = (uint32_t) &fb[0][0];
	
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
}

//*****************************************************************************
//	���IRQ����ˮƽ���ȵ�ĩ�����ɵġ�
//	�����Ƿ�����Ч�Ĵ�ֱ��ʼ֡��VFLAG�������ڣ�
//	������DMAͨ��SPI�豸�������֡�����ߡ�
//*****************************************************************************
__irq void TIM1_CC_IRQHandler(void)
{
	if (vflag) {
		DMA1_Channel3->CCR = 0x93;
	}
	TIM1->SR = 0xFFEF; //~TIM_IT_CC4; ~0000 0000 0001 0000
	
}

//*****************************************************************************
//	���IRQ���ڴ�ֱ���ȵ�ĩ�����ɵġ�
//	����vFLAG����������Ϊ1����Ч��ֱ֡����
//*****************************************************************************
__irq void TIM2_IRQHandler(void)
{
	vflag = 1;
	TIM2->SR = 0xFFFD; //~TIM_IT_CC1; ~0000 0000 0000 0010
}

//*****************************************************************************
//	����ж�����ÿһ�е�ĩβ���ɵġ�
//	���������кŲ���DMA�Ĵ�����������Ӧ����ָ�롣
//*****************************************************************************
__irq void DMA1_Channel3_IRQHandler(void)
{	
	DMA1->IFCR = DMA1_IT_TC3;
	DMA1_Channel3->CCR = 0x92;
	DMA1_Channel3->CNDTR = VTOTAL;
	vline++;
	if (vline >= VID_VSIZE) {
		vdraw = vline = vflag = 0;
		DMA1_Channel3->CMAR = (uint32_t) &fb[0][0];
	} 
	else {
		DMA1_Channel3->CMAR += VTOTAL;
	}
}
/*vga��Ļ���*/
void ga_clear_screen(void)
{
	uint16_t x, y;

	for (y = 0; y < VID_VSIZE; y++) {
		for (x = 0; x < VTOTAL; x++) {
			fb[y][x] = 0;
		}
	}
}
/*��Ļ����*/
void test(void)
{
	static uint16_t num;
	uint8_t str[500];
	ga_draw_text(0,0,(u8*)"SYS_FONT !\"#$%&'()*+`-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^ _`abcdefghijklmnopqrstuvwxyz{|}~",GDI_ROP_COPY);
	ga_draw_text(0,20,(u8*)" AMDEUS_SYSTEM V2.0",GDI_ROP_COPY);
	for(num = 30;num < 600;num += 20)
	{
		sprintf((char * )str,"SYSTEM:> EL PSY CONGROO L = %d",num);
		delay_ms(200);
		ga_draw_text(0,num,(u8*)"SYSTEM:> STM32 VGA TEST[56HZ]", GDI_ROP_COPY);
		delay_ms(200);
		ga_draw_text(0,num + 10,(u8*)str, GDI_ROP_COPY);
	}
	delay_ms(500);
}
