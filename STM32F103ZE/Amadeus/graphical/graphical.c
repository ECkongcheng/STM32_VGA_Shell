#include "graphical.h"
#include "math.h"
#include "delay.h"


uint8_t fb[VID_VSIZE][VID_HSIZE+2];	            /* ֡������ 200x(50+2)x8*/

extern const u8 ascii_word[];

//*****************************************************************************
//	λ��ת�ƺ�����
//  �������ʹ��STM32����λ��ģʽ���򻯸��ӵ�BITBLT���ܡ�
//
//	�� cortex STM32 F10X�ο��ֲᣨRM00 08����
//	ӳ�乫ʽʾ������ν����������е�ÿ���������õ�λ�������е���Ӧ���ء�ӳ�乫ʽΪ��
//	bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number ?4)
//	λ��:
//	bit_word_addr ��ӳ�䵽Ŀ��λ�ı����ڴ������еĵ�ַ��
//	bit_band_base �Ǳ����������ʼ��ַ��
//	byte_offset ����Ŀ��λ��������λ�������е��ֽ�����Ŀ��λ��λ��0��7����
//	����:
//	�����ʾ����ʾ����ڱ���������ӳ��λ��SRAM��ַ0x2000 300���ֽڵı���2��
//	0x22006008 = 0x22000000 + (0x300*32) + (2*4).
//	д���ַ0x2200��600��SRAM��ַ0x2000 300�ֽ��ϵ�λ2�Ķ��޸�д����������ͬ��Ч����
//	��ȡ��ַ0x2200��600��SRAM��ַ0x2000 300��0x01��λ����0x00��λ���ã�
//  �����ֽڵ�λ2��ֵ��0x01��0x00����
//
//	��һ���˽���ο� Cortex M3 �����ο��ֲ�
//
//	����:
//
//		prc		�������Ρ����е�X/Y������"prc"�ڣ����"prc"���꽫��������ʾ����
//		x			λͼX��ʼλ��
//		y			λͼY��ʼλ��
//		w			λͼ��ȣ�������Ϊ��λ
//		h			λͼ�߶ȣ�������Ϊ��λ
//		bm		ָ��λͼ��ʼλ�õ�ָ��
//		rop		��դ�������μ�GdiyRopxxxx����
//
//	��������	��
//
//*****************************************************************************
void ga_bitblt(PGDI_RECT prc, int16_t x, int16_t y, int16_t w, int16_t h, u8* bm, uint16_t rop)
{
    uint16_t i, xz, xb, xt;
    uint32_t wb;				// �ֽڿ��
    uint32_t r;					//��λ�������x����ʼxλ��
    uint32_t k;				
    uint32_t d;
    uint32_t offs;
    uint8_t  c;
    u8*	     fbPtr;				// ָ��֡������λ�������ָ��
    u8*	     fbBak;
    u8	     fb1;
    uint32_t fb2;
    uint32_t rp;
    u8*	     bmPtr;				//ָ��λͼλ��ָ��

    //�����������
	if (prc != NULL) {
		x = prc->x + x;
		y = prc->y + y;
	}

    //���ֽ�Ϊ��λ��ȡ��λͼ���
	wb = (uint32_t) w >> 3;
    //������С��1�ֽڣ���������Ϊ1�ֽ�
	if ((wb << 3) < (uint32_t) w) ++wb;

    //�ڵ�һ���ֽ��ڻ����ʼλ
	d = (uint32_t) x >> 3;
	r  = ((uint32_t) x - (d << 3));

    //����X
	if (prc == NULL) {
		if ((x + w) >= VID_PIXELS_X ) {
			xt =  VID_PIXELS_X - x;
		} else {
			xt = w;
		}
	} else {
		if ((x + w) >= (x + prc->w)) {
			xt = prc->w - x;
		} else {
			xt = w;
		}
	}

  //����λ
	for (i = 0; i < h; i++) {
        // Clip Y
		if ((i + y) > (VID_VSIZE - 1)) return;

    //��λ��ģʽ�¶�֡����������ƫ��
		offs = (((u32) x >> 3)) + ((u32) (y + i)  * VID_HSIZE_R);
		k = (u32) (&fb - 0x20000000);
		k += offs;
		fbPtr = (u8*) (0x22000000 + (k * 32) + ((7 - r) * 4));
		fbBak = (u8*) (0x22000000 + (k * 32) + 28);

    //���λͼλƫ��
		bmPtr = bm + ((u32) i * wb);
		xz = w;

		xb = 0;
		for (xz = 0; xz < xt; xz++) {
			fb1 = ((u32) fbPtr) & 0x000000E0;
			if (xb++ == 0) {
				c = *bmPtr;
				++bmPtr;
			}
			xb &= 0x07;
			(c & 0x80) ? (rp = 1) : (rp = 0);
			switch(rop) {
				case GDI_ROP_COPY:	*fbPtr = rp;		break;
				case GDI_ROP_XOR:	*fbPtr ^= rp;		break;
				case GDI_ROP_AND:	*fbPtr &= rp;		break;
				case GDI_ROP_OR:	*fbPtr |= rp;		break;
				case GDI_ROP_DEL:	*fbPtr &= ~rp;		break;
			}
			fbPtr -= 4;
			fb2 = ((u32) fbPtr) & 0x000000E0;
			if (fb1 != fb2) 
			{
				fbPtr = fbBak + 32;
				fbBak = fbPtr;
			}
			c <<= 1;
		}
	}
}

