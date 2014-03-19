#include <windows.h>
#include "widgets.h"



PAGE_LIST page_list={0};
PARAM_LIST param_list={0};

int init_page_list()
{
	int result=FALSE;
	PAGE_DATA *p;
	p=malloc(sizeof(PAGE_DATA));
	if(p){
		char *str;
		memset(p,0,sizeof(PAGE_DATA));
		_snprintf(p->name,sizeof(p->name),"default");
		p->name[sizeof(p->name)-1]=0;
		add_page(&page_list,&p);
		page_list.current=p;
		result=TRUE;
	}
	return result;
}
int build_page(SCREEN *sc,RECT *rect,int *xscroll,int *yscroll)
{
	int i;
	int rect_height,rect_width;
	PAGE_DATA *p;
	p=page_list.current;
	if(p==0)
		p=page_list.list;
	if(p==0)
		return FALSE;

	rect_height=rect->bottom-rect->top;
	rect_width=rect->right-rect->left;
	if(p->vscroll > (sc->h - rect_height))
		p->vscroll=(sc->h - rect_height);
	if(p->vscroll<0)
		p->vscroll=0;

	if(p->hscroll > (sc->w - rect_width))
		p->hscroll=(sc->w - rect_width);
	if(p->hscroll<0)
		p->hscroll=0;
	*xscroll=p->hscroll;
	*yscroll=p->vscroll;
	sc->clipxl=p->hscroll;
	sc->clipxr=p->hscroll+rect_width;
	sc->clipyt=p->vscroll;
	sc->clipyb=p->vscroll+rect_height;

	if(p->list){
		OP *list=p->list;
		while(list){
			switch(list->type){
			case TMULTIPLY:
			case TCUBE:
				{
					BUTTON *b=list->control.data;
					static char *str="";
					switch(list->type){
						case TCUBE:str="CUBE";break;
						case TMULTIPLY:str="MULTIPLY";break;
					}
					if(list->name[0]==0)
						b->text=str;
					else{
						char str[sizeof(list->name)];
						int len;
						len=_snprintf(str,sizeof(str),"\"%s\"",list->name);
						if(len<0)
							len=sizeof(str);
						if((len*8)>=b->w){
							len=(b->w)/8;
							if(len>sizeof(str))
								len=sizeof(str);
							if(len>0)
								str[len-1]=0;
						}
						b->text=str;
					}
					draw_button(sc,b);

					if(list->selected)
						draw_rect(sc,b->x+4,b->y+4,5,5,0xFF0000);
					if(list->activated)
						draw_rect(sc,b->x+4,b->y+b->h-10,5,5,0xFF00);
					if(list->error)
						draw_line_h(sc,b->x+2,b->y+1,b->w-5,0xFF0000);
				}
				break;
			case TDRAG:
				{
					CONTROLDRAG *d=list->control.data;
					if(d){
						OP *allops=p->list;
						while(allops){
							if(allops->type!=TDRAG){
								switch(allops->control.type){
								case CBUTTON:
									{
										BUTTON *b=allops->control.data;
										if(b && b->pressed){
											int xo=d->deltax;
											int yo=d->deltay;
											draw_line_h(sc,b->x+xo,       b->y+yo,       b->w,  d->color);
											draw_line_h(sc,b->x+xo,       b->y+b->h-1+yo,b->w,  d->color);
											draw_line_v(sc,b->x+xo,       b->y+1+yo,     b->h-2,d->color);
											draw_line_v(sc,b->x+b->w-1+xo,b->y+1+yo,     b->h-2,d->color);
										}
									}
									break;
								}
							}
							allops=allops->list_next;
						}
					}
				}
				break;
			}
			list=list->list_next;
		}
	}
	draw_cursor(sc,p->cursorx,p->cursory);
	if(rect_height<sc->h){
		SCROLLBAR scroll={0};
		int x;
		scroll.w=SCROLL_WIDTH;
		scroll.h=rect_height;
		scroll.pos=p->vscroll;
		scroll.range=sc->h-rect_height;
		x=p->hscroll+rect_width;
		if(x>=sc->w)
			x=sc->w;
		scroll.x=x-SCROLL_WIDTH;
		scroll.y=p->vscroll;
		scroll.pressed=p->vscroll_pressed;
		draw_vscroll(sc,&scroll);
	}
	if(rect_width<sc->w){
		SCROLLBAR scroll={0};
		int x,y;
		scroll.w=rect_width-SCROLL_WIDTH;
		scroll.h=SCROLL_WIDTH;
		scroll.pos=p->hscroll;
		scroll.range=sc->w-rect_width;
		x=p->hscroll;
		y=p->vscroll+rect_height;
		if(x>=sc->w)
			x=sc->w;
		if(y>=sc->h)
			y=sc->h;
		scroll.x=x;
		scroll.y=y-SCROLL_WIDTH;
		scroll.pressed=p->hscroll_pressed;
		draw_hscroll(sc,&scroll);
	}

	return TRUE;
}

