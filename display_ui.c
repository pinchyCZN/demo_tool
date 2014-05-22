#include <windows.h>
#include "widgets.h"



PAGE_LIST page_list={0};
PARAM_LIST param_list={0};
PARAM_LIST subparam_list={0};
SPLINE_EDIT spline_edit={0};

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
			case TLIGHT:
			case TMULTIPLY:
			case TCUBE:
			case TTRANSFORM:
				{
					BUTTON *b=list->control.data;
					static char *str="";
					switch(list->type){
						case TLIGHT:str="LIGHT";break;
						case TCUBE:str="CUBE";break;
						case TMULTIPLY:str="MULTIPLY";break;
						case TTRANSFORM:str="TRANSFORM";break;
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
					if(list->isroot)
						draw_rect(sc,b->x+4,b->y+b->h-10,5,5,0xFF00);
					if(list->error)
						draw_line_h(sc,b->x+2,b->y+1,b->w-5,0xFF0000);
					draw_line_v(sc,b->x+b->w-RESIZE_MARGIN,b->y,b->h,0);
				}
				break;
			case TRESIZE:
				{
					RESIZERECT *rr=list->control.data;
					if(rr){
						draw_rect(sc,rr->x,rr->y,12,12,rr->color);
						draw_char(sc,rr->x,rr->y,'R',0);
					}
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

int build_spline(SCREEN *sc,RECT *rect,int *xscroll,int *yscroll)
{

}

int build_param_edit_type(SCREEN *sc,int has_focus,int overwrite,int cursor,int changed,char *str,int x,int y,int w,int h,int *outheight)
{
	if(sc){
		int color=WHITE;
		draw_rect(sc,x,y,w,h,0x202020);
		if(has_focus){
			if(overwrite)
				draw_line_h(sc,x+(cursor*8),y+h-1,9,0x7F2020);
			else
				draw_line_v(sc,x+(cursor*8),y,h,0x7F2020);
		}
		if(changed)
			color=RED;
		draw_string(sc,x,y+(h/2)-6,str,color);
		if(outheight)
			*outheight=h;
	}
	return TRUE;
}
int get_droplist_height(DROPLIST *dl,int *h)
{
	if(dl && dl->dropped){
		int height,index=0,num_items=0;
		while(dl->list && dl->list[index]!=0){
			if(dl->list[index]=='\n')
				num_items++;
			index++;
		}
		if(h)
			*h=dl->h*num_items;
	}
	return TRUE;
}
int build_params(SCREEN *sc,PARAM_CONTROL *paramc,RECT *rect,int *xscroll,int *yscroll)
{
	PARAM_CONTROL *pc=paramc;
	while(pc){
		int height=0;
		switch(pc->control.type){
		case CRECT:
			{
				RECTANGLE *c=pc->control.data;
				if(c){
					int color=0;
					if(c->r && c->g && c->b)
						color=((*c->r)<<16)|((*c->g)<<8)|(*c->b);
					draw_rect(sc,c->x,c->y,c->w,c->h,color);
					height=c->h;
				}
			}
			break;
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
		case CEDITBYTE:
			{
				EDITBYTE *c=pc->control.data;
				if(c)
					build_param_edit_type(sc,pc->has_focus,c->overwrite,c->cursor,c->changed,c->str,c->x,c->y,c->w,c->h,&height);
			}
			break;
		case CEDITINT:
			{
				EDITINT *c=pc->control.data;
				if(c)
					build_param_edit_type(sc,pc->has_focus,c->overwrite,c->cursor,c->changed,c->str,c->x,c->y,c->w,c->h,&height);
			}
			break;
		case CEDITFLOAT:
			{
				EDITFLOAT *c=pc->control.data;
				if(c)
					build_param_edit_type(sc,pc->has_focus,c->overwrite,c->cursor,c->changed,c->str,c->x,c->y,c->w,c->h,&height);
			}
			break;
		case CBUTTON:
			{
				BUTTON *b=pc->control.data;
				if(b){
					draw_button(sc,b);
				}
			}
			break;
		case CDROPLIST:
			{
				DROPLIST *dl=pc->control.data;
				if(dl){
					draw_rect(sc,dl->x,dl->y,dl->w,dl->h,0x202020);
					if(dl->list){
						int i=0,index=0,xpos=0;
						while(index<dl->current){
							char c=dl->list[i++];
							if(c=='\n')
								index++;
							else if(c==0)
								break;
						}
						while(TRUE){
							char c=dl->list[i++];
							if(c=='\n' || c==0)
								break;
							else{
								draw_char(sc,dl->x+1+xpos,dl->y+(dl->h/2)-6,c,WHITE);
								xpos+=8;
							}
						}
					}

				}
			}
			break;
		case CPOPUPLIST:
			{
				POPUPLIST *pu=pc->control.data;
				if(pu){
					int i=0,xpos=0,ypos=0;
					int count=pu->count;
					if(count==0)
						count=1;
					draw_rect(sc,pu->x,pu->y,pu->w,pu->h,0x402020);
					while(pu->list){
						int done=FALSE;
						while(xpos<pu->w){
							char c=pu->list[i++];
							if(c==0 || c=='\n'){
								ypos+=pu->h/count;
								xpos=0;
								if(c==0)
									done=TRUE;
								break;
							}
							else{
								draw_char(sc,pu->x+1+xpos,pu->y+ypos+6,c,WHITE);
								xpos+=8;
							}
						}
						if(done)
							break;
					}
				}
			}
			break;
		}
		pc=pc->next;
	}
	{
		PARAM_LIST *p=&param_list;
		if(p){
			int rect_height=rect->bottom-rect->top;
			int rect_width=rect->right-rect->left;
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
		}
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
	if(spline_edit.count)
		build_spline(sc,&rect,&xscroll,&yscroll);
	else
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
	PARAM_CONTROL *pc=param_list.list;

	if(sc==0 || sc->buffer==0 || pc==0)
		return 0;
	buffer=sc->buffer;
	w=sc->w;
	h=sc->h;
	memset(buffer,0x8,w*h*4);
	GetWindowRect(hwnd,&rect);
	build_params(sc,pc,&rect,&xscroll,&yscroll);
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


int display_subparams(HWND hwnd,SCREEN *sc)
{
	HDC hdc;
	RECT rect={0};
	int *buffer,w,h,xscroll=0,yscroll=0;
	PARAM_CONTROL *pc=subparam_list.list;

	if(sc==0 || sc->buffer==0 || pc==0)
		return 0;
	buffer=sc->buffer;
	w=sc->w;
	h=sc->h;
	memset(buffer,0x8,w*h*4);
	GetWindowRect(hwnd,&rect);
	build_params(sc,pc,&rect,&xscroll,&yscroll);
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