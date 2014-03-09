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
	}
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
				button->w=DEFBUTTONW;
				button->h=DEFBUTTONH;
				button->x=x;
				button->y=y;
				op->control.type=TBUTTON;
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
	tx+=DEFBUTTONH/2;
	ty*=DEFBUTTONH;
	ty+=DEFBUTTONH/2;
	*x=tx;
	*y=ty;
	return TRUE;
}
int find_op(PAGE_DATA *p,int x,int y,OP *op)
{

}
int page_win_message(SCREEN *sc,HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
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
	case WM_LBUTTONDOWN:
		{
			int x,y;
			OP *op=0;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			p->cursorx=p->hscroll+x;
			p->cursory=p->vscroll+y;
			get_nearest_grid(sc,&p->cursorx,&p->cursory);
			find_op(p,x,y,op);
			if(op){

			}
		}
		break;
	case WM_RBUTTONDOWN:
		{
			int x,y;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
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
