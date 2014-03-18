#define _WIN32_WINNT 0x400
#include <windows.h>
#include "widgets.h"

int add_op(PAGE_DATA *pd,OP *op)
{
	int result=FALSE;
	if(pd==0 || op==0)
		return result;
	if(pd->list==0){
		pd->list=op;
		result=TRUE;
	}
	else{
		OP *o=pd->list;
		while(o->list_next!=0){
			o=o->list_next;
		}
		o->list_next=op;
		op->list_prev=o;
		result=TRUE;
	}
}
int del_op(PAGE_DATA *pd,OP *op)
{
	int result=FALSE;
	if(pd==0 || op==0)
		return result;
	{
		OP *prev,*next;
		prev=op->list_prev;
		next=op->list_next;
		if(op->control.data)
			free(op->control.data);
		if(op->data)
			free(op->data);
		if(prev)
			prev->list_next=next;
		if(next)
			next->list_prev=prev;
		free(op);
		result=TRUE;
	}
	return result;
}
int add_page(PAGE_LIST *plist,PAGE_DATA *pd)
{
	int result=FALSE;
	if(plist==0 || pd==0)
		return result;
	if(plist->list==0){
		plist->list=pd;
		result=TRUE;
	}else{
		PAGE_DATA *p=plist->list;
		while(p->next!=0){
			p=p->next;
		}
		if(p){
			p->next=pd;
			pd->prev=p;
			result=TRUE;
		}
	}
	return result;
}
int del_page(PAGE_LIST *pl,PAGE_DATA *pd)
{
	int result=FALSE;
	if(pl==0 || pd==0)
		return result;
	if(pd->prev==0){
		PAGE_DATA *n=pd->next;
		pl->list=n;
		n->prev=0;
		if(pd->name)
			free(pd->name);
		free(pd);
		result=TRUE;
	}else{
		PAGE_DATA *n,*p;
		n=pd->next;
		p=pd->prev;
		if(n)
			n->prev=p;
		if(p)
			p->next=n;
		result=TRUE;
	}
	return result;
}
int create_op(int type,OP *op,int x,int y)
{
	int result=FALSE;
	if(op==0)
		return result;
	switch(type){
	case TCUBE:
		{
			BUTTON *button;
			CUBE_DATA *cube;
			button=malloc(sizeof(BUTTON));
			cube=malloc(sizeof(CUBE_DATA));
			if(button && cube){
				memset(button,0,sizeof(BUTTON));
				memset(cube,0,sizeof(CUBE_DATA));
				button->w=DEFBUTTONW;
				button->h=DEFBUTTONH;
				button->x=x;
				button->y=y;
				op->control.type=CBUTTON;
				op->control.data=button;
				op->type=type;
				op->data=cube;
				result=TRUE;
			}
			else{
				if(button)
					free(button);
				if(cube)
					free(cube);
			}
		}
		break;
	case TMULTIPLY:
		{
			BUTTON *button;
			MULTIPLY_DATA *data;
			button=malloc(sizeof(BUTTON));
			data=malloc(sizeof(MULTIPLY_DATA));
			if(button && data){
				memset(button,0,sizeof(BUTTON));
				memset(data,0,sizeof(MULTIPLY_DATA));
				button->w=DEFBUTTONW;
				button->h=DEFBUTTONH;
				button->x=x;
				button->y=y;
				op->control.type=CBUTTON;
				op->control.data=button;
				op->type=type;
				op->data=data;
				result=TRUE;
			}
			else{
				if(button)
					free(button);
				if(data)
					free(data);
			}

		}
		break;
	case TDRAG:
		{
			CONTROLDRAG *drag;
			drag=malloc(sizeof(CONTROLDRAG));
			if(drag){
				memset(drag,0,sizeof(CONTROLDRAG));
				op->control.type=CDRAG;
				op->control.data=drag;
				op->type=type;
				result=TRUE;
			}
		}
		break;
	}
	return result;
}
int add_type_op(PAGE_DATA *p,int type,int x,int y)
{
	int result=FALSE;
	OP *op;
	op=malloc(sizeof(OP));
	if(op){
		memset(op,0,sizeof(OP));
		if(create_op(type,op,x+p->hscroll,y+p->vscroll)){
			if(add_op(p,op))
				result=TRUE;
		}
		if(!result)
			free(op);
	}
	return result;
}
int get_nearest_grid(SCREEN *sc,int *x,int *y)
{
	int tx,ty;
	tx=*x;
	if(tx>=sc->w)
		tx=sc->w-1;
	if(tx<0)
		tx=0;
	ty=*y;
	if(ty>=sc->h)
		ty=sc->h-1;
	if(ty<0)
		ty=0;
	tx/=DEFBUTTONH;
	ty/=DEFBUTTONH;
	tx*=DEFBUTTONH;
	ty*=DEFBUTTONH;
	*x=tx;
	*y=ty;
	return TRUE;
}
int set_control_pos(CONTROL *c,int *x,int *y,int *w,int *h)
{
	int result=FALSE;
	if(c==0)
		return result;
	switch(c->type){
	case CBUTTON:
		{
			BUTTON *b;
			b=c->data;
			if(b){
				if(x)
					b->x=*x;
				if(y)
					b->y=*y;
				if(w)
					b->w=*w;
				if(h)
					b->h=*h;
				result=TRUE;
			}
		}
		break;
	}
}
int get_control_pos(CONTROL *control,int *x,int *y,int *w,int *h){
	int result=FALSE;
	if(control==0)
		return result;
	switch(control->type){
	case CBUTTON:
		{
			BUTTON *b;
			b=control->data;
			if(b){
				if(x)
					*x=b->x;
				if(y)
					*y=b->y;
				if(w)
					*w=b->w;
				if(h)
					*h=b->h;
				result=TRUE;
			}
		}
		break;
	case CSCROLL:
		{
			SCROLLBAR *s;
			s=control->data;
			if(s){
				if(x)
					*x=s->x;
				if(y)
					*y=s->y;
				if(w)
					*w=s->w;
				if(h)
					*h=s->h;
				result=TRUE;
			}
		}
		break;
	case CSTATIC:
		{
			STATICTEXT *s;
			s=control->data;
			if(s){
				if(x)
					*x=s->x;
				if(y)
					*y=s->y;
				if(w)
					*w=s->w;
				if(h)
					*h=s->h;
				result=TRUE;
			}
		}
		break;
	case CRECT:
		{
			RECTANGLE *r;
			r=control->data;
			if(r){
				if(x)
					*x=r->x;
				if(y)
					*y=r->y;
				if(w)
					*w=r->w;
				if(h)
					*h=r->h;
				result=TRUE;
			}
		}
		break;
	case CEDIT:
		{
			EDITBOX *e;
			e=control->data;
			if(e){
				if(x)
					*x=e->x;
				if(y)
					*y=e->y;
				if(w)
					*w=e->w;
				if(h)
					*h=e->h;
				result=TRUE;
			}
		}
		break;
	case PC_3FLOATA:
		{
			C3FLOATA *c;
			c=control->data;
			if(c){
				if(x)
					*x=c->x;
				if(y)
					*y=c->y;
				if(w)
					*w=c->w;
				if(h)
					*h=c->h;
				result=TRUE;
			}

		}
		break;
	}
	return result;
}
int find_selected_op(PAGE_DATA *p,OP **op)
{
	int result=FALSE;
	OP *oplist;
	if(p==0)
		return result;
	oplist=p->list;
	while(oplist){
		if(oplist->selected){
			if(op){
				*op=oplist;
			}
			result=TRUE;
			break;
		}
		oplist=oplist->list_next;
	}
	return result;
}
int find_drag_op(PAGE_DATA *p,OP **op)
{
	int result=FALSE;
	OP *oplist;
	if(p==0)
		return result;
	oplist=p->list;
	while(oplist){
		if(oplist->type==TDRAG){
			if(op)
				*op=oplist;
			result=TRUE;
			break;
		}
		oplist=oplist->list_next;
	}
	return result;
}
int clear_pressed_all(PAGE_DATA *p)
{
	int count=0;
	OP *oplist;
	if(p==0)
		return count;
	oplist=p->list;
	while(oplist){
		if(oplist->control.type==CBUTTON){
			BUTTON *b=oplist->control.data;
			if(b){
				b->pressed=FALSE;
				count++;
			}
		}
		oplist->selected=FALSE;
		oplist=oplist->list_next;
	}
	return count;
}
int hittest_op(PAGE_DATA *p,int x,int y,OP **op)
{
	int result=FALSE;
	OP *oplist;
	if(p==0)
		return result;
	oplist=p->list;
	while(oplist){
		int tx,ty,tw,th;
		if(get_control_pos(&oplist->control,&tx,&ty,&tw,&th)){
			if(x>=tx && x<=(tx+tw)){
				if(y>=ty && y<=(ty+th)){
					if(op)
						*op=oplist;
					result=TRUE;
					break;
				}

			}
		}
		oplist=oplist->list_next;
	}
	return result;
}
int drag_control(SCREEN *sc,PAGE_DATA *p,int x,int y,int startx,int starty)
{
	int result=FALSE;
	if(p){
		OP *op=0;
		if(!find_drag_op(p,&op)){
			op=malloc(sizeof(OP));
			if(op){
				memset(op,0,sizeof(OP));
				if(create_op(TDRAG,op,x,y))
					add_op(p,op);
			}
		}
		if(op){
			CONTROLDRAG *d=op->control.data;
			if(d){
				int x1,y1,x2,y2;
				x1=startx;y1=starty;
				x2=x;y2=y;
				get_nearest_grid(sc,&x1,&y1);
				get_nearest_grid(sc,&x2,&y2);
				d->deltax=x2-x1;
				d->deltay=y2-y1;
				d->color=0xFF;
			}
			op->selected=TRUE;
		}
	}
	return result;
}
int is_location_avail(PAGE_DATA *p,int x,int y,int w,int h)
{
	int result=TRUE;
	if(p){
		OP *oplist=p->list;
		while(oplist){
			if(oplist->selected==FALSE){
				int ox,oy,ow,oh;
				if(get_control_pos(&oplist->control,&ox,&oy,&ow,&oh)){
					int list[8];
					int i;
					list[0]=x;
					list[1]=y;
					list[2]=x+w-1;
					list[3]=y;
					list[4]=x;
					list[5]=y+h-1;
					list[6]=x+w-1;
					list[7]=y+h-1;
					for(i=0;i<4;i+=2){
						int px,py;
						px=list[i+0];
						py=list[i+1];
						if(px>=ox && px<=(ox+ow-1)){
							if(py>=oy && py<=(oy+oh-1)){
								result=FALSE;
								break;
							}
						}
					}
				}
			}
			oplist=oplist->list_next;
		}
	}
	return result;
}
int drag_finish(SCREEN *sc,PAGE_DATA *p,OP *drag)
{
	int result=FALSE;
	if(p && drag){
		int dest_ok=TRUE;
		CONTROLDRAG *d=drag->control.data;
		OP *olist;
		int deltax=0,deltay=0;
		if(d){
			deltax=d->deltax;
			deltay=d->deltay;
		}
		olist=p->list;
		while(olist){
			if(olist->type!=TDRAG && olist->selected==TRUE){
				int x,y,w,h;
				get_control_pos(&olist->control,&x,&y,&w,&h);
				x+=d->deltax;
				y+=d->deltay;
				if(x<0 || y<0 || (x+w)>=sc->w || (y+h)>=sc->h){
					dest_ok=FALSE;
					break;
				}
				if(!is_location_avail(p,x,y,w,h)){
					dest_ok=FALSE;
					break;
				}
			}
			olist=olist->list_next;
		}
		if(dest_ok){
			olist=p->list;
			while(olist){
				CONTROL *c=&olist->control;
				if(c){
					switch(c->type){
					case CBUTTON:
						{
							BUTTON *b=c->data;
							if(b && b->pressed){
								b->x+=deltax;
								b->y+=deltay;
							}
						}
						break;
					}
				}
				olist=olist->list_next;
			}
		}
	}
	return result;
}
int clear_params()
{
	extern PARAM_LIST param_list;
	PARAM_CONTROL *p=param_list.list;
	while(p){
		PARAM_CONTROL *tmp;
		if(p->control.data){
			switch(p->control.type){
			case CEDIT:
				{
					EDITBOX *e=p->control.data;
					if(e && e->str)
						free(e->str);
				}
				break;
			}
			free(p->control.data);
		}
		tmp=p;
		p=p->next;
		free(tmp);
	}
	param_list.list=0;
	return TRUE;
}
int add_param_control(PARAM_LIST *pl,PARAM_CONTROL *c)
{
	int result=FALSE;
	if(pl==0 || c==0)
		return result;
	if(pl->list==0){
		pl->list=c;
		result=TRUE;
	}else{
		PARAM_CONTROL *list=pl->list;
		while(list->next!=0){
			list=list->next;
		}
		list->next=c;
		c->prev=list;
		result=TRUE;
	}
	return result;
}
int create_param_control(int type,PARAM_CONTROL *pc)
{
	int result=FALSE;
	int size=0;
	if(pc==0)
		return result;
	switch(type){
	case PC_3FLOATA: size=sizeof(C3FLOATA);break;
	case CSTATIC: size=sizeof(STATICTEXT);break;
	case CEDIT: size=sizeof(EDITBOX);break;
	}
	if(size!=0){
		void *data=0;
		data=malloc(size);
		if(data!=0){
			memset(data,0,size);
			pc->control.data=data;
			pc->control.type=type;
			result=TRUE;
		}
	}
	return result;
}
int create_op_params(OP *o)
{
	extern PARAM_LIST param_list;
	int result=FALSE;
	clear_params();
	if(o==0)
		return result;
	else{
		PARAM_LIST *pl=&param_list;
		switch(o->type){
		case TCUBE:
			{
				struct PCLIST{
					int type;
					const char *name;
					const char *text;
					int x,y,w,h;
				};
				struct PCLIST pclist[]={
					{CSTATIC,"type","cube",8*8,0,40,20},
					{CEDIT,"name","",8*8,0,8*40,20},
					{PC_3FLOATA,"tesselate","",10*8,0,3*12*8,20},
				};
				int i,xpos=0,ypos=0;
				for(i=0;i<sizeof(pclist)/sizeof(struct PCLIST);i++){
					PARAM_CONTROL *pc=malloc(sizeof(PARAM_CONTROL));
					if(pc){
						memset(pc,0,sizeof(PARAM_CONTROL));
						if(create_param_control(pclist[i].type,pc)){
							xpos=0;
							switch(pclist[i].type){
							case PC_3FLOATA:
								{
									C3FLOATA *c=pc->control.data;
									if(c){
										c->x=pclist[i].x+xpos;
										c->y=pclist[i].y+ypos;
										c->w=pclist[i].w;
										c->h=pclist[i].h;
										_snprintf(c->numa,sizeof(c->numa),"%.4f",c->a);
										_snprintf(c->numb,sizeof(c->numb),"%.4f",c->b);
										_snprintf(c->numc,sizeof(c->numc),"%.4f",c->c);
										pc->name=pclist[i].name;
										pc->x=xpos;
										pc->y=ypos;
										result=TRUE;
									}
								}
								break;
							case CSTATIC:
								{
									STATICTEXT *c=pc->control.data;
									if(c){
										c->x=pclist[i].x+xpos;
										c->y=pclist[i].y+ypos;
										c->w=pclist[i].w;
										c->h=pclist[i].h;
										c->str=pclist[i].text;
										pc->name=pclist[i].name;
										pc->x=xpos;
										pc->y=ypos;
										result=TRUE;
									}
								}
								break;
							case CEDIT:
								{
									EDITBOX *c=pc->control.data;
									if(c){
										char *str=0;
										int maxlen=80;
										str=malloc(maxlen+1);
										if(str)
											memset(str,0,maxlen+1);
										c->x=pclist[i].x+xpos;
										c->y=pclist[i].y+ypos;
										c->w=pclist[i].w;
										c->h=pclist[i].h;
										c->str=pclist[i].text;
										c->str=str;
										c->maxlen=maxlen;
										pc->name=pclist[i].name;
										pc->x=xpos;
										pc->y=ypos;
										result=TRUE;
									}
								}
								break;
							}
						}
					}
					if((!result) && pc)
						free(pc);
					if(result){
						add_param_control(pl,pc);
						ypos+=30;
					}
				}
			}
			break;
		}
	}
}
int page_win_message(SCREEN *sc,HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int drag=0,clickx=0,clicky=0;
	PAGE_DATA *p;
	extern PAGE_LIST page_list;
	p=page_list.current;
	if(p==0)
		p=page_list.list;
	if(p==0)
		return FALSE;
	switch(msg){
	case WM_KEYFIRST:
		{
			int key=wparam;
			int control;
			RECT rect={0};
			GetWindowRect(hwnd,&rect);
			control=GetKeyState(VK_CONTROL)&0x8000;
			switch(key){
			case 'A':
				break;
			case VK_PRIOR:
				scroll_page_view(hwnd,sc,-rect.bottom,control);
				break;
			case VK_NEXT:
				scroll_page_view(hwnd,sc,rect.bottom,control);
				break;
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{
			short w=HIWORD(wparam);
			int key=LOWORD(wparam);
			int amount=DEFBUTTONH;
			int control=0;
			if(key&MK_SHIFT)
				amount<<=2;
			if(key&MK_CONTROL)
				control=1;

			if(w>0)
				amount=-amount;
			scroll_page_view(hwnd,sc,amount,control);
		}
		break;
	case WM_MOUSEMOVE:
		{
			int lmb=wparam&MK_LBUTTON;
			int x,y;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			if(lmb && drag){
				x+=p->hscroll;
				y+=p->vscroll;
				drag_control(sc,p,x,y,p->cursorx+DEFBUTTONH/2,p->cursory+DEFBUTTONH/2);
			}
			if(p->vscroll_pressed){
				float delta=y-clicky;
				RECT rect={0};
				int h,bh,range,d;
				GetWindowRect(hwnd,&rect);
				h=rect.bottom-rect.top;
				range=sc->h-h;
				bh=h-(range/3);
				if(bh<10){
					bh=10;
					if(h<10)
						bh=h;
				}
				d=h-bh;
				if(d<=0)
					d=1;
				delta=delta*range/d;
				p->vscroll+=(int)delta;
				clicky=y;
			}
			if(p->hscroll_pressed){
				float delta=x-clickx;
				RECT rect={0};
				int w,bw,range,d;
				GetWindowRect(hwnd,&rect);
				w=rect.right-rect.left;
				range=sc->w-w;
				bw=w-(range/3);
				if(bw<10){
					bw=10;
					if(w<10)
						bw=w;
				}
				d=w-bw;
				if(d<=0)
					d=1;
				delta=delta*range/d;
				p->hscroll+=(int)delta;
				clickx=x;
			}
		}
		break;
	case WM_KILLFOCUS:
		break;
	case WM_LBUTTONUP:
		{
			OP *op=0;
			if(drag){
				if(find_drag_op(p,&op)){
					drag_finish(sc,p,op);
					del_op(p,op);
				}
				drag=0;
			}
			p->hscroll_pressed=0;
			p->vscroll_pressed=0;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int x,y,cx,cy;
			OP *op=0;
			clickx=x=LOWORD(lparam);
			clicky=y=HIWORD(lparam);
			x+=p->hscroll;
			y+=p->vscroll;
			cx=x;
			cy=y;
			get_nearest_grid(sc,&cx,&cy);
			if(!(wparam&MK_CONTROL))
				clear_pressed_all(p);
			hittest_op(p,x,y,&op);
			if(op){
				drag=TRUE;
				op->selected=TRUE;
				create_op_params(op);
				if(op->control.type==CBUTTON){
					BUTTON *b=op->control.data;
					if(b){
						b->pressed=TRUE;
					}
				}
			}
			else{
			}
			if(!check_scroll_hit(sc,p,hwnd,x,y)){
				p->cursorx=cx;
				p->cursory=cy;
			}
		}
		break;
	case WM_RBUTTONDOWN:
		{
			int x,y;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
		}
		break;
	}
	return TRUE;
}
int hittest_param(PARAM_CONTROL *pclist,int x,int y,PARAM_CONTROL **pc)
{
	int result=FALSE;
	if(pclist==0)
		return result;
	while(pclist){
		int tx,ty,tw,th;
		if(get_control_pos(&pclist->control,&tx,&ty,&tw,&th)){
			printf("x=%i y=%i tx=%i ty=%i twend=%i thend=%i\n",x,y,tx,ty,tx+tw,ty+th);
			if(x>=tx && x<=(tx+tw)){
				if(y>=ty && y<=(ty+th)){
					if(pc)
						*pc=pclist;
					result=TRUE;
					break;
				}

			}
		}
		pclist=pclist->next;
	}
	return result;
}
int clear_param_selected(PARAM_CONTROL *list)
{
	int count=0;
	while(list!=0){
		list->has_focus=FALSE;
		count++;
		list=list->next;
	}
	return count;
}
int send_char_control(CONTROL *c,WPARAM wparam,LPARAM lparam)
{
	int result=FALSE;
	if(c){
		int key=wparam;
		switch(c->type){
		case CEDIT:
			{
				EDITBOX *e=c->data;
				if(e && e->str){
					if(e->cursor < e->maxlen){
						e->str[e->cursor]=wparam;
						e->cursor++;
					}
				}
			}
			break;
		case PC_3FLOATA:
			break;
		}
	}
	return result;
}
int param_win_message(SCREEN *sc,HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int clickx=0,clicky=0;
	PARAM_CONTROL *p;
	extern PARAM_LIST param_list;
	p=param_list.list;
	if(p==0)
		return FALSE;
	switch(msg){
	case WM_LBUTTONDOWN:
		{
			int x,y;
			PARAM_CONTROL *pc=0;
			clickx=x=LOWORD(lparam);
			clicky=y=HIWORD(lparam);
			clear_param_selected(p);
			if(hittest_param(p,x,y,&pc)){
				pc->has_focus=TRUE;
			}
		}
		break;
	case WM_KEYFIRST:
		{
			PARAM_CONTROL *list=p;
			while(list){
				if(list->has_focus){
					send_char_control(&list->control,wparam,lparam);
				}
				list=list->next;
			}
		}
		break;
	}
}
int check_scroll_hit(SCREEN *sc,PAGE_DATA *p,HWND hwnd,int x,int y)
{
	int result=FALSE;
	if(sc==0)
		return result;
	else{
		RECT rect={0};
		int rwidth,rheight;
		GetWindowRect(hwnd,&rect);
		rwidth=rect.right-rect.left;
		if(rwidth>sc->w)
			rwidth=sc->w;
		rheight=rect.bottom-rect.top;
		if(rheight>sc->h)
			rheight=sc->h;
		if(sc->h > rheight){
			if(x > (p->hscroll+rwidth-SCROLL_WIDTH)){
				p->vscroll_pressed=TRUE;
				result=TRUE;
			}
			else
				p->vscroll_pressed=FALSE;
		}
		if(sc->w > rwidth){
			if(y > (p->vscroll+rheight-SCROLL_WIDTH)){
				p->hscroll_pressed=TRUE;
				result=TRUE;
			}
			else
				p->hscroll_pressed=FALSE;
		}
	}
	return result;
}
int scroll_page_view(HWND hwnd,SCREEN *sc,int amount,int control)
{
	PAGE_DATA *p;
	extern PAGE_LIST page_list;
	p=page_list.current;
	if(p==0)
		p=page_list.list;
	if(p){
		RECT rect={0};
		int rw,rh;
		GetWindowRect(hwnd,&rect);
		rw=rect.right-rect.left;
		rh=rect.bottom-rect.top;
		if((!control) && rh>=sc->h){
			p->vscroll=0;
			return 0;
		}
		if(control && rw>=sc->w){
			p->hscroll=0;
			return 0;
		}
		if(control){
			p->hscroll+=amount;
			if(p->hscroll>=sc->w)
				p->hscroll=sc->w-1;
			if(p->hscroll<0)
				p->hscroll=0;
		}
		else{
			p->vscroll+=amount;
			if(p->vscroll>=sc->h)
				p->vscroll=sc->h-1;
			if(p->vscroll<0)
				p->vscroll=0;
		}
	}
	return 0;
}
