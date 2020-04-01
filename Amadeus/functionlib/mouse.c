/**
  ******************************************************************************
  * @file    mouse.c
  * @author  �Ŷ�
  * @version V1.0.0			
  * @date    2019-10-2 	���̱��C89��׼ΪC99 
  * @brief   ������������Ӧ�ó���
  ******************************************************************************
  */
/*include --------------------------------------------------------------------*/	
#include "stm32f10x.h"
#include "mouse.h"
#include "tmnio.h"
#include "keyboard.h"
#include "graphical.h"
#include "stdio.h"
/*Golbal Data Space ----------------------------------------------------------*/

PosWallTypedef g_mousePos;


/*
  ___  _____  ____      ____________      ____    ____    
 |\  \|     \|    \    |\    ____   \    |\   \   \   \   
 \ \    /\    /\   \   \ \   \   \   \   \ \   \   \   \  
  \ \   \ \   \ \   \   \ \   \___\   \   \ \   \___\   \ 
   \ \___\ \ __\ \___\   \ \___________\   \ \___________\
    \|___| |___|  |___|   \|___________|    \|___________|


    __________      __________
   /    ______\    |\    ___   \
  |\ __\ _____|_   \ \  \___\   \
   \|_  _______ \   \ \    _____/_
    /_________/ |    \ \ _________\
   |__________|/      \|__________|

*/
void ShowVersion(void)
{
	SysOutInfo((uint8_t *)"  ___  _____  ____      ____________      ____    ____");
	SysOutInfo((uint8_t *)" |\\  \\|     \\|    \\    |\\    ____   \\    |\\   \\   \\   \\");
	SysOutInfo((uint8_t *)" \\ \\    /\\    /\\   \\   \\ \\   \\   \\   \\   \\ \\   \\   \\   \\");
	SysOutInfo((uint8_t *)"  \\ \\   \\ \\   \\ \\   \\   \\ \\   \\___\\   \\   \\ \\   \\___\\   \\");
	SysOutInfo((uint8_t *)"   \\ \\___\\ \\ __\\ \\___\\   \\ \\___________\\   \\ \\___________\\");
	SysOutInfo((uint8_t *)"    \\|___| |___| |___|    \\|___________|    \\|___________|");
	
	SysOutInfo((uint8_t *)"    __________      __________");
	SysOutInfo((uint8_t *)"   /    ______\\    |\\    ___   \\");
	SysOutInfo((uint8_t *)"  |\\ __\\ _____|_   \\ \\  \\___\\   \\");
	SysOutInfo((uint8_t *)"   \\|_  _______ \\   \\ \\    _____/_");
	SysOutInfo((uint8_t *)"    /_________/ |    \\ \\ _________\\");
	SysOutInfo((uint8_t *)"   |__________|/      \\|__________|");
	SysOutInfo((uint8_t *)" ");
	SysOutInfo((uint8_t *)"Mouse Environment Map Creator V1.0 191002 by: zhangdong");
	SysOutInfo((uint8_t *)" ");
	SysOutInfo((uint8_t *)"Pass any key to exit.");
	SysOutInfo((uint8_t *)" ");
}

#define MSGXSTA 320

//��ʾ�汾��Ϣ
void ShowMessage(void)
{
	ga_draw_text(MSGXSTA,32,"Mouse Environment Map Creator V1.0 191002 by: zhangdong",GDI_ROP_COPY);
	ga_draw_text(MSGXSTA,44,"Powered by Amadeus System.",GDI_ROP_COPY);
	ga_draw_text(MSGXSTA,56,"Communication Port :UART4 115200bps passthrough 433Mhz radio.",GDI_ROP_COPY);
}

#define STAX  16			//��ʼ��X����
#define STAY  28			//��ʼ��Y����
#define WEGX  288			//���
#define WEGY  288			//�߶�
#define EDGE  8				//����Ԥ��

//���Ƶ�
void mouseDrawPoint(void)
{
	//���Ŵ�ӡ
	for(int x = 0;x < 15;x++)
	{
		for(int y = 0;y < 15;y++)
		{
			uint16_t posx = 16 + x * 18 + 6;
			uint16_t posy = 320 - y * 18 - 16;
			//ga_draw_rec(posx,posy,posx + 15,posy + 15,GDI_ROP_COPY);
			
			ga_draw_point(posx+16,posy-2,GDI_ROP_COPY);
			ga_draw_point(posx+16+1,posy-2,GDI_ROP_COPY);
			ga_draw_point(posx+16,posy+1-2,GDI_ROP_COPY);
			ga_draw_point(posx+16+1,posy-2+1,GDI_ROP_COPY);
		}
	}
}


