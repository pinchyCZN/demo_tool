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
				int tx=x-startx,ty=y-starty;
				d->deltax=(tx/DEFBUTTONH)*DEFBUTTONH;
				d->deltay=(ty/DEFBUTTONH)*DEFBUTTONH;
				d->color=0xFF;
			}
		}
	}
	return result;
}
int drag_finish()
{
}
int check_free_pos(PAGE_DATA *p,OP *exclude[],int excount,int x,int y,int w,int h)
{
	int result=TRUE;
	OP *oplist;
	if(p==0)
		return result;
	oplist=p->list;
	while(oplist){
		int tx,ty,tw,th;
		int i,skip=FALSE;
		for(i=0;i<excount;i++){
			if(exclude[i]==oplist){
				skip=TRUE;
				break;
			}
		}
		if(!skip){
			if(get_op_pos(oplist,&tx,&ty,&tw,&th)){
				int xr;
				if(x>=tx && x<=(tx+tw-1)){
					if(y>=ty && y<=(ty+th-1)){
						result=FALSE;
						break;
					}
				}
				xr=x+w-1;
				if(xr>=tx && xr<=(tx+tw-1)){
					if(y>=ty && y<=(ty+th-1)){
						result=FALSE;
						break;
					}
				}
			}
		}
		oplist=oplist->list_next;
	}
	return result;
}
int page_win_message(SCREEN *sc,HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int drag=0,startx=0,starty=0;
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
		switch(key){
		case 'A':
			break;
		}
		}
		break;
	case WM_MOUSEMOVE:
		{
			int x,y;
			int lmb=wparam&MK_LBUTTON;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			if(lmb && drag){
				drag_control(sc,p,x,y,startx,starty);
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			OP *op=0;
			if(find_drag_op(p,&op)){
				if(drag){
					CONTROLDRAG *d=op->control.data;
					if(d){
						drag_finish(p,d);
					}
				}
				del_op(p,op);
			}
			drag=0;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int x,y;
			OP *op=0;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			p->cursorx=p->hscroll+x;
			p->cursory=p->vscroll+y;
			get_nearest_grid(sc,&p->cursorx,&p->cursory);
			if(!(wparam&MK_CONTROL))
				clear_pressed_all(p);
			hittest_op(p,x,y,&op);
			if(op){
				drag=TRUE;
				op->selected=TRUE;
				if(op->control.type==CBUTTON){
					BUTTON *b=op->control.data;
					if(b){
						b->pressed=TRUE;
					}
				}
			}
			startx=x;
			starty=y;
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
