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
		memset(p,0,sizeof(PAGE_DATA));
		_snprintf(p->name,sizeof(p->name),"default");
		p->name[sizeof(p->name)-1]=0;
		add_page(&page_list,&p);
		page_list.current=p;

		/**********testing!!****************/
		{
			OP *o=0;
			add_type_op(p,TTRANSFORM,0,0);
			find_op_type(p,&o,TTRANSFORM);
			if(o){
				PARAM_CONTROL *pc=0;
				create_op_params(o);
				find_param_type(&param_list,CBUTTON,&pc);
				if(pc){
					create_subparams(o,pc);
					pc=0;
					find_param_type(&subparam_list,CBUTTON,&pc);
					if(pc)
						handle_subparam_button(&subparam_list,pc);
				}
			}
		}
		/**************************************/
		result=TRUE;
	}
	return result;
}
int begin_scroll_adjust(SCREEN *sc,RECT *rect,SCROLL_INFO *si)
{
	int rect_height,rect_width;
	if(sc==0 || rect==0 || si==0)
		return FALSE;
	rect_height=rect->bottom-rect->top;
	rect_width=rect->right-rect->left;
	if(si->vscroll > (sc->h - rect_height))
		si->vscroll=(sc->h - rect_height);
	if(si->vscroll<0)
		si->vscroll=0;

	if(si->hscroll > (sc->w - rect_width))
		si->hscroll=(sc->w - rect_width);
	if(si->hscroll<0)
		si->hscroll=0;
	sc->clipxl=si->hscroll;
	sc->clipxr=si->hscroll+rect_width;
	sc->clipyt=si->vscroll;
	sc->clipyb=si->vscroll+rect_height;
	return TRUE;
}
int end_scroll_adjust(SCREEN *sc,RECT *rect,SCROLL_INFO *si)
{
	int rect_height,rect_width;
	if(sc==0 || rect==0 || si==0)
		return FALSE;
	rect_height=rect->bottom-rect->top;
	rect_width=rect->right-rect->left;
	if(rect_height<sc->h){
		SCROLLBAR scroll={0};
		int x;
		scroll.w=SCROLL_WIDTH;
		scroll.h=rect_height;
		scroll.pos=si->vscroll;
		scroll.range=sc->h-rect_height;
		x=si->hscroll+rect_width;
		if(x>=sc->w)
			x=sc->w;
		scroll.x=x-SCROLL_WIDTH;
		scroll.y=si->vscroll;
		scroll.pressed=si->vscroll_pressed;
		draw_vscroll(sc,&scroll);
	}
	if(rect_width<sc->w){
		SCROLLBAR scroll={0};
		int x,y;
		scroll.w=rect_width-SCROLL_WIDTH;
		scroll.h=SCROLL_WIDTH;
		scroll.pos=si->hscroll;
		scroll.range=sc->w-rect_width;
		x=si->hscroll;
		y=si->vscroll+rect_height;
		if(x>=sc->w)
			x=sc->w;
		if(y>=sc->h)
			y=sc->h;
		scroll.x=x;
		scroll.y=y-SCROLL_WIDTH;
		scroll.pressed=si->hscroll_pressed;
		draw_hscroll(sc,&scroll);
	}
	return TRUE;
}
int build_page(SCREEN *sc,RECT *rect)
{
	PAGE_DATA *p;
	p=page_list.current;
	if(p==0)
		p=page_list.list;
	if(p==0)
		return FALSE;

	begin_scroll_adjust(sc,rect,&p->si);
	sc->xscroll=p->si.hscroll;
	sc->yscroll=p->si.vscroll;

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
	end_scroll_adjust(sc,rect,&p->si);
	return TRUE;
}

