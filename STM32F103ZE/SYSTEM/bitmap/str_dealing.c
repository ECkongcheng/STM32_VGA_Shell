#include "str_dealing.h"
/*�ַ������ȼ���*/
uint16_t strLen(pu8 str)
{
    int	i = 0;
    while (*str != 0) {
        ++i;
        ++str;
    }
	return i;
}
/*�ַ������ݺͳ��ȱȽ�*/
_Bool str_compare(uint8_t * str1,uint8_t * str2)
{
	uint16_t str1_len = strLen(str1);
	uint16_t str2_len = strLen(str2);
	uint16_t num;
	if(str1_len == str2_len)
	{
		for(num = 0;num < str1_len;num++)
		{
			if(str1[num]!=str2[num])
			{
				return 0;
			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
	
}
/*�ַ���������ת�ƺ���*/
void strtoarr(uint8_t * str,uint8_t * arr)
{
//	uint16_t num;
//	for(num = 0;num < sizeof(str);num++)
//	{
//		arr[num] = str[num];
//	}	
	while(*str)
	{
		*arr++  = *str++;
	}
}