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
int add_cube_op(PAGE_DATA *p,int x,int y)
{
	int result=FALSE;
	OP *op;
	op=malloc(sizeof(OP));
	if(op){
		memset(op,0,sizeof(OP));
		if(create_op(TCUBE,op,x+p->hscroll,y+p->vscroll)){
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
int get_op_pos(OP *op,int *x,int *y,int *w,int *h){
	int result=FALSE;
	if(op==0)
		return result;
	switch(op->control.type){
	case CBUTTON:
		{
			BUTTON *b;
			b=op->control.data;
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
			s=op->control.data;
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
			s=op->control.data;
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
			r=op->control.data;
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
		if(get_op_pos(oplist,&tx,&ty,&tw,&th)){
			if(x>=tx && x<=(tx+tw)){
				if(y>=ty && y<=(ty+th)){
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
				if(get_op_pos(oplist,&ox,&oy,&ow,&oh)){
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
				get_op_pos(olist,&x,&y,&w,&h);
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

int page_win_message(SCREEN *sc,HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int drag=0,clickx=0,clicky=0;
	PAGE_DATA *p;
	extern PAGE_LIST page_list;
	extern OP *selected_op;
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
			int amount=DEFBUTTONH/2;
			int control=0;
			if(key&MK_SHIFT)
				amount<<=3;
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
				int height;
				GetWindowRect(hwnd,&rect);
				height=rect.bottom-rect.top;
				if(height<=0)
					height=1;
				delta*=((float)sc->h/(float)height);
				p->vscroll+=(int)delta;
				clicky=y;
			}
			if(p->hscroll_pressed){
				float delta=x-clickx;
				RECT rect={0};
				int width;
				GetWindowRect(hwnd,&rect);
				width=rect.right-rect.left;
				//width+=240;
				if(width<=0)
					width=1;
				delta*=((float)sc->w/(float)width);
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
			int x,y;
			OP *op=0;
			clickx=x=LOWORD(lparam);
			clicky=y=HIWORD(lparam);
			x+=p->hscroll;
			y+=p->vscroll;
			p->cursorx=x;
			p->cursory=y;
			get_nearest_grid(sc,&p->cursorx,&p->cursory);
			if(!(wparam&MK_CONTROL))
				clear_pressed_all(p);
			hittest_op(p,x,y,&op);
			if(op){
				drag=TRUE;
				op->selected=TRUE;
				selected_op=op;
				if(op->control.type==CBUTTON){
					BUTTON *b=op->control.data;
					if(b){
						b->pressed=TRUE;
					}
				}
			}
			else
				selected_op=0;
			check_scroll_hit(sc,p,hwnd,x,y);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			int x,y;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			get_nearest_grid(sc,&x,&y);
			add_cube_op(p,x,y);
		}
		break;
	}
	return TRUE;
}
int check_scroll_hit(SCREEN *sc,PAGE_DATA *p,HWND hwnd,int x,int y)
{
	if(sc==0)
		return 0;
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
			if(x > (p->hscroll+rwidth-SCROLL_WIDTH))
				p->vscroll_pressed=TRUE;
			else
				p->vscroll_pressed=FALSE;
		}
		if(sc->w > rwidth){
			if(y > (p->vscroll+rheight-SCROLL_WIDTH))
				p->hscroll_pressed=TRUE;
			else
				p->hscroll_pressed=FALSE;
		}
	}
	return 0;
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