//*****************************************************************************
//	ʹ��ϵͳ������X/Yλ�û����ı���
//
//	������
//		x			  X��ʼλ��
//		y			  Y��ʼλ��
//		ptext		ָ���ı���ָ��
//		rop			��դ�������μ�GdiyRopxxxx����
//
//	return			none
//*****************************************************************************
void ga_draw_text(int16_t x, int16_t y, u8* ptext, u16 rop)
{

    u16	l, i, pos, xp;
    u8	c;
    u8*	ptx;

	l = strLen(ptext);
	xp = x;
	for (i = 0; i < l; i++) {
		c = *(ptext++);
		if (c >= GDI_SYSFONT_OFFSET) {
			pos = (u16)(c - GDI_SYSFONT_OFFSET) * GDI_SYSFONT_BYTEWIDTH * GDI_SYSFONT_HEIGHT;
			ptx = ((u8*) ascii_word) + pos;
			ga_bitblt(NULL, xp, y, GDI_SYSFONT_WIDTH, GDI_SYSFONT_HEIGHT, ptx, rop);
			xp += GDI_SYSFONT_WIDTH;
			if (xp >= VID_PIXELS_X) return;
		}
	}
}
//ָ��Ҫ���Ƶ���ĸ������
void ga_draw_nwords(int16_t x, int16_t y, u8* ptext, u16 rop, u16 l)
{

    u16	i, pos, xp;
    u8	c;
    u8*	ptx;

	xp = x;
	for (i = 0; i < l; i++) {
		c = *(ptext++);
		if (c >= GDI_SYSFONT_OFFSET) {
			pos = (u16) (c - GDI_SYSFONT_OFFSET) * GDI_SYSFONT_BYTEWIDTH * GDI_SYSFONT_HEIGHT;
			ptx = ((u8*) ascii_word) + pos;
			ga_bitblt(NULL, xp, y, GDI_SYSFONT_WIDTH, GDI_SYSFONT_HEIGHT, ptx, rop);
			xp += GDI_SYSFONT_WIDTH;
			if (xp >= VID_PIXELS_X) return;
		}
	}
}
//*****************************************************************************
//	�ھ����ڻ����ı�
//
//	����:
//		prc			���βü�ָ��
//		ptext		ָ���ı���ָ��
//		style		�ı���ʽ���μ�GdiyWiCabPosithXXX���壩
//		rop			Raster operation. See GDI_ROP_xxx defines
//
//	return			none
//*****************************************************************************
void ga_draw_textrec(PGDI_RECT prc,u8* ptext, uint16_t style, uint16_t rop) 
{
    uint16_t l1, l2, i, pos, xp;
    uint8_t	c;
    u8*	ptx;

    l1 = strLen(ptext);
	l2 = l1 * GDI_SYSFONT_WIDTH;
    
    // Decide where to start painting
	switch(style) {
		case GDI_WINCAPTION_RIGHT:		
            if (l2 < prc->w)
                prc->x += (prc->w - l2);
            break;
		case GDI_WINCAPTION_CENTER:	
            if (l2 < prc->w) 							
                prc->x += ((prc->w - l2) / 2);										
            break;
	}
	xp = 1; //prc->x;
	for (i = 0; i < l1; i++) {
		c = *(ptext++);
		if (c >= GDI_SYSFONT_OFFSET) {
            // Get the position of the letter in vga_word[]
			pos = (uint16_t) (c - GDI_SYSFONT_OFFSET) * GDI_SYSFONT_BYTEWIDTH * GDI_SYSFONT_HEIGHT;
			ptx = ((u8*) ascii_word) + pos;
            
			ga_bitblt(prc, xp, 0, GDI_SYSFONT_WIDTH, GDI_SYSFONT_HEIGHT, ptx, rop);
			xp += GDI_SYSFONT_WIDTH;
			if (xp >= ((prc->x + prc->w) - GDI_SYSFONT_WIDTH)) return;
		}
	}
}

