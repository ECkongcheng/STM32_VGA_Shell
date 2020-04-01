/**
  ******************************************************************************
  * @file    tmnio.c
  * @author  �Ŷ�
  * @version V1.0.0			
  * @date    2019-10-2 	���̱��C89��׼ΪC99 
  * @brief   �ն˴��ڵ��������ʵ�ַ�������
  ******************************************************************************
  */
	
/*include --------------------------------------------------------------------*/	
#include "tmnio.h"
#include "components.h"
#include "str_dealing.h"
#include "graphical.h"
#include "keyboard.h"
#include "libMaster.h"

#include "stdio.h"
/*Golbal Data Space ----------------------------------------------------------*/	
edit_boxes 				g_tmlobj;    		//�ն˽����༭��
TmlStateTypedef		g_TmlState = Application;//�ն�״̬��

uint8_t g_tmlbuf[24][70];		//�ն˽����ı�������
uint8_t *g_tmlbufpnt[24];		//��ǰ��ʾ��ָ��
uint8_t g_lineKeyCur = 0;		//��ǰ�����н���

uint8_t g_tmlcmd[50];				//�ն�ָ��

//�ն˽�����ʾˢ��
void display_editbox(edit_boxes *eb,uint8_t *tl[24])
{
	uint8_t str[70];     //�л�����
	uint8_t num,line;    //�ݴ����
	uint16_t x,y;        //xy��������
	
	for(line = 0;line < eb->nl;line++)
	{
		uint8_t clrflag = 0;
		
		/*����ʾ*/
		for(num = 0;num < eb->len_max;num++)
		{
			
			/*�жϵ�ǰ�Ƿ�Ϊ���ַ�*/
			if(tl[line][num] == 0x00)
			{
				clrflag = 1;
			}
			
			//�ɵ�һ���������ſ�ʼ�������ո�
			if(clrflag == 0)
			{
				str[num] = tl[line][num];
			}
			else
			{
				str[num] = ' ';
			}
		}
		/*ˢ�µ�ǰ��*/
		ga_draw_text(eb->rt.x+5,eb->rt.y+5+line*10,(uint8_t*)str,GDI_ROP_COPY);
	}
}
void tmnUpdate(void)
{
	display_editbox(&g_tmlobj,g_tmlbufpnt);
}


//�ƶ�ȫ�����
void moveLineCur(void)
{
	static uint8_t fullflag = 0; //������־λ
	
	if(fullflag == 0) //��ǰ״̬δʹ�ܹ�����ʾ
	{
		//ʹ�ܹ�����ʾ
		g_lineKeyCur++;
		if(g_lineKeyCur == 24)
		{
			g_lineKeyCur = 0;
			fullflag = 1;
		}
	}
	else//��ǰ״̬ʹ�ܹ�����ʾ 
	{
		g_lineKeyCur = (g_lineKeyCur >= 23) ? 0 : g_lineKeyCur + 1;
		//�ƶ���Ϣ��ʾ��ָ��
		for(int i = 0;i < 24;i++)
		{
			if((i + g_lineKeyCur) <= 23)
			{
				g_tmlbufpnt[i] = g_tmlbuf[i + g_lineKeyCur];
			}
			else
			{
				g_tmlbufpnt[i] = g_tmlbuf[(i + g_lineKeyCur - 1) - 23];
			}
		}
	}
}

//����Ϣд����ʾ����������һ��
void DisMessageNewLine(uint8_t * str)
{
	uint8_t cur = 0;//���
		
	//����Ϣд����ʾ������
	for(cur = 0;str[cur] != 0;cur++)
	{
		g_tmlbuf[g_lineKeyCur][cur] = str[cur];
	}
	//д����Ϣ��β
	g_tmlbuf[g_lineKeyCur][cur] = 0;
	
	//�ƶ����
	moveLineCur();

}

//ϵͳ��ʾ��� �Զ�����
void SysOutInfo(uint8_t * str)
{
	uint16_t num = 0;
	uint8_t flag = 0;
	uint16_t len = strLen(str);
	uint8_t strbuf[70];
	
	//��Ϣ���ȳ������� ��ʼ�Զ�����
	while(len > 65)
	{
		//��������
		for(int i = 0;i < 66;i++)
		{
			strbuf[i] = str[num + i];
		}
		strbuf[66] = 0;
		
		//��ʾ����
		DisMessageNewLine(strbuf);
		
		//�����±����־λ
		len -= 66;
		num += 66;
		flag = 1;
	}
	
	//��Ϣ�������޶���
	if(len != 0)
	{
		for(int i = 0;i < len;i++)
		{
			strbuf[i] = str[num + i];
		}
		strbuf[len] = 0;
		
		DisMessageNewLine(strbuf);
	}
}


extern KeyDataTyepdef g_keydata;	//�������ݴ洢�ṹ

