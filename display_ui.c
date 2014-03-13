#include <windows.h>
#include "widgets.h"



PAGE_LIST page_list={0};

int init_page_list()
{
	int result=FALSE;
	PAGE_DATA *p;
	p=malloc(sizeof(PAGE_DATA));
	if(p){
		char *str;
		memset(p,0,sizeof(PAGE_DATA));
		str=malloc(sizeof("page1"));
		if(str){
			p->name=str;
			add_page(&page_list,&p);
			page_list.current=p;
			result=TRUE;
		}
		else
			free(p);
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
			case TCUBE:
				{
					BUTTON *b=list->control.data;
					static char *str="CUBE";
					if(list->name[0]==0)
						b->text=str;
					else
						b->text=list->name;
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
		scroll.w=SCROLL_WIDTH;
		scroll.h=rect_height;
		scroll.pos=p->vscroll;
		scroll.range=sc->h;
		scroll.x=p->hscroll+rect_width-SCROLL_WIDTH;
		scroll.y=p->vscroll;
		draw_vscroll(sc,&scroll);
	}

	return TRUE;
}

int show_page_list()
{
}

int show_params()
{
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
	memset(buffer,0x10,1024*1024*4);
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
int display_page_list(HWND hwnd,HGLRC hglrc)
{

}
int display_params(HWND hwnd,HGLRC hglrc)
{

}