int remove_param(PARAM_LIST *pl,PARAM_CONTROL *pc)
{
	extern CRITICAL_SECTION mutex;
	if(pl==0 || pc==0)
		return FALSE;
	EnterCriticalSection(&mutex);
	if(pc->prev==0){
		pl->list=pc->next;
	}
	else{
		PARAM_CONTROL *prev,*next;
		prev=pc->prev;
		next=pc->next;
		if(prev)
			prev->next=next;
		if(next)
			next->prev=prev;
	}
	if(pc->control.data)
		free(pc->control.data);
	free(pc);
	LeaveCriticalSection(&mutex);
	return TRUE;
}
int clear_params(PARAM_LIST *pl)
{
	extern CRITICAL_SECTION mutex;
	PARAM_CONTROL *p;
	if(pl==0)
		return FALSE;
	p=pl->list;
	EnterCriticalSection(&mutex);
	while(p){
		PARAM_CONTROL *tmp;
		if(p->control.data){
			switch(p->control.type){
			case CPOPUPLIST:
				break;
			case CSPLINE:
				{
					SPLINE_CONTROL *sc=p->control.data;
					if(sc){
						SPLINE_KEY_CONTROL *list=sc->keys;
						while(list){
							SPLINE_KEY_CONTROL *next=list->next;
							free(list);
							list=next;
						}
					}
				}
				break;
			}
			free(p->control.data);
			p->control.data=0;
		}
		tmp=p;
		p=p->next;
		free(tmp);
	}
	pl->list=0;
	LeaveCriticalSection(&mutex);
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
	case CEDITBYTE: size=sizeof(EDITBYTE);break;
	case CEDITINT: size=sizeof(EDITINT);break;
	case CEDITFLOAT: size=sizeof(EDITFLOAT);break;
	case CSTATIC: size=sizeof(STATICTEXT);break;
	case CEDIT: size=sizeof(EDITBOX);break;
	case CRECT: size=sizeof(RECTANGLE);break;
	case CBUTTON: size=sizeof(BUTTON);break;
	case CDROPLIST: size=sizeof(DROPLIST);break;
	case CPOPUPLIST: size=sizeof(POPUPLIST);break;
	case CSPLINE: size=sizeof(SPLINE_CONTROL);break;
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
int process_param_list(struct PCLIST *pclist,int list_count,PARAM_LIST *pl)
{
	int result=FALSE;
	int i,xpos=0,ypos=0;
	for(i=0;i<list_count;i++){
		PARAM_CONTROL *pc=malloc(sizeof(PARAM_CONTROL));
		if(pc){
			memset(pc,0,sizeof(PARAM_CONTROL));
			if(create_param_control(pclist[i].type,pc)){
				switch(pclist[i].type){
				case CRECT:
					{
						RECTANGLE *c=pc->control.data;
						if(c){
							unsigned char *b=pclist[i].data;
							c->x=pclist[i].x+xpos;
							c->y=pclist[i].y+ypos;
							c->w=pclist[i].w;
							c->h=pclist[i].h;
							if(b){
								//c->color=(b[0]<<16)|(b[1]<<8)|(b[2]);
								c->r=b;
								c->g=b+1;
								c->b=b+2;
								c->filled=TRUE;
							}
							result=TRUE;
						}
					}
					break;
				case CEDITBYTE:
					{
						EDITBYTE *c=pc->control.data;
						if(c){
							unsigned char *b=pclist[i].data;
							c->x=pclist[i].x+xpos;
							c->y=pclist[i].y+ypos;
							c->w=pclist[i].w;
							c->h=pclist[i].h;
							if(b){
								_snprintf(c->str,sizeof(c->str),"%i",*b);
								c->byte=b;
							}
							result=TRUE;
						}
					}
					break;
				case CEDITINT:
					{
						EDITINT *c=pc->control.data;
						if(c){
							int *idata=pclist[i].data;
							c->x=pclist[i].x+xpos;
							c->y=pclist[i].y+ypos;
							c->w=pclist[i].w;
							c->h=pclist[i].h;
							if(idata){
								_snprintf(c->str,sizeof(c->str),"%i",*idata);
								c->integer=idata;
							}
							result=TRUE;
						}
					}
					break;
				case CEDITFLOAT:
					{
						EDITFLOAT *c=pc->control.data;
						if(c){
							float *f=pclist[i].data;
							c->x=pclist[i].x+xpos;
							c->y=pclist[i].y+ypos;
							c->w=pclist[i].w;
							c->h=pclist[i].h;
							if(f){
								_snprintf(c->str,sizeof(c->str),"%.4f",*f);
								c->fdata=f;
							}
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
							c->str=pclist[i].data;
							result=TRUE;
						}
					}
					break;
				case CEDIT:
					{
						EDITBOX *c=pc->control.data;
						if(c){
							c->x=pclist[i].x+xpos;
							c->y=pclist[i].y+ypos;
							c->w=pclist[i].w;
							c->h=pclist[i].h;
							c->str=pclist[i].data;
							c->maxlen=pclist[i].data_ex;
							result=TRUE;
						}
					}
					break;
				case CBUTTON:
					{
						BUTTON *b=pc->control.data;
						if(b){
							b->x=pclist[i].x+xpos;
							b->y=pclist[i].y+ypos;
							b->w=pclist[i].w;
							b->h=pclist[i].h;
							b->text=pclist[i].data;
							b->id=pclist[i].data_ex;
							result=TRUE;
						}
					}
					break;
				case CDROPLIST:
					{
						DROPLIST *dl=pc->control.data;
						if(dl){
							dl->x=pclist[i].x+xpos;
							dl->y=pclist[i].y+ypos;
							dl->w=pclist[i].w;
							dl->h=pclist[i].h;
							dl->list=pclist[i].data;
							dl->current=pclist[i].data_ex;
							result=TRUE;
						}
					}
					break;
				case CSPLINE:
					{
						SPLINE_CONTROL *s=pc->control.data;
						if(s){
							s->x=pclist[i].x+xpos;
							s->y=pclist[i].y+ypos;
							s->w=pclist[i].w;
							s->h=pclist[i].h;
							s->anim=pclist[i].data;
							s->count=pclist[i].data_ex;
							result=TRUE;
						}
					}
					break;
				}
			}
			ypos+=pclist[i].incypos;
			if(pclist[i].incypos)
				xpos=0;
			else
				xpos+=pclist[i].x+pclist[i].w;

		}
		if((!result) && pc)
			free(pc);
		if(result){
			result=add_param_control(pl,pc);
		}

	}
	return result;
}
int create_op_params(OP *o)
{
	extern PARAM_LIST param_list;
	int result=FALSE;
	clear_params(&param_list);
	if(o==0)
		return result;
	else{
		PARAM_LIST *pl=&param_list;
		pl->ref=o;
		switch(o->type){
		case TCUBE:
			{
				struct PCLIST pclist[]={
					{CSTATIC,   8,       8,   0, 20,"type - cube",0,30},
					{CSTATIC,   8,       0, 8*4, 20,"name",       0,0},
					{CEDIT,     8,       0,8*40, 20,NULL,         1,30},
					{CSTATIC,   8,       0, 8*9, 20,"tesselate",  0,0},
					{CEDITFLOAT,8,    0,10*8,20,NULL,         2,0},
					{CEDITFLOAT,2,    0,10*8,20,NULL,         2,0},
					{CEDITFLOAT,2,    0,10*8,20,NULL,         2,30},
				};
				CUBE_DATA *cube=o->data;
				if(cube){
					void *plist[5]={o->name,sizeof(o->name),&cube->tessx,&cube->tessy,&cube->tessz};
					int i,index=0;
					for(i=0;i<sizeof(pclist)/sizeof(struct PCLIST);i++){
						if(pclist[i].data_ex==1){
							pclist[i].data=plist[index++];
							pclist[i].data_ex=plist[index++];
						}
						else if(pclist[i].data_ex==2)
							pclist[i].data=plist[index++];
					}
					process_param_list(&pclist,sizeof(pclist)/sizeof(struct PCLIST),pl);
				}
			}
			break;
		case TMULTIPLY:
			{
				struct PCLIST pclist[]={
					{CSTATIC,   8,  0,40,  20,"type - multiply",0,30},
					{CSTATIC,   8,  0,8*4, 20,"name",0,0},
					{CEDIT,     8,  0,8*40,20,NULL,1,30},
					{CSTATIC,   8,  0,8*9, 20,"scale",0,0},
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
					{CSTATIC,   8,  0,8*9, 20,"count",0,0},
					{CEDITBYTE, 8,  0,10*8,20,NULL,2,30},
				};
				MULTIPLY_DATA *mult=o->data;
				if(mult){
					void *plist[12]={o->name,sizeof(o->name),
							&mult->scalex,&mult->scaley,&mult->scalez,
							&mult->rotx,&mult->roty,&mult->rotz,
							&mult->transx,&mult->transy,&mult->transz,
							&mult->count,
						};
					int i,index=0;
					for(i=0;i<sizeof(pclist)/sizeof(struct PCLIST);i++){
						if(pclist[i].data_ex==1){
							pclist[i].data=plist[index++];
							pclist[i].data_ex=plist[index++];
						}
						else if(pclist[i].data_ex==2)
							pclist[i].data=plist[index++];
					}
					process_param_list(&pclist,sizeof(pclist)/sizeof(struct PCLIST),pl);
				}
			}
			break;
		case TTRANSFORM:
			{
				struct PCLIST pclist[]={
					{CSTATIC,   8,  0,40,  20,"type - transform",0,30},
					{CSTATIC,   8,  0,8*4, 20,"name",0,0},
					{CEDIT,     8,  0,8*40,20,NULL,1,30},
					{CSTATIC,   8,  0,8*9, 20,"scale",0,0},
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
					{CBUTTON,   4,  0,10*8,25,NULL,1,30},
				};
				TRANSFORM_DATA *t=o->data;
				if(t){
					void *plist[17]={o->name,sizeof(o->name),
							&t->scalex,&t->scaley,&t->scalez,
							&t->rotx,&t->roty,&t->rotz,
							&t->transx,&t->transy,&t->transz,"Animate",0,
						};
					int i,index=0;
					for(i=0;i<sizeof(pclist)/sizeof(struct PCLIST);i++){
						if(pclist[i].data_ex==1){
							pclist[i].data=plist[index++];
							pclist[i].data_ex=plist[index++];
						}
						else if(pclist[i].data_ex==2)
							pclist[i].data=plist[index++];
					}
					process_param_list(&pclist,sizeof(pclist)/sizeof(struct PCLIST),pl);
				}
			}
			break;
		case TLIGHT:
			{
				struct PCLIST pclist[]={
					{CSTATIC,   8,  0,40,  20,"type - light",0,30},
					{CSTATIC,   8,  0,8*4, 20,"name",0,0},
					{CEDIT,     8,  0,8*40,20,NULL,1,30},
					{CSTATIC,   8,  0,8*9, 20,"light #",0,0},
					{CEDITBYTE, 8,  0,10*8,20,NULL,2,30},
					{CSTATIC,   8,  0,8*9, 20,"ambient",0,0},
					{CEDITBYTE, 8,  0,10*8,20,NULL,2,0},
					{CEDITBYTE, 2,  0,10*8,20,NULL,2,0},
					{CEDITBYTE, 2,  0,10*8,20,NULL,2,0},
					{CRECT,     2,  0,20,20,NULL,2,30},
					{CSTATIC,   8,  0,8*9,20,"diffuse",0,0},
					{CEDITBYTE, 8,  0,10*8,20,NULL,2,0},
					{CEDITBYTE, 2,  0,10*8,20,NULL,2,0},
					{CEDITBYTE, 2,  0,10*8,20,NULL,2,0},
					{CRECT,     2,  0,20,20,NULL,2,30},
					{CSTATIC,   8,  0,8*9,20,"specular",0,0},
					{CEDITBYTE, 8,  0,10*8,20,NULL,2,0},
					{CEDITBYTE, 2,  0,10*8,20,NULL,2,0},
					{CEDITBYTE, 2,  0,10*8,20,NULL,2,0},
					{CRECT,     2,  0,20,20,NULL,2,30},
					{CSTATIC,   8,  0,8*9,20,"position",0,0},
					{CEDITFLOAT,8,  0,10*8,20,NULL,2,0},
					{CEDITFLOAT,2,  0,10*8,20,NULL,2,0},
					{CEDITFLOAT,2,  0,10*8,20,NULL,2,30},
					{CSTATIC,   8,  0,8*9,20,"positionw",0,0},
					{CEDITFLOAT,8,  0,10*8,20,NULL,2,30},
					{CSTATIC,   8,  0,8*9,20,"exponent",0,0},
					{CEDITFLOAT,8,  0,10*8,20,NULL,2,30},
					{CSTATIC,   8,  0,8*9,20,"cutoff",0,0},
					{CEDITFLOAT,8,  0,10*8,20,NULL,2,30},
					{CSTATIC,   8,  0,8*12,20,"attenuation",0,0},
					{CEDITINT,  8,  0,10*8,20,NULL,2,30},
				};
				LIGHT_DATA *light=o->data;
				if(light){
					void *plist[22]={
						o->name,sizeof(o->name),
						&light->light_num,
						&light->r_ambient,
						&light->g_ambient,
						&light->b_ambient,
						&light->r_ambient,
						&light->r_diffuse,
						&light->g_diffuse,
						&light->b_diffuse,
						&light->r_diffuse,
						&light->r_specular,
						&light->g_specular,
						&light->b_specular,
						&light->r_specular, 
						&light->posx,
						&light->posy,
						&light->posz,
						&light->posw,
						&light->exponent,
						&light->cuttoff,
						&light->attenuation
					};
					int i,index=0;
					for(i=0;i<sizeof(pclist)/sizeof(struct PCLIST);i++){
						if(pclist[i].data_ex==1){
							pclist[i].data=plist[index++];
							pclist[i].data_ex=plist[index++];
						}
						else if(pclist[i].data_ex==2)
							pclist[i].data=plist[index++];
					}
					if(index==22)
						index=index;
					process_param_list(&pclist,sizeof(pclist)/sizeof(struct PCLIST),pl);
				}
			}
			break;
		}
	}
}

int param_win_message(SCREEN *sc,HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static int clickx=0,clicky=0,debounce=0;
	static PARAM_CONTROL *pcdrag=0;
	PARAM_CONTROL *p;
	extern PARAM_LIST param_list;
	p=param_list.list;
	switch(msg){
	case WM_MOUSEWHEEL:
		{
			POINT point;
			PARAM_CONTROL *pc=0;
			point.x=LOWORD(lparam);
			point.y=HIWORD(lparam);
			MapWindowPoints(NULL,hwnd,&point,1);
			point.x+=param_list.si.hscroll;
			point.y+=param_list.si.vscroll;
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
				scroll_view(hwnd,sc,&param_list.si,amount,control);
			}
		}
		break;
	case WM_MOUSEMOVE:
		set_focus(hwnd);
		if(pcdrag){
			int x,y,deltax,deltay;
			int lmb,mmb,rmb,ctrl,shift;
			set_focus(hwnd);
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
			if(lmb==0 && mmb==0 && rmb==0)
				pcdrag=0;
			else
				send_mouse_move(pcdrag,deltax,deltay,lmb,mmb,rmb,shift,ctrl);
			clickx=x;
			clicky=y;
		}
		else{
			int x,y;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			if(!(wparam&MK_LBUTTON)){
				param_list.si.vscroll_pressed=FALSE;
				param_list.si.hscroll_pressed=FALSE;
			}
			if(param_list.si.vscroll_pressed){
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
				param_list.si.vscroll+=(int)delta;
				clicky=y;
			}
			if(param_list.si.hscroll_pressed){
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
				param_list.si.hscroll+=(int)delta;
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
		pcdrag=0;
		param_list.si.hscroll_pressed=0;
		param_list.si.vscroll_pressed=0;
		break;
	case WM_LBUTTONDOWN:
		{
			int x,y;
			PARAM_CONTROL *pc=0;
			clickx=x=LOWORD(lparam);
			clicky=y=HIWORD(lparam);
			x+=param_list.si.hscroll;
			y+=param_list.si.vscroll;
			debounce=0;
			if(check_scroll_hit(sc,&param_list.si,hwnd,x,y))
				break;
			clear_param_selected(p);

			if(hittest_param(p,x,y,&pc)){
				pc->has_focus=TRUE;
				if(pc->control.type==CBUTTON){
					BUTTON *b=pc->control.data;
					if(b){
						b->pressed=TRUE;
						create_subparams(param_list.ref,pc);
					}
				}
				pcdrag=pc;
			}
			else
				pcdrag=0;
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