//����������ʾ
void keyboardInput(void)
{
	//g_tmlbufpnt[g_lineKeyCur][0] = 0
	
	static uint8_t USER[18] = "k.makise@Amadeus>:"; 
	static uint8_t strbuf[70] = "k.makise@Amadeus>:";
	static uint8_t len = 0;//���볤��
	
	static uint8_t firstflag = 0;
	
	//���µ���ʱʹ��һ��������Ϊ������ʾ
	if(firstflag == 0)
	{
		firstflag = 1;
		SysOutInfo(" \0");
		moveLineCur();
		g_lineKeyCur = (g_lineKeyCur > 0) ? g_lineKeyCur - 1:23;
	}

	//�ַ�����
	if(g_keydata.ch != 0)
	{
		//�û�����ʾ 
		for(int i = 0;i < 18;i++)
		{
			strbuf[i] = USER[i];
		}
		//�ַ���ʾ
		strbuf[18 + len] = g_keydata.ch;
		len = (len < 40) ? len + 1:len;
		
		g_keydata.ch = 0;
	}

	//�����ʾ
	for(int i = 0;i < 65;i++)
	{
		if(i < (len + 18))
		{
			g_tmlbuf[g_lineKeyCur][i] = strbuf[i];
		}
		else
		{
			g_tmlbuf[g_lineKeyCur][i] = 0;
			strbuf[i] = 0;
		}
	}
	

	
	//��������
	if(g_keydata.ct != 0)
	{
		switch(g_keydata.ct)
		{
			/*ctrl*/
			case 0x01: break;
			/*cap*/
			case 0x02: break;
			/*del*/
			case 0x03:
				if(len >= 1)
				{
					len--;
					strbuf[18 + len] = 0;
				}
				break;
			/*up*/
			case 0x04:break;
			/*down*/
			case 0x05:break;
			/*lift*/
			case 0x06:break;
			/*right*/
			case 0x07:break;
			/*enter*/
			case 0x0d:
			case 0x0A:
			{
				moveLineCur();//�ƶ���ʾ�н���
			
				key_eb->key_cpt = SET;
				g_TmlState = Application;
			
				len = 0;
				firstflag = 0;
			
				uint8_t str[100];
				sprintf((char *)str,"Searching \"%s\"...",&strbuf[18]);
				SysOutInfo(str);
				
				for(int i = 0;i < 40;i++)
				{
					g_tmlcmd[i] = strbuf[18 + i];
				}
				
				break;
			}
		}

		g_keydata.ct = 0;
	}
}

void KeyBoardEnable(void)
{
	editBox_active(&g_tmlobj);
}


void Test(void)
{
	static uint16_t line = 0;
	
	uint8_t str[50];
	
	sprintf((char *)str,"TEST message No. %d.",line++);
	
	SysOutInfo(str);
	
	delay_ms (500);
}



/*main func ------------------------------------------------------------------*/
void tmnioSetup(void)
{
	/*�ն˽����༭������*/
	g_tmlobj.rt.x = 3;      //�������϶�������
	g_tmlobj.rt.y = 343;  
	g_tmlobj.rt.h = 250;       //���øߺͿ�
	g_tmlobj.rt.w = 468;
	
	g_tmlobj.mod = 0;          //Ĭ����ʾģʽ
	g_tmlobj.len_max = 70;     //����������볤��
	g_tmlobj.ml = 1;           //������ʾ
	g_tmlobj.nl = 24;           //����20

	SysOutInfo("Hello! I'm Makise Kurisu. yoroshiku~ ");
	SysOutInfo("Please enter \"help\" to see the command list.");
	
	/*�ն˽����༭���ʼ��*/
	editBox_init(&g_tmlobj);  //��ʼ���˻��༭��
	editBox_active(&g_tmlobj);//�ն˽����༭�������뼤��
	
	//ִ��Ӧ��
	g_TmlState = WaitCommand;
	
	//�����������
	g_keydata.ch = 0;
	g_keydata.ct = 0;
	
	//ָ�����鸳ֵ
	for(int i = 0;i < 24;i++)
	{
		g_tmlbufpnt[i] = g_tmlbuf[i];
	}
	
}

void tmnioLoop(void)
{
	
	switch(g_TmlState)
	{
		case Application:
		{
			//������ע���Ӧ�ó���ִ��
			uint8_t state = libMaster(g_tmlcmd);
			
			//ִ�н��
			if(state == 0)//δ��������������ָ��
			{
				uint8_t str[50];
				sprintf((char *)str,"\"%s\" Not installed.",g_tmlcmd);
				SysOutInfo("Please check and re-enter.[unknown command or function]");
				SysOutInfo(str);
			}
			else
			{
				uint8_t str[50];
				sprintf((char *)str,"Program has ended Return Code %d.",state - 1);
				SysOutInfo(str);
			}
			

			g_TmlState = WaitCommand;
			break;
		}
		case WaitCommand:
		{
			KeyBoardEnable();
			keyboardInput();
			break;
		}
		default:SysOutInfo("terminal state error.");break;
	}
	
	
	//Test();
	tmnUpdate();
}


void tmnioInterrupt(void)
{
	
}




