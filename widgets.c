#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
/*
B=COLOR_3DDKSHADOW
2=COLOR_3DLIGHT
1=COLOR_BTNHIGHLIGHT
3=COLOR_BTNSHADOW
F=COLOR_BTNFACE
0=COLOR_WINDOWFRAME
*/
/* button
	0000000000000000000000000
	01111111111111111111111B0
	01222222222222222222223B0
	012FFFFFFFFFFFFFFFFFFF3B0
	012FFFFFFFFFFFFFFFFFFF3B0
	01333333333333333333333B0
	0BBBBBBBBBBBBBBBBBBBBBBB0
	0000000000000000000000000
  button pressed
	0000000000000000000000000
	0333333333333333333333330
	03FFFFFFFFFFFFFFFFFFFFF30
	03FFFFFFFFFFFFFFFFFFFFF30
	03FFFFFFFFFFFFFFFFFFFFF30
	03FFFFFFFFFFFFFFFFFFFFF30
	0333333333333333333333330
	0000000000000000000000000

*/
#define _3DDKSHADOW 0x00000000
#define _3DLIGHT 0x00303030	
#define _BTNHIGHLIGHT 0x003838FF
#define _BTNSHADOW 0x00000048
#define _BTNFACE 0x00000070
#define _WINDOWFRAME 0x00000000

int draw_line_h(int *buffer,int bw,int bh,int x,int y,int len,int color)
{
	int i;
	int offset=x+((bh-y)*bw);
	for(i=0;i<len;i++){
		if((x+i)<bw)
			buffer[offset+i]=color;
	}
}
int draw_line_v(int *buffer,int bw,int bh,int x,int y,int len,int color)
{
	int i;
	int offset=x+((bh-y)*bw);
	for(i=0;i<len;i++){
		if((y+i)<bh)
			buffer[offset-(i*bw)]=color;
	}
}
int draw_button(int *buffer,int bw,int bh,int x,int y,int w,int h,int color,int pressed)
{
	int i;
	draw_line_h(buffer,bw,bh,x,    y,    w,  _WINDOWFRAME);
	draw_line_h(buffer,bw,bh,x,    y+h-1,w,  _WINDOWFRAME);
	draw_line_v(buffer,bw,bh,x,    y+1,  h-1,_WINDOWFRAME);
	draw_line_v(buffer,bw,bh,x+w-1,y+1,  h-1,_WINDOWFRAME);

	draw_line_h(buffer,bw,bh,x+1,y+1,w-3,_BTNHIGHLIGHT);
	draw_line_v(buffer,bw,bh,x+1,y+1,h-3,_BTNHIGHLIGHT);

	draw_line_h(buffer,bw,bh,x+2,y+2,w-5,_3DLIGHT);
	draw_line_v(buffer,bw,bh,x+2,y+2,h-5,_3DLIGHT);

	for(i=0;i<h-6;i++)
		draw_line_h(buffer,bw,bh,x+3,y+3+i,w-6,_BTNFACE);

	draw_line_h(buffer,bw,bh,x+2,  y+h-3,w-4,_BTNSHADOW);
	draw_line_v(buffer,bw,bh,x+w-3,y+2,  h-4,_BTNSHADOW);

	draw_line_h(buffer,bw,bh,x+1,  y+h-2,w-2,_3DDKSHADOW);
	draw_line_v(buffer,bw,bh,x+w-2,y+1,  h-2,_3DDKSHADOW);

	return 0;

}