int build_spline(SCREEN *sc,RECT *rect)
{
	if(spline_edit.plist.list){
		build_params(sc,&spline_edit.plist.si,spline_edit.plist.list,rect);
		//draw_string(sc,10,10,"324234243",WHITE);
	}
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
		int index=0,num_items=0;
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
int build_params(SCREEN *sc,SCROLL_INFO *si,PARAM_CONTROL *paramc,RECT *rect)
{
	PARAM_CONTROL *pc=paramc;
	begin_scroll_adjust(sc,rect,si);
	sc->xscroll=si->hscroll;
	sc->yscroll=si->vscroll;
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
		case CSPLINE:
			{
				SPLINE_CONTROL *s=pc->control.data;
				if(s){
					SPLINE_KEY_CONTROL *skc=s->keys;
					draw_rect(sc,s->x,s->y,s->w,s->h,0x44);
					while(skc){
						int x,y,w,h;
						x=skc->x;
						y=skc->y;
						w=skc->w;
						h=skc->h;

						if((x+w>=s->x) && (y+h>=s->y)
							&& (x<s->x+s->w) && (y<s->y+s->h)){
							if(x<s->x){
								x=s->x;
								w=w-(s->x-x);
							}
							else if(x+w>s->x+s->w){
								w=w-(x+w-(s->x+s->w));
							}
							if(y<s->y){
								y=s->y;
								w=w-(s->y-y);
							}
							else if(y+h>s->y+s->h){
								h=h-(y+h-(s->y+s->h));
							}
							draw_rect(sc,x,y,w,h,0x225544);
						}
						skc=skc->next;
					}
				}
			}
			break;
		}
		pc=pc->next;
	}
	end_scroll_adjust(sc,rect,si);
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
int display_screen(HWND hwnd,SCREEN *sc)
{
	HDC hdc;
	hdc=GetDC(hwnd);
	if(hdc && sc){
		BITMAPINFO bmi;
		int w,h;
		w=sc->w;
		h=sc->h;
		memset(&bmi,0,sizeof(BITMAPINFO));
		bmi.bmiHeader.biBitCount=32;
		bmi.bmiHeader.biWidth=w;
		bmi.bmiHeader.biHeight=h;
		bmi.bmiHeader.biPlanes=1;
		bmi.bmiHeader.biSize=40;
		SetDIBitsToDevice(hdc,-sc->xscroll,-sc->yscroll,w,h,0,0,0,w,sc->buffer,&bmi,DIB_RGB_COLORS);
	}
	return 0;
}
int draw_page(HWND hwnd,SCREEN *sc)
{
	RECT rect={0};
	int *buffer,w,h;
	if(sc==0 || sc->buffer==0)
		return 0;
	buffer=sc->buffer;
	w=sc->w;
	h=sc->h;
	memset(buffer,0x10,w*h*4);
	GetWindowRect(hwnd,&rect);
	if(spline_edit.plist.list)
		build_spline(sc,&rect);
	else
		build_page(sc,&rect);
	return TRUE;
}

int draw_params(HWND hwnd,SCREEN *sc)
{
	RECT rect={0};
	int *buffer,w,h;
	PARAM_CONTROL *pc=param_list.list;
	if(sc==0 || sc->buffer==0 || pc==0)
		return 0;
	buffer=sc->buffer;
	w=sc->w;
	h=sc->h;
	memset(buffer,0x10,w*h*4);
	GetWindowRect(hwnd,&rect);
	build_params(sc,&param_list.si,pc,&rect);
	return TRUE;
}

int draw_subparams(HWND hwnd,SCREEN *sc)
{
	RECT rect={0};
	int *buffer,w,h;
	PARAM_CONTROL *pc=subparam_list.list;
	if(sc==0 || sc->buffer==0 || pc==0)
		return 0;
	buffer=sc->buffer;
	w=sc->w;
	h=sc->h;
	memset(buffer,0x10,w*h*4);
	GetWindowRect(hwnd,&rect);
	build_params(sc,&subparam_list.si,pc,&rect);
	return TRUE;
}

