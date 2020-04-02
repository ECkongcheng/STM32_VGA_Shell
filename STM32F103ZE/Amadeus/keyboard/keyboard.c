#include "keyboard.h"
#include "components.h"


edit_boxes * key_eb;

KeyDataTyepdef g_keydata = {.ch = 0,.ct = 0};


/**
  * @brief  key input receive.
  * @note   key board usart input ache.
  * @parma  key key of input
  * @retval None
  */
void key_get(uint8_t key)
{
	/*��������*/
	if(!key_eb->key_cpt)
	{
		/*�ַ�����*/
		if(key >= 0x20)
		{
			/*�������볤��*/
			if(key_eb->len_now < key_eb->len_max)
			{
				key_eb->key_str[key_eb->len_now++] = key;
			}
			//�������ݱ���
			g_keydata.ch = key;
			
		}
		/*��������*/
		else
		{
			//�������ݱ���
			g_keydata.ct = key;
			switch(key)
			{
				/*ctrl*/
				case 0x01: break;
				/*cap*/
				case 0x02: break;
				/*del*/
				case 0x03:
					if(key_eb->len_now > 0)
					{
						key_eb->key_str[key_eb->len_now--] = 0;
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
					key_eb->key_cpt = SET;
					break;
			}
		}
	}
}
/**
  * @brief  key ache clear function.
  * @note   key board usart input ache del.
  * @parma  void.
  * @retval None.
  */
void key_clr(void)
{
	uint8_t num;
	for(num = 0;num < 100;num++)
	{
		key_eb->key_str[num] = 0; 
	}
	key_eb->len_now = 0;
}

uint8_t getCharkey(void)
{
  uint8_t t = g_keydata.ch;
	g_keydata.ch = 0;
	return t;
}
uint8_t getCtrlkey(void)
{
  uint8_t t = g_keydata.ct;
	g_keydata.ct = 0;
	return t;
}
