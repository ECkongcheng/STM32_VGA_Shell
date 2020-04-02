#include "stm32f10x.h"
#include "vga.h"
#include "login.h"
#include "delay.h"
#include "stdio.h"
#include "math.h"
#include "graphical.h"
#include "stdlib.h"
#include "keyboard.h"
#include "components.h"

/*�۲���ͼ����������*/
extern uint8_t fg_map[3040];
/*ϵͳ��Ϣ��������*/
extern uint8_t mes[10][60];
/*��ʾ��������*/
extern uint8_t fb[VID_VSIZE][VID_HSIZE+2];	
/*����������ͼ����������*/
extern uint8_t bmp_makise[32768];
/*��������ͼ����������*/
extern uint8_t bmp_misaka[2048];
/*����ͼ����������*/
extern uint8_t bmp_gear[2048];
/*Amadeusͼ����������*/
extern uint8_t bmp_amdeus[16384];
/*������Ϣ����*/
extern uint8_t cfg_mes[30][70];

/**
  * @brief    get_chipID.
  * @note     ��ȡоƬID.
  * @param    none.
  * @retval   none.
  * @note     96λ��ID��stm32Ψһ��ݱ�ʶ��������8bit��16bit��32bit��ȡ
  *            �ṩ�˴�˺�С�����ֱ�ʾ����.
  */
void get_chipID(u32* id)
{
  id[0] = *(__IO u32 *)(0X1FFFF7E8);  // ���ֽ�
  id[1] = *(__IO u32 *)(0X1FFFF7EC); // 
  id[2] = *(__IO u32 *)(0X1FFFF7F0); // ���ֽ�
}
/**
  * @brief  ϵͳӲ����Ϣ����.
  * @note   ���ݼ�bitmap��
  *
  * @retval None
  */
void Sys_inf(void)
{
	uint8_t   line;
	uint8_t   str[50];
	uint32_t  id[3];
  /*�۲���ͼ�����*/
	ga_bitblt(NULL,424,0,304,80,fg_map,GDI_ROP_COPY);
	/*display information of system and mcu*/
	for(line = 0;line < 10;line++)
	{
		ga_draw_text(0,line*15+5,(u8*)mes[line], GDI_ROP_COPY);
		delay_ms(100);
	}
	get_chipID(id);
	sprintf((char*)str,"MCUID : %8X%8X%8X (96bit)",id[0],id[1],id[2]);
	ga_draw_text(0,line*15+5,(u8*)str, GDI_ROP_COPY);
	ga_draw_line(0,174,725,174, GDI_ROP_COPY);
	ga_draw_line(0,176,725,176, GDI_ROP_COPY);
}

/**
  * @brief  ϵͳ������Ϣ����.
  * @note   ���ݼ�bitmap��
  *
  * @retval None
  */
void Sys_load(void)
{
	
	uint16_t r,line;

	/*�����Ļ*/
	ga_clear_screen();
	delay_ms(1000);
	delay_ms(1000);
	/*��ʾ������Ϣ*/
	for(line = 0;line < 30;line++)
	{
		ga_draw_text(0,line*30+5,(u8*)cfg_mes[line],GDI_ROP_COPY);
		delay_ms(10+line);
	}
	/*�����Ļ*/
	ga_clear_screen();
	delay_ms(1000);
	/*���ƾ�̬����Ʈ��*/
	ga_draw_ribbon(15,10,70,GDI_ROP_COPY);
	ga_draw_ribbon(16,510,70,GDI_ROP_COPY);
	/*������������ͷ��*/
  ga_bitblt(NULL,305,226,128,128,bmp_misaka,GDI_ROP_COPY);
	/*���ƽ��Ȼ�*/
	ga_draw_ring(370,290,80,13,2);
	/*�����Ļ*/
	ga_clear_screen();
	/*��������������ͼƬ*/
	ga_bitblt(NULL,112,40,512,512,bmp_makise,GDI_ROP_COPY);
	
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	for(r = 0;r < 600;r++)
	{
		ga_draw_circle(368,296,r,GDI_ROP_DEL);
		ga_draw_circle(367,296,r,GDI_ROP_DEL);
	}
};
/**
  * @brief  syslogin.
  * @note   system login function.
  *
  * @retval None
  */
