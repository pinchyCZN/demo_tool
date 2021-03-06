#include "widgets.h"
#include "types.h"

int draw_char(SCREEN *sc,int x,int y,char a,int color)
{
extern unsigned char vga737_bin[];
	int i,j;
	int *buffer,bufw,bufh;
	unsigned char *p=vga737_bin+a*12;
	buffer=sc->buffer;
	bufw=sc->w;
	bufh=sc->h;
	for(i=0;i<12;i++){
		for(j=0;j<8;j++){
			int offset;
			offset=(x+j+((bufh-(i+y)-1)*bufw));
			if((x+j)>=bufw)
				break;
			if((i+y)>=bufh)
				break;
			if(offset>=(bufw*bufh))
				break;
			if(offset<0)
				continue;
			if(p[i]&(1<<(7-j))){
				buffer[offset]=color;
			}else{
			}
		}
	}
	return a;
}
int draw_string(SCREEN *sc,int x,int y,char *str,int color)
{
	int i;
	int len;
	len=strlen(str);
	for(i=0;i<len;i++){
		draw_char(sc,x+(i*8),y,str[i],color);
	}
	return i;
}

/*
B=COLOR_3DDKSHADOW
2=COLOR_3DLIGHT
1=COLOR_BTNHIGHLIGHT
3=COLOR_BTNSHADOW
4=COLOR_WINDOW
5=COLOR_WINDOWTEXT
F=COLOR_BTNFACE
0=COLOR_WINDOWFRAME
*/
/* button (surrounded by COLOR_BTNFACE)
	0000000000000000000000000
	01111111111111111111111B0
	01222222222222222222223B0
	012FFFFFFFFFFFFFFFFFFF3B0
	012FFFFFFFFFFFFFFFFFFF3B0
	01333333333333333333333B0
	0BBBBBBBBBBBBBBBBBBBBBBB0
	0000000000000000000000000
  button pressed (surrounded by COLOR_BTNFACE)
	0000000000000000000000000
	0333333333333333333333330
	03FFFFFFFFFFFFFFFFFFFFF30
	03FFFFFFFFFFFFFFFFFFFFF30
	03FFFFFFFFFFFFFFFFFFFFF30
	03FFFFFFFFFFFFFFFFFFFFF30
	0333333333333333333333330
	0000000000000000000000000
*/
/* checkbox 13x13
	FFFFFFFFFFFFFFF
	F3333333333331F
	F3444444444421F
	F3444444445421F
	F3444444455421F
	F3445444555421F
	F3445545554421F
	F3445555544421F
	F3444555444421F
	F3444454444421F
	F3444444444421F
	F3444444444421F
	F3222222222221F
	F1111111111111F
	FFFFFFFFFFFFFFF

*/
#define _3DDKSHADOW 0x00000000
#define _3DLIGHT 0x00303030	
#define _BTNHIGHLIGHT 0x003838FF
#define _BTNSHADOW 0x00000048
#define _BTNFACE 0x00000070
#define _WINDOWFRAME 0x00000000
//#define _WINDOWFRAME 0x00FF0000

