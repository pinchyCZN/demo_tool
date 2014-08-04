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
			case TTIME:
				{
					BUTTON *b=list->control.data;
					static char *str="";
					switch(list->type){
						case TLIGHT:str="LIGHT";break;
						case TCUBE:str="CUBE";break;
						case TMULTIPLY:str="MULTIPLY";break;
						case TTRANSFORM:str="TRANSFORM";break;
						case TTIME:str="TIME";break;
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
		case CCHECKBOX:
			{
				CHECKBOX *cb=pc->control.data;
				if(cb){
					draw_checkbox(sc,cb);
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
					if(pu->highlighted>0){
						int height=pu->h/count;
						draw_rect(sc,pu->x,pu->y+height*(pu->highlighted-1),pu->w,height,RED);
					}
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
					int count=0;
					draw_rect(sc,s->x,s->y,s->w,s->h,0x44);
					while(skc){
						int x,y,w,h;
						x=skc->x;
						y=skc->y;
						w=skc->w;
						h=skc->h;

						if((x+w>=s->x) && (y+h>=s->y)
							&& (x<s->x+s->w) && (y<s->y+s->h)){
							int color=0x225544;
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
							if(skc->selected)
								color=0x7f7f7f;
							draw_rect(sc,x,y,w,h,color);
						}
						if(skc->selected){
							char str[40];
							SPLINE_KEY *key=skc->key;
							if(key){
								_snprintf(str,sizeof(str),"(%i)%1.2f,%1.2f",count,key->val,key->time);
								draw_string(sc,x,y+h,str,0xFFFFFF);
							}
						}
						count++;
						skc=skc->next;
					}
					draw_spline(sc,s);
				}
			}
			break;
		}
		pc=pc->next;
	}
	end_scroll_adjust(sc,rect,si);
	return 0;
}
#include <math.h>
struct CubicPoly
{
	float c0, c1, c2, c3;
};
float eval(struct CubicPoly *c,float t)
{
	float t2 = t*t;
	float t3 = t2 * t;
	return c->c0 + c->c1*t + c->c2*t2 + c->c3*t3;
}

void InitCubicPoly(float x0, float x1, float t0, float t1, struct CubicPoly *p)
{
	p->c0 = x0;
	p->c1 = t0;
	p->c2 = -3*x0 + 3*x1 - 2*t0 - t1;
	p->c3 = 2*x0 - 2*x1 + t0 + t1;
}
void init_catmull(float x0,float x1,float x2,float x3,float dt0,float dt1,float dt2,struct CubicPoly *p)
{
	float t1,t2;
	t1= (x1 - x0) / dt0 - (x2 - x0) / (dt0 + dt1) + (x2 - x1) / dt1;
	t2= (x2 - x1) / dt1 - (x3 - x1) / (dt1 + dt2) + (x3 - x2) / dt2;
	t1*=dt1;
	t2*=dt1;
	InitCubicPoly(x1,x2,t1,t2,p);

}
float dist_squared(float *p1,float *p2)
{
	float dx,dy;
	dx=p2[0]-p1[0];
	dy=p2[1]-p2[1];
	return dx*dx+dy*dy;
}
int init_centr_cr(float *points,struct CubicPoly *px,struct CubicPoly *py)
{
	float dt0,dt1,dt2;
	float *p0,*p1,*p2,*p3;
	p0=points;
	p1=points+2;
	p2=points+4;
	p3=points+6;
	dt0=pow(dist_squared(p0,p1),0.25);
	dt1=pow(dist_squared(p1,p2),0.25);
	dt2=pow(dist_squared(p2,p3),0.25);
    if (dt1 < 1e-4f)
		dt1 = 1.0f;
    if (dt0 < 1e-4f)
		dt0 = dt1;
    if (dt2 < 1e-4f)
		dt2 = dt1;
	init_catmull(p0[0],p1[0],p2[0],p3[0],dt0,dt1,dt2,px);
	init_catmull(p0[1],p1[1],p2[1],p3[1],dt0,dt1,dt2,py);

}
int draw_spline(SCREEN *sc,SPLINE_CONTROL *s)
{
	if(sc && s){
		int i;
		for(i=0;i<s->count;i++){
			ANIMATE_DATA *an=&s->anim[i];
			SPLINE_KEY *klist=an->key;
			int index=0,last=0;
			float points[4*2]={0};
			float tmp[2]={0};
			int color=0xFF0000;
			if(i!=s->selected)
				color=0x7F0000;

			while(klist){
				int x,y;
				x=s->x;
				y=s->y;
				draw_rect(sc,1+x+(int)klist->time,1+y+(int)klist->val+(s->h/2),2,2,0xFF00);
				tmp[0]=klist->time;
				tmp[1]=klist->val;
				klist=klist->next;
dolast:
				if(index==0){
					points[0]=-1000;
					points[1]=0;
					points[2]=0;
					points[3]=0;
					points[4]=tmp[0];
					points[5]=tmp[1];
					if(klist){
						points[6]=klist->time;
						points[7]=klist->val;
					}else{
						points[6]=tmp[0]+1000;
						points[7]=0;
					}
				}
				else{
					points[0]=points[2];
					points[1]=points[3];
					points[2]=points[4];
					points[3]=points[5];
					points[4]=points[6];
					points[5]=points[7];
					points[6]=tmp[0];
					points[7]=tmp[1];
					if(klist){
						points[6]=klist->time;
						points[7]=klist->val;
					}else{
						points[6]=tmp[0]+1000;
						points[7]=0;
					}
				}
				{
					struct CubicPoly px,py;
					int i;
					init_centr_cr(points,&px,&py);
					for(i=0;i<100;i++){
						x=eval(&px,0.01*(float)i);
						y=eval(&py,0.01*(float)i);
						
						draw_rect(sc,x+s->x,y+s->y+(s->h/2),2,2,color);
					}
				}
				index++;
				if(klist==0 && last==0){
					last=1;
					tmp[0]=tmp[0]+1000;
					tmp[1]=0;
					goto dolast;
				}
			}
		}
	}
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
	if(sc==0 || sc->buffer==0)
		return 0;
	buffer=sc->buffer;
	w=sc->w;
	h=sc->h;
	memset(buffer,0x10,w*h*4);
	if(pc!=0){
		GetWindowRect(hwnd,&rect);
		build_params(sc,&param_list.si,pc,&rect);
	}
	return TRUE;
}

int draw_subparams(HWND hwnd,SCREEN *sc)
{
	RECT rect={0};
	int *buffer,w,h;
	PARAM_CONTROL *pc=subparam_list.list;
	if(sc==0 || sc->buffer==0){

		memset(buffer,0x10,w*h*4);
		return 0;
	}
	buffer=sc->buffer;
	w=sc->w;
	h=sc->h;
	memset(buffer,0x10,w*h*4);
	if(pc!=0){
		GetWindowRect(hwnd,&rect);
		build_params(sc,&subparam_list.si,pc,&rect);
	}
	return TRUE;
}

