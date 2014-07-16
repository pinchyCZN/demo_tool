#define _WIN32_WINNT 0x400
#include <windows.h>
#include <math.h>
#include "widgets.h"
extern CRITICAL_SECTION mutex;
int alloc_key(SPLINE_KEY **sk)
{
	int result=FALSE;
	if(sk){
		*sk=malloc(sizeof(SPLINE_KEY));
		if(*sk){
			memset(*sk,0,sizeof(SPLINE_KEY));
			result=TRUE;
		}
	}
	return result;
}
int insert_keylist(ANIMATE_DATA *a,SPLINE_KEY *sk)
{
	int result=FALSE;
	if(a){
		if(a->key==0)
			a->key=sk;
		else{
			SPLINE_KEY *list=a->key;
			while(list){
				if(list->time<=sk->time){
					SPLINE_KEY *next;
					next=list->next;
					if((next && (next->time>=sk->time)) || next==0){
						list->next=sk;
						sk->prev=list;
						if(next)
							next->prev=sk;
						sk->next=next;
						result=TRUE;
						break;
					}
				}
				list=list->next;
			}
			if(!result){
				SPLINE_KEY *head;
				head=a->key;
				a->key=sk;
				head->prev=sk;
				sk->next=head;
			}
		}
	}
	return result;
}
int add_splinekey_control(SPLINE_CONTROL *sc,SPLINE_KEY *k,SPLINE_KEY_CONTROL **skc)
{
	int result=FALSE;
	if(sc && k){
		*skc=malloc(sizeof(SPLINE_KEY_CONTROL));
		if(*skc){
			memset(*skc,0,sizeof(SPLINE_KEY_CONTROL));
			(*skc)->w=10;
			(*skc)->h=10;
			(*skc)->key=k;
			if(sc->keys==0){
				sc->keys=*skc;
				result=TRUE;
			}else{
				SPLINE_KEY_CONTROL *list=sc->keys;
				while(list){
					if(list->next==0){
						list->next=*skc;
						(*skc)->prev=list;
						result=TRUE;
						break;
					}
					list=list->next;
				}
			}
		}
	}
	return result;
}
int init_skc_pos(SPLINE_CONTROL *sc,SPLINE_KEY_CONTROL *skc)
{
	if(sc && skc){
		SPLINE_KEY *key;
		key=skc->key;
		if(key){
			float zx,zy;
			zx=sc->zoomx;
			zy=sc->zoomy;
			key->time=skc->x - sc->x;
			key->val=skc->y - sc->y - (sc->h/2);
		}
	}
	return TRUE;
}
int add_splinekey(PARAM_CONTROL *pc,SPLINE_KEY **nsk,int x,int y)
{
	int result=FALSE;
	EnterCriticalSection(&mutex);
	if(pc){
		if(pc->control.type==CSPLINE){
			SPLINE_CONTROL *sc=pc->control.data;
			if(sc){
				int i=sc->selected;
				ANIMATE_DATA *a=sc->anim;
				if(a){
					SPLINE_KEY *sk=0;
					alloc_key(&sk);
					if(sk){
						SPLINE_KEY_CONTROL *skc=0;
						add_splinekey_control(sc,sk,&skc);
						if(skc){
							skc->x=x;
							skc->y=y;
							init_skc_pos(sc,skc);
						}
						insert_keylist(&a[i],sk);
						if(nsk)
							*nsk=sk;

						result=TRUE;
					}
				}
			}
		}
	}
	LeaveCriticalSection(&mutex);
	return result;
}

int del_spline_key(SPLINE_KEY *key,ANIMATE_DATA *a)
{
	int result=FALSE;
	if(key && a){
		SPLINE_KEY *list=a->key;
		while(list){
			SPLINE_KEY *prev,*next;
			prev=list->prev;
			next=list->next;
			if(list==key){
				if(prev==0)
					a->key=next;
				else
					prev->next=next;
				if(next)
					next->prev=prev;
				free(list);
				result=TRUE;
				break;
			}
			list=next;
		}

	}
	return result;
}
int del_sel_keys(PARAM_CONTROL *pc)
{
	int result=FALSE;
	EnterCriticalSection(&mutex);
	if(pc){
		if(pc->control.type==CSPLINE){
			SPLINE_CONTROL *sc=pc->control.data;
			if(sc){
				SPLINE_KEY_CONTROL *keys=sc->keys;
				while(keys){
					SPLINE_KEY_CONTROL *prev,*next;
					prev=keys->prev;
					next=keys->next;
					if(keys->selected){
						if(prev==0)
							sc->keys=next;
						else
							prev->next=next;
						if(next)
							next->prev=prev;
						del_spline_key(keys->key,sc->anim);
						free(keys);
						result=TRUE;
					}
					keys=next;
				}
			}
		}
	}
	LeaveCriticalSection(&mutex);
	return result;
}