void Sys_login(void)
{
	uint16_t th;//������ת�Ƕ�th
	
	uint8_t pswd_str[16] = "amadeus";//������������
	
	_Bool lock = SET;//����״̬��־λ
	edit_boxes id;//�˻��༭��ؼ��ṹ�����Ͷ���
	edit_boxes pw;//����༭��ؼ��ṹ�����Ͷ���
	
	/*-----------------------�˻��༭��-----------------------------*/
	/*�˻��༭������*/
	id.rt.x = 340;      //�������϶�������
	id.rt.y = 395;  
	id.rt.h = 15;       //���øߺͿ�
	id.rt.w = 120;
	
	id.ml = 0;          //������ʾ
	id.mod = 0;         //������ʾģʽ
	id.len_max = 16;    //����������볤��
	
	/*�˻��༭���ʼ��*/
	editBox_init(&id);  //��ʼ���˻��༭��
	editBox_active(&id);//�˻��༭�������뼤��
	strtoarr((uint8_t *)"K.makise",id.key_str);//�涨�û��˻�
	id.len_now = 8;
	
	/*��ʾ�˻��༭������*/
	editBox_loop(&id);
	
	/*-----------------------����༭��-----------------------------*/
	/*����༭������*/
	pw.rt.x = 340;      //�������϶�������
	pw.rt.y = 415;  
	pw.rt.h = 15;       //���øߺͿ�
	pw.rt.w = 120;
	
	pw.mod = '*';       //���������ʾģʽ
	pw.len_max = 16;    //����������볤��
	
	/*����༭���ʼ��*/
	editBox_init(&pw);  //��ʼ���˻��༭��
	editBox_active(&pw);//����༭�������뼤��
	
	
	/*-----------------------ͼƬ����ı���-------------------------*/
	//����bmp_amdeus
	ga_bitblt(NULL,100,100,512,256,bmp_amdeus,GDI_ROP_COPY);
	//��ӡ�ı���ǩ
	ga_draw_text(270,400,(uint8_t*)"USER_ID  :",GDI_ROP_COPY);
	ga_draw_text(270,420,(uint8_t*)"PASSWORD :",GDI_ROP_COPY);
	ga_draw_text(0,590,(uint8_t*)"Copyright ZhangDong 2018",GDI_ROP_COPY);
	ga_draw_text(610,590,(uint8_t *)"Amadeus System v2.76",GDI_ROP_COPY);
	/*-----------------��֤����,ˢ�������������ת------------------*/
	while(lock)
	{
		/*����༭��ˢ����ʾ*/
		editBox_loop(&pw);
		/*����������֤*/
		passward(&lock,pswd_str,&pw);
		/*���ֶ�̬��תЧ��*/
		th+=5;if(th > 625)th = 0;
		ga_draw_bmp_rotate(460,140,128,128,th,bmp_gear,GDI_ROP_COPY);
		ga_draw_bmp_rotate(544,224,128,128,625-th,bmp_gear,GDI_ROP_COPY);
	}
	for(th = 730;th >0;th--)
	{
		ga_draw_line(th,0,th,600,GDI_ROP_DEL);
	}
}
/**
  * @brief  password.
  * @note   login password function.
  * @parma
  * @retval None
  */
void passward(_Bool * lock,uint8_t * pswd_str,edit_boxes * pw)
{
	uint8_t num;
	/*�������*/
	if(getenter(pw,0))
	{
		/*��֤����*/
		if(str_compare(pw->key_str,pswd_str))
		{
			/*״̬����*/
			*lock = RESET;
		}
		else
		{
			/*�������*/
			ga_draw_text(325,440,(uint8_t*)"Your password is incorrect.",GDI_ROP_DEL);
			ga_draw_text(325,450,(uint8_t*)"Please re-enter your password. ",GDI_ROP_DEL);
			delay_ms(500);
			ga_draw_text(325,440,(uint8_t*)"Your password is incorrect.",GDI_ROP_COPY);
			ga_draw_text(325,450,(uint8_t*)"Please re-enter your password. ",GDI_ROP_COPY);
		}
		key_clr();
	}
}