//����ǽ
void mouseDrawWall(uint8_t x,uint8_t y,uint8_t type)
{
	/*
				L4B 					Active	Inactive
				|---B0	0��     1				0
				|---B1  90��    1        0
				|---B2	180��   1        0
				|---B3	270��	 1        0
	*/
	
	uint16_t x0 = 16 + x * 18 + 6;
	uint16_t y0 = 320 - y * 18 - 16;
	uint16_t x1 = x0 + 15;
	uint16_t y1 = y0 + 15;
	
	type = ~type;
	
	ga_draw_line(x0, y0, x1, y0, GDI_ROP_DEL);//90��
	ga_draw_line(x0, y1, x1, y1, GDI_ROP_DEL);//270��
	ga_draw_line(x0, y0, x0, y1, GDI_ROP_DEL);//180��
	ga_draw_line(x1, y0, x1, y1, GDI_ROP_DEL);//0��
	
	if(type & 0x02)ga_draw_line(x0, y0, x1, y0, GDI_ROP_COPY);//90��
	if(type & 0x08)ga_draw_line(x0, y1, x1, y1, GDI_ROP_COPY);//270��
	if(type & 0x04)ga_draw_line(x0, y0, x0, y1, GDI_ROP_COPY);//180��
	if(type & 0x01)ga_draw_line(x1, y0, x1, y1, GDI_ROP_COPY);//0��
	
}
//����ƽ̨
void mouseDrawAGV(uint16_t x,uint16_t y)
{
	static uint16_t xhist = 16 + 6;
	static uint16_t yhist = 320 - 16;
	
	x = 16 + x * 18 + 6;
	y = 320 - y * 18 - 16;
	
	ga_draw_circle(xhist+7,yhist+7,3,GDI_ROP_DEL);
	ga_draw_point(xhist+7, yhist+7, GDI_ROP_COPY);
	
	ga_draw_circle(x+7,y+7,3,GDI_ROP_COPY);
	
	xhist = x;
	yhist = y;
}


//ͨ�Ž��պ���
void uartMesRx(uint8_t dat)
{
	static uint8_t buf[20];
	static uint8_t len = 0;
	static uint8_t start = 0;
	//��ʼ��־
	if(dat == '<')
	{
		len = 0;
		start = 1;
	}
	//������־
	else if(dat == '>')
	{
		//��ȷ��ʼ
		if(start == 1)
		{
			//        01 23 456 7
			//��Ϣ��ʽ xx,yy,ttt,w
			g_mousePos.x = (buf[0] - 0x30) * 10 
									 + (buf[1] - 0x30);
																	 
			g_mousePos.y = (buf[3] - 0x30) * 10
									 + (buf[4] - 0x30);
																	 
			g_mousePos.t = (buf[6] - 0x30) * 100
									 + (buf[7] - 0x30) * 10
									 + (buf[8] - 0x30);
			
			g_mousePos.w = buf[10];
		}
		len = 0;
		start = 0;
	}
	//����
	else if(start == 1)
	{
		//������д�뻺����
		buf[len] = dat;
		len = (len >= 19) ? 0 : len + 1;
	}
}

//��Ϣ��ʾ
void posStateDisplay(PosWallTypedef p)
{
	uint8_t str[100];
	//������ʾ
	sprintf((char *)str,"AGV Position  X = [ %3d ]",p.x);
	ga_draw_text(MSGXSTA,88+12*0,str,GDI_ROP_COPY);
	sprintf((char *)str,"AGV Position  y = [ %3d ]",p.y);
	ga_draw_text(MSGXSTA,88+12*1,str,GDI_ROP_COPY);
	
	//��̬��ʾ
	sprintf((char *)str,"AGV Position th = [ %3d ]",p.t);
	ga_draw_text(MSGXSTA,88+12*2,str,GDI_ROP_COPY);
	
	//������ʾ
	sprintf((char *)str,"obstacle code = [ 0x0%X ]",p.w);
	ga_draw_text(MSGXSTA,88+12*4,str,GDI_ROP_COPY);
	
	sprintf((char *)str,"Th = 0   wall = [ %d ]",(p.w & 0x01));
	ga_draw_text(MSGXSTA,88+12*6,str,GDI_ROP_COPY);
	sprintf((char *)str,"Th = 90  wall = [ %d ]",(p.w & 0x02)>>1);
	ga_draw_text(MSGXSTA,88+12*7,str,GDI_ROP_COPY);
	sprintf((char *)str,"Th = 180 wall = [ %d ]",(p.w & 0x04)>>2);
	ga_draw_text(MSGXSTA,88+12*8,str,GDI_ROP_COPY);
	sprintf((char *)str,"Th = 270 wall = [ %d ]",(p.w & 0x08)>>3);
	ga_draw_text(MSGXSTA,88+12*9,str,GDI_ROP_COPY);
	
}