int handle_spline_click(SPLINE_CONTROL *sc,int x,int y,SPLINE_KEY_CONTROL **result,int keypress)
{
	SPLINE_KEY_CONTROL *skc;
	if(sc==0)
		return TRUE;
	if(result)
		*result=0;
	skc=sc->keys;
	if(skc){
		SPLINE_KEY_CONTROL *list=skc,*first=0,*selected=0,*next=0;
		int count=0;
		while(list){
			if(x>=list->x && x<(list->x+list->w) && y>=list->y && y<(list->y+list->h)){
				if(first==0)
					first=list;
				if(list->selected)
					selected=list;
				else if(selected && next==0)
					next=list;
				count++;
			}else{
				if((keypress&MK_CONTROL)==0)
					list->selected=FALSE;
			}
			list=list->next;
		}
		if(first){
			SPLINE_KEY_CONTROL *s=0;
			if(count==1){
				s=first;
			}
			else{
				if(next)
					s=next;
				else
					s=first;
			}
			if(selected){
				if((keypress&MK_CONTROL)==0)
					selected->selected=FALSE;
			}
			if(result)
				*result=s;
			if(s)
				s->selected=TRUE;
		}
	}
	return TRUE;
}
int handle_spline_key_move(SPLINE_CONTROL *sc,SPLINE_KEY_CONTROL *skc,int dx,int dy)
{
	if(sc && skc){
		int x,y;
		x=skc->x;
		y=skc->y;
		x+=dx;
		y+=dy;
		if(x>=sc->x && x<(sc->x+sc->w) && y>=sc->y && y<(sc->y+sc->h)){
			SPLINE_KEY *sk;
			skc->x=x;
			skc->y=y;
			sk=skc->key;
			if(sk){
				float scalex,scaley;
				scalex=sc->zoomx;
				scaley=sc->zoomy;
				if(scalex==0)
					scalex=1;
				if(scaley==0)
					scaley=1;
				sk->time+=scalex*dx;
				sk->val+=scalex*dy;
			}
		}
	}
	return TRUE;
}
int spline_win_message(SCREEN *sc,HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	extern SPLINE_EDIT spline_edit;
	typedef enum CMDMENU{
		CMD_ADDKEY,CMD_DELKEY
	};
	static int clickx=0,clicky=0,debounce=0;
	static PARAM_CONTROL *pcdrag=0;
	static HMENU hmenu=0;
	static SPLINE_KEY_CONTROL *selected_key=0;
	PARAM_CONTROL *p;

	p=spline_edit.plist.list;
	if(FALSE)
	if(msg!=WM_PAINT&&msg!=WM_SETCURSOR&&msg!=WM_NCHITTEST&&msg!=WM_ENTERIDLE)
	{
		static DWORD tick;
		if((GetTickCount()-tick)>500)
			printf("--\n");
		printf("spl");
		print_msg(msg,lparam,wparam,hwnd);
		tick=GetTickCount();
	}
	switch(msg){
	case WM_CREATE:
		{
			hmenu=CreatePopupMenu();
			if(hmenu){
				InsertMenu(hmenu,0xFFFFFFFF,MF_BYPOSITION,CMD_ADDKEY,"add key");
				InsertMenu(hmenu,0xFFFFFFFF,MF_BYPOSITION,CMD_DELKEY,"delete key");
			}
		}
		break;
	case WM_CONTEXTMENU:
		if(hmenu){
			POINT point;
			int x,y;
			point.x=x=LOWORD(lparam);
			point.y=y=HIWORD(lparam);
			MapWindowPoints(NULL,hwnd,&point,1);
			clickx=x;
			clicky=y;
			point.x+=spline_edit.plist.si.hscroll;
			point.y+=spline_edit.plist.si.vscroll;
			if(hittest_param(p,point.x,point.y,0)){
				TrackPopupMenu(hmenu,TPM_LEFTALIGN,x,y,0,hwnd,NULL);
			}
		}
		break;
	case WM_KEYFIRST:
		switch(wparam){
		case VK_DELETE:
			PostMessage(hwnd,WM_COMMAND,CMD_DELKEY,0);
			break;
		}
		break;
	case WM_COMMAND:
		{
			PARAM_CONTROL *pc=0;
			if(p){
				find_param_type(&spline_edit.plist,CSPLINE,&pc);
				if(pc){
					
				}
			}
			switch(LOWORD(wparam)){
			case CMD_ADDKEY:
				{
					SPLINE_KEY *sk=0;
					POINT p;
					p.x=clickx;
					p.y=clicky;
					MapWindowPoints(NULL,hwnd,&p,1);
					p.x+=spline_edit.plist.si.hscroll;
					p.y+=spline_edit.plist.si.vscroll;

					add_splinekey(pc,&sk,p.x,p.y);
				}
				break;
			case CMD_DELKEY:
				del_sel_keys(pc);
				break;
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{
			POINT point;
			PARAM_CONTROL *pc=0;
			point.x=LOWORD(lparam);
			point.y=HIWORD(lparam);
			MapWindowPoints(NULL,hwnd,&point,1);
			point.x+=spline_edit.plist.si.hscroll;
			point.y+=spline_edit.plist.si.vscroll;
			if(hittest_param(p,point.x,point.y,&pc)
				&& pc->control.type==CDROPLIST){
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
			else{
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
				scroll_view(hwnd,sc,&spline_edit.plist.si,amount,control);
			}
		}
		break;
	case WM_MOUSEMOVE:
		set_focus(hwnd);
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
			if(selected_key && pcdrag->control.type==CSPLINE)
				handle_spline_key_move(pcdrag->control.data,selected_key,deltax,deltay);
			clickx=x;
			clicky=y;
		}
		else{
			int x,y;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			if(!(wparam&MK_LBUTTON)){
				spline_edit.plist.si.vscroll_pressed=FALSE;
				spline_edit.plist.si.hscroll_pressed=FALSE;
			}
			if(spline_edit.plist.si.vscroll_pressed){
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
				spline_edit.plist.si.vscroll+=(int)delta;
				clicky=y;
			}
			if(spline_edit.plist.si.hscroll_pressed){
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
				spline_edit.plist.si.hscroll+=(int)delta;
				clickx=x;
			}
		}
		break;
	case WM_LBUTTONUP:
		if(pcdrag && pcdrag->control.type==CBUTTON){
			BUTTON *b=pcdrag->control.data;
			if(b)
				b->pressed=FALSE;
		}
		spline_edit.plist.si.hscroll_pressed=0;
		spline_edit.plist.si.vscroll_pressed=0;
		pcdrag=0;
		selected_key=0;
		break;
	case WM_LBUTTONDOWN:
		{
			int x,y;
			PARAM_CONTROL *pc=0;
			clickx=x=LOWORD(lparam);
			clicky=y=HIWORD(lparam);
			x+=spline_edit.plist.si.hscroll;
			y+=spline_edit.plist.si.vscroll;
			debounce=0;
			if(check_scroll_hit(sc,&spline_edit.plist.si,hwnd,x,y))
				break;
			clear_param_selected(p);
			if(hittest_param(p,x,y,&pc)){
				int list_handled=FALSE;
				if(pc->control.type==CDROPLIST){
					DROPLIST *dl=pc->control.data;
					handle_drop_list(&spline_edit.plist,pc);
					list_handled=TRUE;
				}
				else if(pc->control.type==CPOPUPLIST){
					handle_popup_list(&spline_edit.plist,pc,y);
					list_handled=TRUE;
				}
				else if(pc->control.type==CBUTTON){
					BUTTON *b=pc->control.data;
					if(b)
						b->pressed=TRUE;
				}
				else if(pc->control.type==CSPLINE){
					handle_spline_click(pc->control.data,x,y,&selected_key,wparam);
				}
				if(!list_handled)
					remove_popup(&spline_edit.plist);
				pc->has_focus=TRUE;
				pcdrag=pc;
			}
			else{
				pcdrag=0;
				remove_popup(&spline_edit.plist);
			}
		}
		break;

	}
	return 0;
}