int draw_line_h(SCREEN *sc,int x,int y,int len,int color)
{
	int i;
	int *buffer,bw,bh,offset;
	buffer=sc->buffer;
	bw=sc->w;
	bh=sc->h;
	if(y<0 || y>=bh || x>=bw || (x+len)<0)
		return 0;
	offset=x+((bh-y-1)*bw);
	for(i=0;i<len;i++){
		if((x+i)<0)
			continue;
		if((x+i)<bw)
			buffer[offset+i]=color;
		else
			break;
	}
	return 0;
}
int draw_line_v(SCREEN *sc,int x,int y,int len,int color)
{
	int i;
	int *buffer,bw,bh,offset;
	buffer=sc->buffer;
	bw=sc->w;
	bh=sc->h;
	if(x<0 || x>=bw || y>=bh || (y+len)<0)
		return 0;
	offset=x+((bh-y-1)*bw);
	for(i=0;i<len;i++){
		if((y+i)<0)
			continue;
		if((y+i)<bh)
			buffer[offset-(i*bw)]=color;
		else
			break;
	}
	return 0;
}
int draw_diag_line(SCREEN *sc,int x,int y,int len,int dir,int color)
{
	int i;
	int *buffer,bw,bh,offset;
	buffer=sc->buffer;
	bw=sc->w;
	bh=sc->h;
	offset=x+((bh-y-1)*bw);
	for(i=0;i<len;i++){
		if((x+(i*dir))>=bw || (x+(i*dir))<0)
			continue;
		if((y+i)>=bh || (y+i)<0)
			continue;
		if((offset+(i*dir)-(i*bw))<0)
			continue;
		buffer[offset+(i*dir)-(i*bw)]=color;
	}
	return 0;
}
int draw_cursor(SCREEN *sc,int x,int y,int color)
{
	int h=DEFBUTTONH-4;
	draw_line_v(sc,x+2,y+2,h,color);
	draw_diag_line(sc,x+2,y+2,h/2,1,color);
	draw_diag_line(sc,x+2+(h/2),y+2+(h/2),h/2,-1,color);
	return 0;
}
int draw_checkbox(SCREEN *sc,CHECKBOX *cb)
{
	int x,y,w,h;
	x=cb->x;
	y=cb->y;
	w=cb->w;
	h=cb->h;
	draw_line_h(sc,x,    y,    w-1, _BTNSHADOW);
	draw_line_h(sc,x,    y+h-1,w,   _BTNHIGHLIGHT);
	draw_line_h(sc,x+1,  y+h-2,w-3, _3DLIGHT);
	draw_line_v(sc,x,    y+1,  h-2, _BTNSHADOW);
	draw_line_v(sc,x+w-1,y,    h-1, _BTNHIGHLIGHT);
	draw_line_v(sc,x+w-2,y+1,  h-2, _3DLIGHT);
	draw_rect(sc,x+1,y+1,w-3,  h-3, 0);
	if(cb->checked){
		int i;
		int centerx=(x+w/2)-3;
		int centery=(y+h/2)-3;
		int dir=1;
		for(i=0;i<7;i++){
			draw_line_v(sc,centerx+i,centery,3,GREEN);
			centery+=dir;
			if(i==1)
				dir=-1;
		}
	}
	if(cb->str[0]!=0){
		draw_string(sc,x+20,y,cb->str,WHITE);
	}
	return 0;

}
int draw_button(SCREEN *sc,BUTTON *button)
{
	int i;
	int x,y,w,h,pressed;
	x=button->x;
	y=button->y;
	w=button->w;
	h=button->h;
	pressed=button->pressed;
	draw_line_h(sc,x,    y,    w,  _WINDOWFRAME);
	draw_line_h(sc,x,    y+h-1,w,  _WINDOWFRAME);
	draw_line_v(sc,x,    y+1,  h-1,_WINDOWFRAME);
	draw_line_v(sc,x+w-1,y+1,  h-1,_WINDOWFRAME);

	if(pressed){
		draw_line_h(sc,x+1,y+1,  w-2,_BTNSHADOW);
		draw_line_h(sc,x+1,y+h-2,w-2,_BTNSHADOW);

		draw_line_v(sc,x+1,  y+2,h-4,_BTNSHADOW);
		draw_line_v(sc,x+w-2,y+2,h-4,_BTNSHADOW);
		for(i=0;i<h-4;i++)
			draw_line_h(sc,x+2,y+2+i,w-4,_BTNFACE);
	}
	else{
		draw_line_h(sc,x+1,y+1,w-3,_BTNHIGHLIGHT);
		draw_line_v(sc,x+1,y+1,h-3,_BTNHIGHLIGHT);

		draw_line_h(sc,x+2,y+2,w-5,_3DLIGHT);
		draw_line_v(sc,x+2,y+2,h-5,_3DLIGHT);

		for(i=0;i<h-6;i++)
			draw_line_h(sc,x+3,y+3+i,w-6,_BTNFACE);

		draw_line_h(sc,x+2,  y+h-3,w-4,_BTNSHADOW);
		draw_line_v(sc,x+w-3,y+2,  h-4,_BTNSHADOW);

		draw_line_h(sc,x+1,  y+h-2,w-2,_3DDKSHADOW);
		draw_line_v(sc,x+w-2,y+1,  h-2,_3DDKSHADOW);
	}
	if(button->text && button->text[0]!=0){
		char *str=button->text;
		int len;
		int cx,cy;
		len=strlen(str);
		cx=x+(w/2)-((len*8)/2);
		cy=y+(h/2)-6;
		draw_string(sc,cx,cy,str,WHITE);
	}
	return 0;
}
int draw_rect(SCREEN *sc,int x,int y,int w,int h,int color)
{
	int i,j;
	int *buffer,bw,bh;
	buffer=sc->buffer;
	bw=sc->w;
	bh=sc->h;
	if(x>=bw || y>=bh || (x+w)<0 || (y+h)<0)
		return 0;
	for(i=0;i<h;i++){
		for(j=0;j<w;j++){
			int offset=(x+j)+((bh-(y+i)-1)*bw);
			if((x+j)>=bw)
				break;
			if((y+i)>=bh)
				break;
			if((y+i)<0)
				break;
			if((x+j)<0)
				break;
			if(offset<0)
				break;
			buffer[offset]=color;
		}
	}
	return 0;
}
/*
COLOR_BTNHIGHLIGHT scroll background
*/
int draw_vscroll(SCREEN *sc,SCROLLBAR *scroll)
{
	float ypos;
	int bheight;
	int x,y,w,h,pos,range;
	x=scroll->x;
	y=scroll->y;
	w=scroll->w;
	h=scroll->h;
	pos=scroll->pos;
	range=scroll->range;
	draw_rect(sc,x,y,w,h,_BTNHIGHLIGHT);
	bheight=h-(range/3);
	if(bheight<10){
		bheight=10;
		if(h<10)
			bheight=h;
	}
	ypos=(float)(h-bheight)*((float)pos/(float)(range));
	{
		BUTTON button={0};
		button.w=w;
		button.h=bheight;
		button.x=x;
		button.y=(int)ypos+y;
		button.pressed=scroll->pressed;
		draw_button(sc,&button);
	}
	return 0;
}
int draw_hscroll(SCREEN *sc,SCROLLBAR *scroll)
{
	float xpos;
	int bwidth;
	int x,y,w,h,pos,range;
	x=scroll->x;
	y=scroll->y;
	w=scroll->w;
	h=scroll->h;
	pos=scroll->pos;
	range=scroll->range;
	draw_rect(sc,x,y,w,h,_BTNHIGHLIGHT);
	bwidth=w-(range/3);
	if(bwidth<10){
		bwidth=10;
		if(w<10)
			bwidth=w;
	}
	xpos=(float)(w-bwidth)*((float)pos/(float)(range));
	{
		BUTTON button={0};
		button.w=bwidth;
		button.h=h;
		button.x=(int)xpos+x;
		button.y=y;
		button.pressed=scroll->pressed;
		draw_button(sc,&button);
	}
	return 0;
}