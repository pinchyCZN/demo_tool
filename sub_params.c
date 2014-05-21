#define _WIN32_WINNT 0x400
#include <windows.h>
#include <math.h>
#include "widgets.h"


int create_subpcontrols(OP *o)
{
	extern PARAM_LIST subparam_list;
	int result=FALSE;
	clear_params(&subparam_list);
	if(o==0)
		return result;
	else{
		PARAM_LIST *pl=&subparam_list;
		pl->ref=o;
		switch(o->type){
		case TTRANSFORM:
			{

				struct PCLIST pclist[]={
					{CSTATIC,   8,  0,8*10,20,"animtype",0,0},
					{CDROPLIST, 8,  0,8*20,20,"linear\nspline\nother\n",0,30},
					{CSTATIC,   8,  0,8*9,20,"scale",0,0},
					{CEDITFLOAT,8,  0,10*8,20,NULL,2,0},
					{CEDITFLOAT,2,  0,10*8,20,NULL,2,0},
					{CEDITFLOAT,2,  0,10*8,20,NULL,2,30},
					{CSTATIC,   8,  0,8*9,20,"rotate",0,0},
					{CEDITFLOAT,8,  0,10*8,20,NULL,2,0},
					{CEDITFLOAT,2,  0,10*8,20,NULL,2,0},
					{CEDITFLOAT,2,  0,10*8,20,NULL,2,30},
					{CSTATIC,   8,  0,8*9,20,"translate",0,0},
					{CEDITFLOAT,8,  0,10*8,20,NULL,2,0},
					{CEDITFLOAT,2,  0,10*8,20,NULL,2,0},
					{CEDITFLOAT,2,  0,10*8,20,NULL,2,30},
					{CBUTTON,   8,  0,14*8,25,"Spline editor",0,30},
				};
				TRANSFORM_DATA *t=o->data;
				if(t){
					int i,index=0;
					for(i=0;i<sizeof(pclist)/sizeof(struct PCLIST);i++){
						if(pclist[i].data_ex==2)
							pclist[i].data=&t->anim[index++].amount;
					}
					process_param_list(&pclist,sizeof(pclist)/sizeof(struct PCLIST),pl);
				}
			}
			break;
		}
	}


}
int create_subparams(OP *op,PARAM_CONTROL *pc)
{
	if(op==0 || pc==0)
		return FALSE;
	if(pc->control.type==CBUTTON){
		switch(op->type){
		case TTRANSFORM:
			{
				create_subpcontrols(op);
			}
			break;
		}
	}
}
int get_droplist_count(char *list)
{
	int result=0;
	if(list!=0){
		int i=0;
		int wordstart=TRUE;
		char c;
		while((c=list[i++])!=0){
			if(c!=0 && c!='\n'){
				if(wordstart){
					result++;
					wordstart=FALSE;
				}
			}
			else
				wordstart=TRUE;
		}
	}
	return result;
}
int handle_drop_list(PARAM_LIST *plist,PARAM_CONTROL *pc)
{
	int result=FALSE;
	if(plist && pc){
		DROPLIST *dl=pc->control.data;
		if(dl->dropped==FALSE && dl->child==0){
			PARAM_CONTROL *pcpopup;
			pcpopup=malloc(sizeof(PARAM_CONTROL));
			if(pcpopup){
				memset(pcpopup,0,sizeof(PARAM_CONTROL));
				dl->dropped=TRUE;
				create_param_control(CPOPUPLIST,pcpopup);
				if(pcpopup->control.data){
					POPUPLIST *popup;
					pcpopup->has_focus=TRUE;
					popup=pcpopup->control.data;
					if(popup){
						int count=get_droplist_count(dl->list);
						if(count==0)
							count=1;
						popup->x=dl->x;
						popup->y=dl->y+dl->h;
						popup->w=dl->w;
						popup->h=dl->h*count;
						popup->count=count;
						popup->list=dl->list;
						popup->parent=pc;
						dl->child=pcpopup;
						add_param_control(plist,pcpopup);
						result=TRUE;
					}
				}
				if(!result){
					free(pcpopup);
					pcpopup=0;
				}
			}
		}
		else if(dl->child!=0){
			if(remove_param(plist,dl->child)){
				dl->child=0;
				dl->dropped=FALSE;
				result=TRUE;
			}
		}
	}
	return result;
}
int handle_popup_list(PARAM_LIST *plist,PARAM_CONTROL *pc,int y)
{
	int result=FALSE;
	if(plist && pc){
		POPUPLIST *pu=pc->control.data;
		PARAM_CONTROL *parent=0;
		int selected=-1;
		if(pu){
			int ypos=y-pu->y;
			parent=pu->parent;
			if(ypos>=0 && ypos<=pu->h){
				if(pu->count!=0){
					int h=pu->h/pu->count;
					if(h!=0){
						int pos=ypos/h;
						if(pos<pu->count)
							selected=pos;
					}
				}
			}
		}
		if(remove_param(plist,pc)){
			if(parent){
				DROPLIST *dl=parent->control.data;
				if(dl){
					dl->child=0;
					dl->dropped=FALSE;
					if(selected>=0)
						dl->current=selected;
					result=TRUE;
				}
				parent->has_focus=TRUE;
			}
		}
	}
	return result;
}
int find_param_type(PARAM_LIST *plist,int type,PARAM_CONTROL **pc)
{
	int result=FALSE;
	if(plist){
		PARAM_CONTROL *list=plist->list;
		while(list){
			if(list->control.type==type){
				if(pc)
					*pc=list;
				result=TRUE;
				break;
			}
			list=list->next;
		}
	}
	return result;
}
int remove_popup(PARAM_LIST *plist)
{
	int result=FALSE;
	if(plist){
		PARAM_CONTROL *pc=0;
		if(find_param_type(plist,CPOPUPLIST,&pc)){
			result=handle_popup_list(plist,pc,-1);
		}
	}
	return result;
}
int subparam_win_message(SCREEN *sc,HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int clickx=0,clicky=0,debounce=0;
	static PARAM_CONTROL *pcdrag=0;
	PARAM_CONTROL *p;
	extern PARAM_LIST subparam_list;
	p=subparam_list.list;
	if(p==0)
		return FALSE;
	switch(msg){
	case WM_MOUSEWHEEL:
		{
			POINT point;
			PARAM_CONTROL *pc=0;
			point.x=LOWORD(lparam);
			point.y=HIWORD(lparam);
			MapWindowPoints(NULL,hwnd,&point,1);
			if(hittest_param(p,point.x,point.y,&pc)){
				if(pc->has_focus){
					signed short deltax=(HIWORD(wparam));
					int lmb,mmb,rmb,ctrl,shift;
					lmb=wparam&MK_LBUTTON;
					mmb=wparam&MK_MBUTTON;
					rmb=wparam&MK_RBUTTON;
					shift=wparam&MK_SHIFT;
					ctrl=wparam&MK_CONTROL;
					deltax>>=4;
					send_mouse_move(pc,deltax,0,lmb,mmb,rmb,shift,ctrl);
				}
			}
		}
		break;
	case WM_MOUSEMOVE:
		if(pcdrag){
			int x,y,deltax,deltay;
			int lmb,mmb,rmb,ctrl,shift;
			lmb=wparam&MK_LBUTTON;
			mmb=wparam&MK_MBUTTON;
			rmb=wparam&MK_RBUTTON;
			shift=wparam&MK_SHIFT;
			ctrl=wparam&MK_CONTROL;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			deltax=x-clickx;
			deltay=y-clicky;
			if(debounce==0){
				int size=4;
				if((deltax<-size) || (deltax>size))
					debounce=1;
				if((deltay<-size) || (deltay>size))
					debounce=1;
				if(debounce==0)
					break;
				else{
					if(deltax<0)
						deltax+=size;
					else
						deltax-=size;
					if(deltay<0)
						deltay+=size;
					else
						deltay-=size;
				}
			}
			send_mouse_move(pcdrag,deltax,deltay,lmb,mmb,rmb,shift,ctrl);
			clickx=x;
			clicky=y;
		}
		break;
	case WM_LBUTTONUP:
		if(pcdrag && pcdrag->control.type==CBUTTON){
			BUTTON *b=pcdrag->control.data;
			if(b)
				b->pressed=FALSE;
			if(subparam_list.ref && subparam_list.ref->type==TTRANSFORM){
				//set_page_mode(TRUE);
			}
		}
		pcdrag=0;
		break;
	case WM_LBUTTONDOWN:
		{
			int x,y;
			PARAM_CONTROL *pc=0;
			clickx=x=LOWORD(lparam);
			clicky=y=HIWORD(lparam);
			debounce=0;
			clear_param_selected(p);
			if(hittest_param(p,x,y,&pc)){
				int list_handled=FALSE;
				if(pc->control.type==CDROPLIST){
					DROPLIST *dl=pc->control.data;
					handle_drop_list(&subparam_list,pc);
					list_handled=TRUE;
				}
				else if(pc->control.type==CPOPUPLIST){
					handle_popup_list(&subparam_list,pc,y);
					list_handled=TRUE;
				}
				else if(pc->control.type==CBUTTON){
					BUTTON *b=pc->control.data;
					if(b)
						b->pressed=TRUE;
				}
				if(!list_handled)
					remove_popup(&subparam_list);
				pc->has_focus=TRUE;
				pcdrag=pc;
			}
			else{
				pcdrag=0;
				remove_popup(&subparam_list);
			}
		}
		break;
	case WM_KEYFIRST:
		{
			PARAM_CONTROL *list=p;
			while(list){
				if(list->has_focus){
					int key,vkey,ctrl,shift;
					key=MapVirtualKey(wparam,2)&0xFF;
					vkey=wparam;
					ctrl=GetKeyState(VK_CONTROL)&0x8000;
					shift=GetKeyState(VK_SHIFT)&0x8000;
					if(vkey==VK_TAB){
						PARAM_CONTROL *tmp=list;
						if(shift){
							tmp=list->prev;
							while(tmp && (!((tmp->control.type>=CEDIT) && (tmp->control.type<=CEDITFLOAT))))
								tmp=tmp->prev;
						}
						else{
							tmp=list->next;
							while(tmp && (!((tmp->control.type>=CEDIT) && (tmp->control.type<=CEDITFLOAT))))
								tmp=tmp->next;
						}
						if(tmp){
							tmp->has_focus=TRUE;
							list->has_focus=FALSE;
							break;
						}

					}
					send_char_control(&list->control,key,vkey,ctrl,shift);
				}
				list=list->next;
			}
		}
		break;
	}
}