int build_params(SCREEN *sc,RECT *rect,int *xscroll,int *yscroll)
{
	PARAM_CONTROL *pc=param_list.list;
	while(pc){
		int height=0;
		switch(pc->control.type){
		case CSTATIC:
			{
				STATICTEXT *c=pc->control.data;
				if(c){
					draw_string(sc,c->x,c->y+(c->h/2)-6,c->str,WHITE);
					height=c->h;
				}
			}
			break;
		case CEDIT:
			{
				EDITBOX *e=pc->control.data;
				if(e){
					int width=e->maxlen*8;
					draw_rect(sc,e->x,e->y,width,e->h,0x202020);
					if(pc->has_focus){
						if(e->overwrite)
							draw_line_h(sc,e->x+(e->cursor*8),e->y+e->h-1,9,0x7F2020);
						else
							draw_line_v(sc,e->x+(e->cursor*8),e->y,e->h,0x7F2020);
					}
					if(e->str)
						draw_string(sc,e->x+1,e->y+(e->h/2)-6,e->str,WHITE);
					height=e->h;
				}
			}
			break;
		case PC_3FLOATA:
			{
				C3FLOATA *c=pc->control.data;
				if(c){
					int ypos=c->h/2-6;
					int fwidth=(c->w-12)/3;
					int i;
					float f[3]={c->a,c->b,c->c};
					char *str[3]={c->numa,c->numb,c->numc};
					for(i=0;i<3;i++){
						int x=c->x+(i*fwidth);
						draw_rect(sc,x,c->y,fwidth-1,c->h,0x202020);
						draw_string(sc,x+4,c->y+ypos,str[i],WHITE);
					}
					draw_rect(sc,c->x+3*fwidth,c->y,16,c->h,0x202020);
					draw_string(sc,c->x+4+3*fwidth,c->y+ypos,"A",WHITE);
					height=c->h;
				}
			}
			break;
		}
		if(pc->name){
			int offset=0;
			if(height!=0)
				offset=(height/2)-6;
			draw_string(sc,pc->x,pc->y+offset,pc->name,WHITE);
		}
		pc=pc->next;
	}
	return 0;
}
int display_view1(HWND hwnd,HGLRC hglrc)
{
	HDC hdc;
	if(hwnd==0 || hglrc==0)
		return FALSE;
	hdc=GetDC(hwnd);
	if(hdc){
		wglMakeCurrent(hdc,hglrc);
		display();
		SwapBuffers(hdc);
	}
}
int display_page(HWND hwnd,SCREEN *sc)
{
	HDC hdc;
	RECT rect={0};
	int *buffer,w,h,xscroll=0,yscroll=0;
	if(sc==0 || sc->buffer==0)
		return 0;
	buffer=sc->buffer;
	w=sc->w;
	h=sc->h;
	memset(buffer,0x10,w*h*4);
	GetWindowRect(hwnd,&rect);
	build_page(sc,&rect,&xscroll,&yscroll);
	hdc=GetDC(hwnd);
	if(hdc){
		BITMAPINFO bmi;
		memset(&bmi,0,sizeof(BITMAPINFO));
		bmi.bmiHeader.biBitCount=32;
		bmi.bmiHeader.biWidth=w;
		bmi.bmiHeader.biHeight=h;
		bmi.bmiHeader.biPlanes=1;
		bmi.bmiHeader.biSize=40;
		SetDIBitsToDevice(hdc,-xscroll,-yscroll,w,h,0,0,0,w,buffer,&bmi,DIB_RGB_COLORS);
	}
	return 0;
}
int display_page_list(HWND hwnd,SCREEN *sc)
{
}



int display_params(HWND hwnd,SCREEN *sc)
{
	HDC hdc;
	RECT rect={0};
	int *buffer,w,h,xscroll=0,yscroll=0;
	if(sc==0 || sc->buffer==0)
		return 0;
	buffer=sc->buffer;
	w=sc->w;
	h=sc->h;
	memset(buffer,0x8,w*h*4);
	GetWindowRect(hwnd,&rect);
	build_params(sc,&rect,&xscroll,&yscroll);
	hdc=GetDC(hwnd);
	if(hdc){
		BITMAPINFO bmi;
		memset(&bmi,0,sizeof(BITMAPINFO));
		bmi.bmiHeader.biBitCount=32;
		bmi.bmiHeader.biWidth=w;
		bmi.bmiHeader.biHeight=h;
		bmi.bmiHeader.biPlanes=1;
		bmi.bmiHeader.biSize=40;
		SetDIBitsToDevice(hdc,-xscroll,-yscroll,w,h,0,0,0,w,buffer,&bmi,DIB_RGB_COLORS);
	}
	return 0;
}