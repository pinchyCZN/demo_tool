#define _WIN32_WINNT 0x400
#include <windows.h>
#include <math.h>
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
	return result;
}
int del_op(PAGE_DATA *pd,OP *op)
{
	extern PARAM_LIST param_list;
	int result=FALSE;
	if(pd==0 || op==0)
		return result;
	if(op->selected && op->type!=TDRAG)
		clear_params(&param_list);
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
		if(prev==0)
			pd->list=next;
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
int create_op_button(int otype,int osize,OP *op,int x,int y)
{
	int result=FALSE;
	BUTTON *button;
	void *op_data;
	button=malloc(sizeof(BUTTON));
	op_data=malloc(osize);
	if(button && op_data){
		memset(button,0,sizeof(BUTTON));
		memset(op_data,0,osize);
		button->w=DEFBUTTONW;
		button->h=DEFBUTTONH;
		button->x=x;
		button->y=y;
		op->control.type=CBUTTON;
		op->control.data=button;
		op->type=otype;
		op->data=op_data;
		result=TRUE;
	}
	else{
		if(button)
			free(button);
		if(op_data)
			free(op_data);
	}
	return result;
}
int create_op(int type,OP *op,int x,int y)
{
	int result=FALSE;
	if(op==0)
		return result;
	switch(type){
	case TLIGHT:
		result=create_op_button(type,sizeof(LIGHT_DATA),op,x,y);
		break;
	case TCUBE:
		result=create_op_button(type,sizeof(CUBE_DATA),op,x,y);
		break;
	case TMULTIPLY:
		{
			MULTIPLY_DATA *m;
			result=create_op_button(type,sizeof(MULTIPLY_DATA),op,x,y);
			m=op->data;
			if(m){
				m->scalex=m->scaley=m->scalez=1.0;
			}
		}
		break;
	case TTRANSFORM:
		{
			TRANSFORM_DATA *t;
			result=create_op_button(type,sizeof(TRANSFORM_DATA),op,x,y);
			t=op->data;
			if(t){
				t->scalex=t->scaley=t->scalez=1.0;
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
	case TRESIZE:
		{
			RESIZERECT *r;
			r=malloc(sizeof(RESIZERECT));
			if(r){
				memset(r,0,sizeof(RESIZERECT));
				op->control.type=CRESIZERECT;
				op->control.data=r;
				op->type=type;
				r->x=x;
				r->y=y;
				r->w=10;
				r->h=10;
				r->color=0xFFFFFF;
				r->filled=1;
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
int trans_pos_data(int *x,int *y,int *w,int *h,int sx,int sy,int sw,int sh)
{
	if(x)*x=sx;
	if(y)*y=sy;
	if(w)*w=sw;
	if(h)*h=sh;
	return TRUE;
}
int get_control_pos(CONTROL *control,int *x,int *y,int *w,int *h){
	int result=FALSE;
	if(control==0)
		return result;
	switch(control->type){
	case CPOPUPLIST:
		{
			POPUPLIST *pu;
			pu=control->data;
			if(pu){
				trans_pos_data(x,y,w,h,pu->x,pu->y,pu->w,pu->h);
				result=TRUE;
			}
		}
		break;
	case CDROPLIST:
		{
			DROPLIST *dl;
			dl=control->data;
			if(dl){
				trans_pos_data(x,y,w,h,dl->x,dl->y,dl->w,dl->h);
				result=TRUE;
			}
		}
		break;
	case CBUTTON:
		{
			BUTTON *b;
			b=control->data;
			if(b){
				trans_pos_data(x,y,w,h,b->x,b->y,b->w,b->h);
				result=TRUE;
			}
		}
		break;
	case CSCROLL:
		{
			SCROLLBAR *s;
			s=control->data;
			if(s){
				trans_pos_data(x,y,w,h,s->x,s->y,s->w,s->h);
				result=TRUE;
			}
		}
		break;
	case CSTATIC:
		{
			STATICTEXT *s;
			s=control->data;
			if(s){
				trans_pos_data(x,y,w,h,s->x,s->y,s->w,s->h);
				result=TRUE;
			}
		}
		break;
	case CRECT:
		{
			RECTANGLE *r;
			r=control->data;
			if(r){
				trans_pos_data(x,y,w,h,r->x,r->y,r->w,r->h);
				result=TRUE;
			}
		}
		break;
	case CEDIT:
		{
			EDITBOX *e;
			e=control->data;
			if(e){
				trans_pos_data(x,y,w,h,e->x,e->y,e->w,e->h);
				result=TRUE;
			}
		}
		break;
	case CEDITFLOAT:
		{
			EDITFLOAT *e;
			e=control->data;
			if(e){
				trans_pos_data(x,y,w,h,e->x,e->y,e->w,e->h);
				result=TRUE;
			}
		}
		break;
	case CEDITBYTE:
		{
			EDITBYTE *e;
			e=control->data;
			if(e){
				trans_pos_data(x,y,w,h,e->x,e->y,e->w,e->h);
				result=TRUE;
			}
		}
		break;
	case CEDITINT:
		{
			EDITINT *e;
			e=control->data;
			if(e){
				trans_pos_data(x,y,w,h,e->x,e->y,e->w,e->h);
				result=TRUE;
			}
		}
		break;
	}
	return result;
}
int find_root_op(PAGE_DATA *p,OP **op)
{
	int result=FALSE;
	OP *oplist;
	if(p==0)
		return result;
	oplist=p->list;
	while(oplist){
		if(oplist->isroot){
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
int find_op_type(PAGE_DATA *p,OP **op,int type)
{
	int result=FALSE;
	OP *oplist;
	if(p==0)
		return result;
	oplist=p->list;
	while(oplist){
		if(oplist->type==type){
			if(op)
				*op=oplist;
			result=TRUE;
			break;
		}
		oplist=oplist->list_next;
	}
	return result;
}
int set_root(PAGE_DATA *p,OP *o)
{
	OP *oplist;
	if(p==0 || o==0)
		return FALSE;
	oplist=p->list;
	while(oplist){
		oplist->isroot=FALSE;
		oplist=oplist->list_next;
	}
	o->isroot=TRUE;
	return TRUE;
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
int clear_selected_all(PAGE_DATA *p)
{
	int count=0;
	OP *oplist;
	if(p==0)
		return count;
	oplist=p->list;
	while(oplist){
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
		if(!find_op_type(p,&op,TDRAG)){
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
int add_resize_op(PAGE_DATA *p,CONTROL *c)
{
	int result=FALSE;
	OP *op=0;
	find_op_type(p,&op,TRESIZE);
	if(op==0){
		op=malloc(sizeof(OP));
		if(op){
			int x=0,y=0;
			get_control_pos(c,&x,&y,0,0);
			memset(op,0,sizeof(OP));
			if(create_op(TRESIZE,op,x,y)){
				add_op(p,op);
			}
			else{
				free(op);
				op=0;
			}
		}
	}
	if(op)
		result=TRUE;
	return result;
}
int resize_selected(PAGE_DATA *p,int x,int y,int startx,int starty)
{
	int result=FALSE;
	if(p){
		OP *oplist=p->list;
		while(oplist){
			if(oplist->selected){
				if(oplist->control.type==CBUTTON){
					BUTTON *b=oplist->control.data;
					if(b){
						int deltax=startx-b->x + (x-startx);
						deltax-=deltax%DEFBUTTONH;
						b->w=deltax+DEFBUTTONH;
						b->w-=b->w%DEFBUTTONH;
						if(b->w<DEFBUTTONW)
							b->w=DEFBUTTONW;
						result=TRUE;
					}
				}
			}
			oplist=oplist->list_next;
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
int page_win_message(SCREEN *sc,HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int drag=0,resize=0,clickx=0,clicky=0,debounce=0;
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
			if(lmb){
				if(debounce==0){
					int size=4;
					int deltax=0,deltay=0;
					deltax=clickx-x;
					deltay=clicky-y;
					if((deltax<-size) || (deltax>size))
						debounce=1;
					if((deltay<-size) || (deltay>size))
						debounce=1;
				}
				if(drag && debounce!=0){
					drag_control(sc,p,x+p->hscroll,y+p->vscroll,p->cursorx+DEFBUTTONH/2,p->cursory+DEFBUTTONH/2);
				}
				if(resize && debounce!=0){
					resize_selected(p,x+p->hscroll,y+p->vscroll,p->cursorx+DEFBUTTONH/2,p->cursory+DEFBUTTONH/2);
				}
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
				if(find_op_type(p,&op,TDRAG)){
					drag_finish(sc,p,op);
					del_op(p,op);
				}
				drag=0;
			}
			if(resize){
				if(find_op_type(p,&op,TRESIZE)){
					del_op(p,op);
				}
				resize=0;
			}
			p->hscroll_pressed=0;
			p->vscroll_pressed=0;
			test_build(p);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int x,y,cx,cy;
			OP *op=0;
			clickx=x=LOWORD(lparam);
			clicky=y=HIWORD(lparam);
			debounce=0;
			x+=p->hscroll;
			y+=p->vscroll;
			cx=x;
			cy=y;
			get_nearest_grid(sc,&cx,&cy);
			if(!(wparam&MK_CONTROL))
				clear_pressed_all(p);
			if(!check_scroll_hit(sc,p,hwnd,x,y)){
				p->cursorx=cx;
				p->cursory=cy;
			}
			else
				break;

			hittest_op(p,x,y,&op);
			if(op){
				clear_selected_all(p);
				if(wparam&MK_CONTROL)
					set_root(p,op);
				op->selected=TRUE;
				create_op_params(op);
				if(op->control.type==CBUTTON){
					BUTTON *b=op->control.data;
					if(b){
						int xright=b->x+b->w;
						if(x<xright && x>=(xright-RESIZE_MARGIN)){
							resize=TRUE;
							add_resize_op(p,&op->control);
						}
						b->pressed=TRUE;
					}
				}
				if(!resize)
					drag=TRUE;
			}
			else{
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
	PARAM_CONTROL *pl=pclist;
	if(pclist==0)
		return result;
	while(pl){
		if(pl->next==0)
			break;
		else
			pl=pl->next;
	}
	while(pl){
		int tx,ty,tw,th;
		if(get_control_pos(&pl->control,&tx,&ty,&tw,&th)){
			if(x>=tx && x<=(tx+tw)){
				if(y>=ty && y<=(ty+th)){
					if(pc)
						*pc=pl;
					result=TRUE;
					break;
				}

			}
		}
		pl=pl->prev;
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
int handle_edit_keys(int key,int vkey,int shift,int ctrl,char *str,int maxlen,int *cursor,int *overwrite)
{
	int result=FALSE;
	if(str==0)
		return result;
	switch(vkey){
	case VK_BACK:
		if(*cursor>0){
			int i;
			for(i=*cursor-1;i<maxlen;i++){
				str[i]=str[i+1];
				if(str[i+1]==0)
					break;
			}
			(*cursor)--;
			result=TRUE;
		}
		break;
	case VK_END:
		*cursor=strlen(str);
		break;
	case VK_HOME:
		*cursor=0;
		break;
	case VK_LEFT:
		(*cursor)--;
		break;
	case VK_RIGHT:
		if(str[*cursor]!=0)
			(*cursor)++;
		break;
	case VK_INSERT:
		*overwrite=!(*overwrite);
		break;
	case VK_DELETE:
		{
			int i;
			for(i=*cursor;i<maxlen;i++){
				if(str[i]!=0)
					result=TRUE;
				str[i]=str[i+1];
			}
		}
		break;
	}
	if(key < ' ' || key > 0x7E)
		;
	else if(*cursor < maxlen){
		int k=key;
		if(!shift)
			k=tolower(key);
		else
			k=toupper(key);
		if(!(*overwrite)){
			int i;
			for(i=maxlen-2;i>=*cursor;i--){
				str[i+1]=str[i];
			}
		}
		str[*cursor]=k;
		(*cursor)++;
		result=TRUE;
	}
	if(*cursor < 0)
		*cursor=0;
	else if(*cursor > maxlen)
		*cursor = maxlen;
	return result;
}
int type_edit_modify(char *fmt,char *str,int size,int *cursor,int dtype,void *d,int update)
{
	if(d && fmt && str && size>0){
		int len;
		switch(dtype){
		case ETYPE_FLOAT:
			{
				float *f=d;
				if(update)
					*f=atof(str);
				_snprintf(str,size,fmt,*f);
			}
			break;
		case ETYPE_BYTE:
			{
				unsigned char *b=d;
				if(update){
					unsigned int i;
					i=atoi(str);
					if(i>255)
						i=255;
					*b=i;
				}
				_snprintf(str,size,fmt,*b);
			}
			break;
		case ETYPE_INT:
			{
				int *i=d;
				if(update)
					*i=atoi(str);
				_snprintf(str,size,fmt,*i);
			}
			break;
		}
		str[size-1]=0;
		len=strlen(str);
		if(*cursor>len)
			*cursor=len;
		if(*cursor<0)
			*cursor=0;
	}
	return 0;
}
int zero_edit_data(int dtype,void *data)
{
	switch(dtype){
	case ETYPE_FLOAT:
		{
			float *f=data;
			if(f)
				*f=0;
		}
		break;
	case ETYPE_BYTE:
		{
			unsigned char *b=data;
			if(b)
				*b=0;
		}
		break;
	case ETYPE_INT:
		{
			int *i=data;
			if(i)
				*i=0;
		}
		break;
	}
	return TRUE;
}
int handle_edit_control(int key,int vkey,int ctrl,int shift,int dtype,char *str,int str_size,int *cursor,int *changed,int *overwrite,void *data)
{
	if(str){
		int tilde=VkKeyScan('`');
		if(vkey!=tilde){
			if(handle_edit_keys(key,vkey,shift,ctrl,str,str_size-1,cursor,overwrite))
				*changed=TRUE;
		}
		if(vkey==VK_RETURN){
			if(data){
				switch(dtype){
				case ETYPE_FLOAT:
					type_edit_modify("%.4f",str,str_size,cursor,dtype,data,TRUE);
					break;
				case ETYPE_BYTE:
				case ETYPE_INT:
					type_edit_modify("%i",str,str_size,cursor,dtype,data,TRUE);
					break;
				}
			}
			*changed=FALSE;
		}
		else if(vkey==VK_ESCAPE || vkey==tilde){
			if(data){
				if(vkey==tilde)
					zero_edit_data(dtype,data);
				switch(dtype){
				case ETYPE_FLOAT:
					type_edit_modify("%.4f",str,str_size,cursor,dtype,data,FALSE);
					break;
				case ETYPE_BYTE:
				case ETYPE_INT:
					type_edit_modify("%i",str,str_size,cursor,dtype,data,FALSE);
					break;
				}
			}
			*changed=FALSE;
		}
		else if(vkey==VK_UP || vkey==VK_DOWN){
			if(data){
				if(*cursor<0)
					*cursor=0;
				if(*cursor<str_size){
					unsigned char c;
					c=str[*cursor];
					if(isdigit(c)){
						if(vkey==VK_UP)
							c++;
						else
							c--;
						if(c>'9'){
							c='0';
						}
						if(c<'0')
							c='0';
						str[*cursor]=c;
						switch(dtype){
						case ETYPE_FLOAT:
							type_edit_modify("%.4f",str,str_size,cursor,dtype,data,TRUE);
							break;
						case ETYPE_BYTE:
						case ETYPE_INT:
							type_edit_modify("%i",str,str_size,cursor,dtype,data,TRUE);
							break;
						}
						*changed=FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}
int send_char_control(CONTROL *c,int key,int vkey,int ctrl,int shift)
{
	int result=FALSE;
	if(c){
		switch(c->type){
		case CEDITFLOAT:
			{
				EDITFLOAT *e=c->data;
				handle_edit_control(key,vkey,ctrl,shift,ETYPE_FLOAT,e->str,sizeof(e->str),&e->cursor,&e->changed,&e->overwrite,e->fdata);
			}
			break;
		case CEDITBYTE:
			{
				EDITBYTE *e=c->data;
				handle_edit_control(key,vkey,ctrl,shift,ETYPE_BYTE,e->str,sizeof(e->str),&e->cursor,&e->changed,&e->overwrite,e->byte);
			}
			break;
		case CEDITINT:
			{
				EDITINT *e=c->data;
				handle_edit_control(key,vkey,ctrl,shift,ETYPE_INT,e->str,sizeof(e->str),&e->cursor,&e->changed,&e->overwrite,e->integer);
			}
			break;
		case CEDIT:
			{
				EDITBOX *e=c->data;
				if(e && e->str)
					handle_edit_keys(key,vkey,shift,ctrl,e->str,e->maxlen,&e->cursor,&e->overwrite);
			}
			break;

		}
	}
	return result;
}

int send_mouse_move(PARAM_CONTROL *pc,int deltax,int deltay,int lmb,int mmb,int rmb,int shift,int ctrl)
{
	int result=FALSE;
	if(pc==0 || pc->control.data==0)
		return result;
	switch(pc->control.type){
	case CEDITFLOAT:
		{
			EDITFLOAT *e=pc->control.data;
			if(e){
				float dx=deltax;
				if(e->fdata){
					float scale=.1;
					if(shift)
						scale=10;
					if(ctrl)
						scale=.01;
					if(shift && ctrl)
						scale=1;
					if(rmb)
						*e->fdata=0;
					else
						(*e->fdata)+=(((float)dx)*scale);
					type_edit_modify("%.4f",e->str,sizeof(e->str),&e->cursor,ETYPE_FLOAT,e->fdata,FALSE);
				}
				e->changed=FALSE;
			}

		}
		break;
	case CEDITBYTE:
		{
			EDITBYTE *e=pc->control.data;
			if(e){
				if(e->byte){
					int scale=1;
					if(shift)
						scale=10;
					if(ctrl)
						scale=2;
					if(shift && ctrl)
						scale=4;
					if(rmb)
						*e->byte=0;
					else{
						int i=*e->byte;
						i+=(deltax*scale);
						if(i<0)
							*e->byte=0;
						else if(i>255)
							*e->byte=255;
						else
							*e->byte=i;
					}
					type_edit_modify("%i",e->str,sizeof(e->str),&e->cursor,ETYPE_BYTE,e->byte,FALSE);
				}
				e->changed=FALSE;
			}
		}
		break;
	case CEDITINT:
		{
			EDITINT *e=pc->control.data;
			if(e){
				if(e->integer){
					int scale=1;
					if(shift)
						scale=10;
					if(ctrl)
						scale=2;
					if(shift && ctrl)
						scale=4;
					if(rmb)
						*e->integer=0;
					else{
						int i=*e->integer;
						i+=(deltax*scale);
						*e->integer=i;
					}
					type_edit_modify("%i",e->str,sizeof(e->str),&e->cursor,ETYPE_INT,e->integer,FALSE);
				}
				e->changed=FALSE;
			}
		}
		break;
	}
	return result;
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
#include "ui_params.h"

DWORD WINAPI page_ui_thread(void *arg)
{
	MSG msg={0};
	while(GetMessage(&msg,NULL,0,0)){
		extern SCREEN scpage;
		extern HWND ghpage;
		page_win_message(&scpage,ghpage,msg.message,msg.wParam,msg.lParam);
	}
}
DWORD WINAPI param_ui_thread(void *arg)
{
	MSG msg={0};
	while(GetMessage(&msg,NULL,0,0)){
		extern SCREEN scparams;
		extern HWND ghparams;
		param_win_message(&scparams,ghparams,msg.message,msg.wParam,msg.lParam);
	}
}

DWORD WINAPI subparam_ui_thread(void *arg)
{
	MSG msg={0};
	while(GetMessage(&msg,NULL,0,0)){
		extern SCREEN scsubparams;
		extern HWND ghsubparams;
		subparam_win_message(&scsubparams,ghsubparams,msg.message,msg.wParam,msg.lParam);
	}
}