//*****************************************************************************
//	ʹ�ô洢��GrMODE�����еĵ�ǰͼ��ģʽ��X/Yλ����ʾ��
//
//	����:
//		x			X ����
//		y			Y ����
//		rop		դ��������ῴ��_ ROP��GDI _ XXX
//
//	return:			none
//*****************************************************************************
void ga_draw_point(u16 x, u16 y, u16 rop)
{

  u16	w, r;
  u8	m;


    //������ʾ�������

	if (x >= VID_PIXELS_X || y >= VID_PIXELS_Y) return;

	w = x >> 3;
	r = x - (w << 3);

    // Prepare mask
	m = (0x80 >> r);

	switch(rop) {
		case GDI_ROP_COPY:	fb[y][w] |= m;break;
		case GDI_ROP_XOR:		fb[y][w] ^= m;break;
		case GDI_ROP_AND:		fb[y][w] &= m;break;
		case GDI_ROP_DEL:		fb[y][w] &= ~m;break;
	}
}
//*****************************************************************************
//	��Bresenham�㷨����ֱ��
//
//	�������ȡ���Ȿ�飺
//	����ʽ�����ͼ��ѧ���Զ����µ�OpenGL������Edward Angel
//
//	����:
//		x1			X��ʼλ��
//		y1			Y��ʼλ��
//		x2			X����λ��
//		y2			Y����λ��
//		rop			��դ����������GDI_ROPE_XXX�Ķ���
//
//	return			none
//*****************************************************************************
void ga_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t rop) 
{
    int16_t	dx, dy, i, e;
    int16_t	incx = 1, incy = 1, inc1, inc2;
    int16_t	x, y;

	dx = x2 - x1;
	dy = y2 - y1;

	if(dx < 0) {
        dx = -dx;
        incx = -1;
    }
	if(dy < 0) { 
        dy = -dy;
        incy = -1;
    }

	x=x1;
	y=y1;

	if (dx > dy) {
		ga_draw_point(x, y, rop);
		e = 2*dy - dx;
		inc1 = 2 * ( dy -dx);
		inc2 = 2 * dy;
		for (i = 0; i < dx; i++) {
			if (e >= 0) {
				y += incy;
				e += inc1;
			}
			else {
				e += inc2;
			}
			x += incx;
			ga_draw_point(x, y, rop);
		}
	} else {
		ga_draw_point(x, y, rop);
		e = 2 * dx - dy;
		inc1 = 2 * (dx - dy);
		inc2 = 2 * dx;
		for(i = 0; i < dy; i++) {
			if (e >= 0) {
				x += incx;
				e += inc1;
			} else {
				e += inc2;
			}
			y += incy;
			ga_draw_point(x, y, rop);
		}
	}
}
//*****************************************************************************
//	���ƾ���
//
//	����:
//		x1			X ��ʼλ��
//		y1			Y ��ʼλ��
//		x2			X ����λ��
//		y2			Y ����λ��
//		rop			��դ����������GDI_ROPE_XXX�Ķ���
//
//	return			none
//*****************************************************************************
void ga_draw_rec(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t rop) 
{
	ga_draw_line(x0, y0, x1, y0, rop);
	ga_draw_line(x0, y1, x1, y1, rop);
	ga_draw_line(x0, y0, x0, y1, rop);
	ga_draw_line(x1, y0, x1, y1, rop);
}
//*****************************************************************************
//	��Bresenham�㷨���Ƶ�����Բ�� �Ľ��㷨
//
//	�������ȡ���Ȿ�飺
//	����ʽ�����ͼ��ѧ���Զ����µ�OpenGL������Edward Angel
//
//	����:
//		x			  Բ��Xλ��
//		y 			Բ��Yλ�á�
//    r       �뾶��
//		rop			��դ����������GDI_ROPE_XXX�Ķ���
//
//	return			none
//*****************************************************************************
void ga_draw_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t rop) 
{
	int p,dx,dy;
	p=3-2*r;
	dx=0;
	dy=r;
	for(;dx<=dy;)
	{
		ga_draw_point(dx+x,dy+y,rop);
		ga_draw_point(-dx+x,-dy+y,rop);
		ga_draw_point(-dx+x,dy+y,rop);
		ga_draw_point(dx+x,-dy+y,rop);
		ga_draw_point(dy+x,dx+y,rop);
		ga_draw_point(-dy+x,dx+y,rop);
		ga_draw_point(-dy+x,-dx+y,rop);
		ga_draw_point(dy+x,-dx+y,rop);
		if(p<0)
		{
			p=p+4*dx+6;
		}
		else 
		{
			dy--;p=p+4*(dx-dy)+10;
		}
		dx++;
	}
}
//*****************************************************************************
//	�����귨���Ƽ��ػ�
//
//
//	����:
//		x			  Բ��Xλ��
//		y 			Բ��Yλ�á�
//    r       �뾶��
//    w       ����
//    ti      �ƶ�һ�ε��ٶ��ٶ� ����
//
//	return			none
//*****************************************************************************
void ga_draw_ring(uint16_t x,uint16_t y,uint16_t r,uint16_t w,uint16_t ti) 
{
	
	uint16_t x0,y0,x1,y1;
	int16_t th; 
	float t;

	
	for(th = 0;th < 1270;th++)
	{
		t = (th / 100.0);
		x0 = x + r*cos(t);
		y0 = y + r*sin(t);
		x1 = x + (r-w)*cos(t);
		y1 = y + (r-w)*sin(t);
		ga_draw_line(x0,y0,x1,y1,GDI_ROP_COPY);
		
		t = (th/ 100.0)/2;
		x0 = x + r*cos(t);
		y0 = y + r*sin(t);
		x1 = x + (r-w)*cos(t);
		y1 = y + (r-w)*sin(t);
		ga_draw_line(x0,y0,x1,y1,GDI_ROP_DEL);
		ga_draw_line(x0+1,y0+1,x1+1,y1+1,GDI_ROP_DEL);
		delay_ms(ti); 
	}
	ga_draw_line(x+r-w,y-1,x+r-1,y-1,GDI_ROP_COPY);
	for(th = 2;th < 1270;th++)
	{
		t = (th / 100.0);
		x0 = x + r*cos(t);
		y0 = y + r*sin(t);
		x1 = x + (r-w)*cos(t);
		y1 = y + (r-w)*sin(t);
		ga_draw_line(x0,y0,x1,y1,GDI_ROP_DEL);
		ga_draw_line(x0+1,y0,x1+1,y1,GDI_ROP_DEL);
		ga_draw_line(x0-1,y0,x1-1,y1,GDI_ROP_DEL);
		ga_draw_line(x0,y0+1,x1,y1+1,GDI_ROP_DEL);
		ga_draw_line(x0,y0-1,x1,y1-1,GDI_ROP_DEL);

		t = (th/ 100.0)/2;
		x0 = x + r*cos(t);
		y0 = y + r*sin(t);
		x1 = x + (r-w)*cos(t);
		y1 = y + (r-w)*sin(t);
		ga_draw_line(x0,y0,x1,y1,GDI_ROP_COPY);
		delay_ms(ti);
	}
}
//*****************************************************************************
//	���㷽ʽ����������
//  ��Bresenhamֱ���㷨Ϊ����
//
//	����:
//		x0			  p0x����
//		y0 			  p0y����
//    x1        p1x����
//		y1        p1y����
//    x2        p2x����
//    y2        p2y����
//	return			none
//*****************************************************************************
void ga_draw_triangle(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t rop) 
{
	ga_draw_line(x0, y0, x1, y1, rop);
	ga_draw_line(x1, y1, x2, y2, rop);
	ga_draw_line(x2, y2, x0, y0, rop);
}
//*****************************************************************************
//	����ȫ�������̬������
//
//	return			none
//*****************************************************************************
#define POINT_NUM 10
void ga_draw_rand_triangle(void) 
{
	static uint16_t  x[POINT_NUM],y[POINT_NUM];
	static uint16_t  dx[POINT_NUM],dy[POINT_NUM];
	static _Bool flag = 0;
	uint16_t  num;
	 
	if(flag == 0)
	{
		flag = 1;
		for(num = 0; num < POINT_NUM;num++)
		{
			x[num] = 0;
			y[num] = 0;
			dx[num] = rand()%720;
			dy[num] = rand()%580;
		}
	}
	for(num = 0;num < POINT_NUM;num++)
	{
		if(dx[num] == x[num])
		{
			dx[num] = rand()%720;
		}
		else if (dx[num] > x[num]){x[num]++;}
		else{x[num]-- ;}
		
		if(dy[num] == y[num])
		{
			dy[num] = rand()%580;
		}
		else if (dy[num] > y[num]){y[num]++;}
		else{y[num]--;}
	}
	
	for(num = 0;num < (POINT_NUM -3);num++)
	{
		ga_draw_triangle(x[num],y[num],x[num+1],y[num+1],x[num+2],y[num+2],GDI_ROP_COPY);
	}
	delay_ms(70);
	for(num = 0;num < (POINT_NUM -3);num++)
	{
		ga_draw_triangle(x[num],y[num],x[num+1],y[num+1],x[num+2],y[num+2],GDI_ROP_DEL);
	}
}
//*****************************************************************************
//  ���ƾ�̬������Ʈ��
//
//	����:
//		xs			  x���경��������
//		y0 			  y����ʼ����
//    yp        y���𵴷���
//    rop       ��դ����
//	return			none
//*****************************************************************************
void ga_draw_ribbon(uint16_t xs,uint16_t y0,uint16_t yp,uint16_t rop) 
{
	uint16_t x[80],y[80];
	uint16_t num;

	for(num = 0;num < 80;num++)
	{
		x[num] = 0;
		y[num] = 0;
	}
	x[0] = 0;
	y[0] = y0;
	x[1] = 0;
	y[1] = y0+yp;
	num = 1;
	while(1)
	{
		num++;
		do
		{
			x[num] = x[num-1] + rand()%xs + 5;
		}
		while((x[num]-x[num-1]) < xs);
		if(x[num] > 800)return;
		do
		{
				y[num] = rand()%yp + y0 + 5;
		}
		while((y[num] - y[num-1]) > yp);
		ga_draw_triangle(x[num-2],y[num-2],x[num-1],y[num-1],x[num],y[num],GDI_ROP_COPY);
	}
}
//*****************************************************************************
//	λͼ��ת��ʾ
//	��תԲ�Ĳ���λͼ���ĵ�
//
//  ��ת��ʽ��
//
//  x0 = r*cos(g)
//  y0 = r*sin(g)
//
//  x = r*cos(g+th) = r*cos(g)*cos(th) - r*sin(g)sin(th) = x0*cos(th) - y0*sin(th)
//  y = r*sin(g+th) = r*sin(g)*cos(th) + r*cos(g)sin(th) = x0*sin(th) + y0*cos(th)
//
//  - -   -                   -  -  -
// | x | | cos(th) -sin(th) -a || x0 |
// | y |=| sin(th) cos(th)  -b || y0 |
// | 1 | | 0       0         1 || 1  |
//  - -   -                   -  -  -
//
//  -  -   -     -  - -
// | xp | | 1 0 a || x |
// | yp |=| 0 1 b || y |
// | 1  | | 0 0 1 || 1 |
//  -  -   -     -  - -
//
//	����:
//		x			    ͼƬԲ��x��ƫ��
//		y 			  ͼƬԲ��y��ƫ��
//    h  				ͼƬ�߶�      	
//		w         ͼƬ���
//    th        ��ת�Ƕ�(0rad-6.28rad * 100)
//    p         ָ��ͼ��������׵�ַ
//
//    rop       ��դ�������μ��궨����ע�ͣ�
//
//	return			none
//*****************************************************************************
/*������ת��ʾ��̬��ʾ*/
void ga_draw_bmp_rotate(uint32_t x,uint32_t y,uint16_t h,uint16_t w,uint16_t th,uint8_t * p,uint16_t rop) 
{
	
	/*��תԲ������ = ��ʼ���� + ��(��) / 2*/
	uint16_t xo = w / 2;
	uint16_t yo = h / 2;
	
	/*��λ�õ���ʱ����*/
	int32_t xt;
	int32_t yt;
	
	/*��ǰʹ������*/
	int32_t dx;
	int32_t dy;
	
	/*ʣ��ĸߺͿ�*/
	uint16_t wn;
	uint16_t hn;
	
	/*���ؼ����־λ*/
	_Bool m;
	
	/*����λ*/
	uint8_t har = w/8;
	
	/*�ֽ��е�λ*/
	uint8_t bit;
	
	/*��ǰ��ת���� ͨ��������ʽ��������ٶ�*/
	int16_t t[4]; 
	
	/*����ֵ*/
	t[0] =  cos(th/100.0)*255;
	t[1] = -sin(th/100.0)*255;
	t[2] =  sin(th/100.0)*255;
	t[3] =  cos(th/100.0)*255;
	
	/*ͳһ��λ Ϊ����������׼��*/
	x = x<<8;
	y = y<<8;
	
	/*����λͼ*/
	for(hn = 0;hn < h;hn++)
	{
		for(wn = 0;wn < w;wn++)
		{
			/*�����ֽ��ڵ�λƫ��*/
			bit = wn & 0x07;
			/*��õ�ǰ����ļ���ֵ*/
			m = (*(p + wn/8 + hn * har) & (0x80 >> bit))<<bit;
			/*ƽ������ϵ ��Բ���ƶ���ԭ��*/
			xt = wn - xo;
			yt = hn - yo;
			
			/*��ת����任*/
			dx = xt*t[0] + yt*t[1];
			dy = xt*t[2] + yt*t[3];
			/*�ָ�����ϵ*/
			dx = (dx + x)>>8;
			dy = (dy + y)>>8;
			if(m)
			{
				/*ʹ��ͼ����ʾ�����Ӧλ*/
				ga_draw_point(dx,dy,rop);
			}
			else
			{
				/*ʧ��ͼ����ʾ�����Ӧλ*/
				ga_draw_point(dx,dy,GDI_ROP_DEL);
			}
			
		}
	} 
}
//*****************************************************************************
//  ���������������ͼ
//
//	����:       void.
//	return			none.
//*****************************************************************************
#define POINT_N 100
void ga_draw_worldline(void) 
{
	/*·����֧��Ŀ���*/
	uint16_t xe[POINT_N];
	uint16_t ye[POINT_N];
	
	/*·����֧������*/
	uint16_t x[POINT_N];
	uint16_t y[POINT_N];
	
	
	/*��ǰ��֧��*/
	uint16_t pn;
	
	/*xy�����������*/
	uint8_t xs = 20,ys = 30;
	
	uint16_t num,temp;
	_Bool flag = 0;
	
	pn = 20;
	
	for(num = 0;num<pn;num++)
	{
		y[num]  = num*20 + 100;
		xe[num] = rand()%xs;
		ye[num] = rand()%ys + y[num];
	}
	while(1)
	{
		/*������һ�з���Ҫ������Ŀ���*/
		for(num = 0;num < pn;num++)
		{
			xe[num] += rand()%xs+40;
			if(x[num] > 730)
			{
				/*������Ļ��һ�˺��������*/
				return;
			}
			if(rand()%2)
			{
				if(rand()%2)
				{
					ye[num] += rand()%xs;
				}
				else
				{
					ye[num] -= rand()%xs;
				}
			}
			else
			{
				if(rand()%2)
				{
					ye[num] -=  rand()%ys;
				}
				else
				{
					ye[num] +=  rand()%ys;
				}
			}
		}
		/*ʧ�ܱ�־λ*/
		flag = 0;
		/*��ʼĿ��·���滮��ȫ��·��ͬʱ*/
		for(num = 0;flag == 0;num++)
		{
			if(num == pn)
			{
				num = 0;
			}
			if(x[num] < xe[num])
			{
				x[num]++;
			}
			else if(x[num] > xe[num])
			{
				x[num]--;
			}
			if(y[num] < ye[num])
			{
				y[num]++;
			}
			else if(y[num] > ye[num])
			{
				y[num]--;
			}
			for(temp = 0;temp < pn;temp++)
			{
				flag = 0;
				if((x[num]==xe[num])&&(y[num]==ye[num]))
				{
					flag = 1;
					temp = pn;
				}
			}
			ga_draw_point(x[num],y[num],GDI_ROP_COPY);
			delay_us(500);
		}
	}	
}
//*****************************************************************************
//  ����sin wave
//
//	����:       void.
//	return			none.
//*****************************************************************************
void sin_wave(void)
{
	static uint16_t x,y,num;
	ga_draw_circle(200,200,50,GDI_ROP_COPY);
	for(num = 0;num < 12;num++)
	{
		for(x = 200;x < 500;x++)
		{
			y =  (x-200)/5*(sin((x-200)/10.0-num))+200;
			ga_draw_point(x,y,GDI_ROP_COPY);
		}
		delay_ms(50);
		for(x = 200;x < 500;x++)
		{
			y =  (x-200)/5*(sin((x-200)/10.0-num))+200;
			ga_draw_point(x,y,GDI_ROP_XOR);
		}
	}
}
//*****************************************************************************
//  ����sigmoid
//
//	����:       void.
//	return			none.
//*****************************************************************************
void sigmoid(void)
{
	uint16_t x,y;
	uint16_t xh,yh;
	uint16_t num;
	_Bool flag;
	xh = 0;
	yh = 335 - 300*1/(1+exp(-(x/72.6-5)));
	for(x = 0;x < 726;x++)
	{
		y = 335 - 315*1/(1+exp(-(x/72.6-5)));
		ga_draw_line(xh+2,yh,x+2,y,GDI_ROP_COPY);
		ga_draw_line(xh+2,yh-1,x+2,y-1,GDI_ROP_COPY);
		yh = y;
		xh = x;
		delay_ms(5);
	}
}
//*****************************************************************************
//  ������̬�ֲ�
//
//	����:       void.
//	return			none.
//*****************************************************************************
void N_distributio(void)
{
	uint16_t num,p;
	int16_t x,y;
	uint16_t yt = 335;
	_Bool flag = 0;
	float u,b2;
	u =  5;
	b2 = 2;
/*��̬�ֲ�����*/
	for(x= 0;x < 726;x++)
	{
		y = 335 - 600.0*exp(-(x/72.6-u)*(x/72.6-u)/(2*b2))/sqrt(2*b2*3.141592);
		ga_draw_point(x,y,GDI_ROP_COPY);
		ga_draw_point(x,y+1,GDI_ROP_COPY);
	}
	/*��̬�ֲ���״ͼ*/
	for(num = 0;num < 60;num++)
	{
		/*��̬�ֲ���״ͼ�����������*/
		x = num *12 + 6;
		y = 335 - 600.0 * 
		    exp(-(x/72.6-u)*(x/72.6-u) /
		    (2*b2))/sqrt(2*b2*3.141592);
		/*��״ͼ�Ļ���*/
		while(!flag)
		{
			ga_draw_line(x+1,yt,x+10,yt,GDI_ROP_XOR);
			if(yt==y)
			{
				flag = 1;
			}
			delay_ms((uint16_t)(50/(50*(yt-y)))+1);
			yt--;
		}
		yt = 335;
		flag = 0;
	}
}