//��Ϣ����
void posStateUpdate(void)
{
	static PosWallTypedef hist;
	
	//���ݷ����ı�
	if(g_mousePos.x != hist.x || 
		 g_mousePos.y != hist.y ||
		 g_mousePos.t != hist.t ||
		 g_mousePos.w != hist.w )
	{
		//��ʾ��Ϣ
		posStateDisplay(g_mousePos);
		//���µ�ͼ
		mouseDrawWall(g_mousePos.x,g_mousePos.y,g_mousePos.w);
		//����λ��
		mouseDrawAGV(g_mousePos.x,g_mousePos.y);
		
		//����Ϊ��ʷ����
		hist.x = g_mousePos.x;
		hist.y = g_mousePos.y;
		hist.t = g_mousePos.t;
		hist.w = g_mousePos.w;
		
	}
}
void mouseTest(void)
{
	uint8_t posx[] =
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,14,13,12,11,10,9,8,7,6,
		5,4,3,3,4,5,5,4,4,3,3,3,4,4,5,5,4,4,3,3,3,4,4,5,6,7,8,9,10,10,10,10,9,9,8,8,7,6,6,7,7,8,8,7,6,6,7,
	};
	uint8_t posy[] =
	{
		1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,14,13,13,13,13,13,
		13,13,13,13,13,13,13,13,12,12,12,11,11,10,10,9,8,8,7,7,6,6,5,5,4,3,3,2,2,2,2,2,2,2,3,4,5,5,4,4,3,3,3,4,4,5,5,6,6,6,7,7,
	};
	
	uint8_t map[] =
	{
	0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x09,0x05,0x05,0x05,0x05,
	0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x0D,0x0A,0x0E,0x05,0x05,0x05,0x05,0x05,0x05,
	0x05,0x05,0x0D,0x05,0x05,0x0D,0x02,0x05,0x04,0x06,0x05,0x0E,0x01,0x0A,0x03,0x0C,0x07,0x0E,0x0A,
	0x05,0x0E,0x01,0x0A,0x03,0x05,0x07,0x05,0x05,0x05,0x05,0x05,0x05,0x0A,0x0A,0x0A,0x09,0x0A,0x0D,
	0x0E,0x05,0x03,0x0A,0x07,0x09,0x04,0x0D,0x05,0x07,0x08,0x05,
	};
	static uint8_t i = 0;
		mouseDrawWall(posx[i],posy[i],map[i]);
		mouseDrawAGV(posx[i],posy[i]);
	
	i++;
	if(i > 88)
	{
		i = 0;
	}
	
	delay_ms(500);
}

void MouseSetup(void)
{
	//��ʾ�汾����Ϣ
	ShowVersion();
	ShowMessage();
	
	//���Ƶ�ͼ���
	ga_draw_rec(STAX,STAY,WEGX+STAX+EDGE,WEGY+STAY+EDGE,GDI_ROP_COPY);
	
	//���Ƶ�
	mouseDrawPoint();
	
	//��ʼ����
	g_mousePos.x = 0;
	g_mousePos.y = 0;
	g_mousePos.w = 0x0F;
	
	//����
	// mouseTest();
}

void MouseLoop(void)
{
	//mouseTest();
	tmnUpdate();			//�ն���ʾ����
	KeyBoardEnable();	//��������ʹ��
	posStateUpdate();	//״̬����
}


uint8_t Mouse(void * param)
{
	uint8_t exit = 0;

	MouseSetup();
	
	while(1)
	{
		MouseLoop();//
		//��������˳�
		if(getCharkey() != 0 || getCtrlkey() != 0)
		{
			return 1;
		}
	}
	
}









