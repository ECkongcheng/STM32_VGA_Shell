#include <components.h>
#include <graphical.h>
#include <keyboard.h>

/*amadeus system �����ؼ�֧�ֿ�*/

/*-----------------------------�༭����----------------------------*/
//�༭�� �����ṹ������ edit_boxes

/**
  * @brief  Edit box initialization.
  * @note   �༭���ʼ������.
  * @parma  
  * @retval None
  */
void editBox_init(edit_boxes * eb)
{
	/*���Ʊ༭�����*/
	ga_draw_rec(
			eb->rt.x,
    	eb->rt.y,
	    eb->rt.x + eb->rt.w,
	    eb->rt.y + eb->rt.h,
			GDI_ROP_COPY);
}
/**
  * @brief  Edit box active.
  * @note   �༭�򼤻��.
  * @parma  
  * @retval None
  */
void editBox_active(edit_boxes * eb)
{
	/*���������־λ����*/
	eb->key_cpt = RESET;
	/*����ǰ����ı༭���ַ���ݸ����̳����*/
	key_eb = eb;
}
/**
  * @brief  Edit box loop.
  * @note   �༭��ˢ����ʾ��������.
  * @parma  
  * @retval None
  */
void editBox_loop(edit_boxes * eb)
{
	uint8_t num;
	uint8_t str[100];
	if(!eb->ml)
	{
		/*δ���벿�ֿո񲹳�*/
		for(num = 0;num < eb->len_max;num++)
		{
			if(num < (key_eb->len_now))
			{
				if(eb->mod)
				{
					str[num] = eb ->mod;
				}
				else
				{
					str[num] = key_eb->key_str[num];
				}
			}
			else if(num >= (key_eb->len_now))
			{
				str[num] = ' ';
			}
		}
		for(num = 0;num < 100-eb->len_max;num++)
		{
			str[num+eb->len_max] = 0;
		}
		/*ˢ�±༭������ʾ������*/
		ga_draw_text(eb->rt.x+5,eb->rt.y+5,(uint8_t*)str,GDI_ROP_COPY);
	}
}
/**
  * @brief  getenter.
  * @note   ���enter�¼������غ���ر�־λ�����.
  * @parma  pw  ָ����Ӧ�༭��ṹ��
  *         clr �Ƿ��������
  * @retval None
  */
_Bool getenter(edit_boxes * eb,_Bool clr)
{
	if(eb->key_cpt)
	{
		if(clr)
		{
			key_clr();
		}
		eb->key_cpt = RESET;
		return 1;
	}
	return 0;
}



