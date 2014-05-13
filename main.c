#define _WIN32_WINNT 0x400
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "entity.h"
#include "resource.h"
#include "widgets.h"

HGLRC	hGLRC=0;
HWND	ghwindow=0;

HWND	ghview1=0;
HWND	ghpage=0;
HWND	ghpagelist=0;
HWND	ghparams=0;
SCREEN scpage={0};
SCREEN scpagelist={0};
SCREEN scparams={0};

HWND		ghfocus=0;

HINSTANCE	ghinstance=0;
HACCEL		ghaccel=0;
//int view1_divider=0;
int params_divider=0;
int horiz_divider=0;
int page_divider=0;
RECT gwinrect={0};

int g_draw=0;
int g_screenw=1024;
int g_screenh=758;

float gx=0,gy=0,gz=0;
float grx=0,gry=0,grz=0;

int g_ztri=-100;
int g_tmp=700;

GLfloat ambient[]={1.0,1.0,1.0,0.0};
GLfloat light_position[]={-5.0,-10.0,10.0,0.0};
GLfloat white_light[]={1.0,1.0,1.0,1.0};

ENTITY *players[2];
ENTITY *non_players[100];

CRITICAL_SECTION mutex={0};
DWORD page_ui_threadid=0;
DWORD param_ui_threadid=0;
DWORD view1_ui_threadid=0;
DWORD WINAPI page_ui_thread(void *arg);
DWORD WINAPI param_ui_thread(void *arg);
DWORD WINAPI view1_ui_thread(void *arg);

void gl_init(void)
{
	
	glClearColor(0.0,0.0,0.0,0.0);
//	glShadeModel(GL_FLAT);
//	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
	glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    glMatrixMode(GL_PROJECTION);
    glFrustum(-0.08, 0.08F, -0.06F, 0.06F, 0.1F, 1000.0F);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_DEPTH_TEST);
//		glLightfv(GL_LIGHT0,GL_POSITION,light_position);

/*
    glMatrixMode(GL_PROJECTION);
    glFrustum(-0.5F, 0.5F, -0.5F, 0.5F, 1.0F, 3.0F);

    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0F, 0.0F, -2.0F);

    glRotatef(30.0F, 1.0F, 0.0F, 0.0F);
    glRotatef(30.0F, 0.0F, 1.0F, 0.0F);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
*/
}
int test_triangle()
{
	static float theta=0;
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
//	glLoadIdentity();

//    glTranslatef(0.0F, 0.0F, g_ztri);
//	glRotatef(theta, 0.0f, 1.0f, 1.0f);


	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);   glVertex2f(0.0f,   1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);   glVertex2f(0.87f,  -0.5f);
	glColor3f(0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);
	glEnd();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	theta+=1.0;
}

void display(void)
{

	static float x,y,m,b,h;
	static float position[3],offset[3];
	DWORD t1;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glColor3f(1.0,0.0,0.0);

	glMatrixMode(GL_MODELVIEW);
//	glMatrixMode(GL_PROJECTION);
	
	glLoadIdentity();
	glRotatef(grx,1,0,0);
	glRotatef(gry,0,1,0);
	glRotatef(grz,0,0,1);
	glTranslatef(gx,gy,gz);
	{
		unsigned int tick,global_tick;
		tick=GetTickCount();
		global_tick=get_global_tick();
		update_world(players,sizeof(players)/sizeof(ENTITY*),global_tick,tick);
		global_tick=update_world(non_players,sizeof(non_players)/sizeof(ENTITY*),global_tick,tick);
		set_global_tick(global_tick);
	}
	render_entitys(players,sizeof(players)/sizeof(ENTITY*));
	render_entitys(non_players,sizeof(non_players)/sizeof(ENTITY*));
	get_modifiers();


	test_triangle();


	display_tree();
//	t1=GetTickCount();
//	printf("time=%u v=%f\n",GetTickCount()-t1,bike.v[0]);

	glFlush();
	glFinish();

	{
		static DWORD tick=0,delta;
		tick=GetTickCount();
		delta=GetTickCount()-tick;
		if(delta>0)
			printf("delta=%u\n",delta);
	}
//	bezier();
	/*
	glBindTexture(GL_TEXTURE_2D, 13);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(1.0/256, 1.0/256, 1);
	*/

}
void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;
	fH = tan( fovY / 360 * pi ) * zNear;
	fW = fH * aspect;
	glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}
void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	perspectiveGL(25.0,(GLfloat)w/(GLfloat)h,.1,10000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0,0,(GLsizei)w,(GLsizei)h);

}

void open_console()
{
	char title[MAX_PATH]={0}; 
	HWND hcon; 
	FILE *hf;
	static BYTE consolecreated=FALSE;
	static int hcrt=0;
	
	if(consolecreated==TRUE)
	{
		GetConsoleTitle(title,sizeof(title));
		if(title[0]!=0){
			hcon=FindWindow(NULL,title);
			ShowWindow(hcon,SW_SHOW);
		}
		hcon=(HWND)GetStdHandle(STD_INPUT_HANDLE);
		FlushConsoleInputBuffer(hcon);
		return;
	}
	AllocConsole(); 
	hcrt=_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE),0x4000);

	fflush(stdin);
	hf=_fdopen(hcrt,"w"); 
	*stdout=*hf; 
	setvbuf(stdout,NULL,_IONBF,0);
	GetConsoleTitle(title,sizeof(title));
	if(title[0]!=0){
		hcon=FindWindow(NULL,title);
		ShowWindow(hcon,SW_SHOW); 
		SetForegroundWindow(hcon);
	}
	consolecreated=TRUE;
}
void hide_console()
{
	char title[MAX_PATH]={0}; 
	HANDLE hcon; 
	
	GetConsoleTitle(title,sizeof(title));
	if(title[0]!=0){
		hcon=FindWindow(NULL,title);
		ShowWindow(hcon,SW_HIDE);
		SetForegroundWindow(hcon);
	}
}

int move_console()
{
	char title[MAX_PATH]={0}; 
	HWND hcon; 
	GetConsoleTitle(title,sizeof(title));
	if(title[0]!=0){
		hcon=FindWindow(NULL,title);
		SetWindowPos(hcon,0,820,0,800,600,SWP_NOZORDER);
	}
	return 0;
}
int find_empty_slot(void **array,int count)
{
	int i,index=-1;
	for(i=0;i<count;i++){
		if(array[i]==0){
			index=i;
			break;
		}
	}
	return index;
}
int add_player()
{
	ENTITY *e;
	int i,index;
	index=find_empty_slot(players,sizeof(players)/sizeof(ENTITY*));
	if(index>=0){
		e=malloc(sizeof(ENTITY));
		if(e){
			init_entity(e,PLAYER1);
			players[index]=e;
		}
	}
	return index;
}
int add_bullet(void *owner,int *speed,int *pos)
{
	ENTITY *e;
	int i,index=-1;
	index=find_empty_slot(non_players,sizeof(non_players)/sizeof(ENTITY*));
	if(index>=0){
		e=malloc(sizeof(ENTITY));
		if(e){
			init_entity(e,BULLET1);
			e->owner=owner;
			non_players[index]=e;
			if(speed!=0){
				e->speedx=speed[0];
				e->speedy=speed[1];
				e->speedz=speed[2];
			}
			if(pos!=0){
				e->posx=pos[0];
				e->posy=pos[1];
				e->posz=pos[2];
			}
		}
	}
	return index;
}
int init_entity_array(ENTITY **e,int count)
{
	int i;
	for(i=0;i<count;i++){
		e[i]=0;
	}
	return 0;
}
void idle()
{
	static DWORD tick=0;
	DWORD delta,current=GetTickCount();
	delta=current-tick;
	if(delta>=12){// && g_draw){
		tick=current;
//		display();
		InvalidateRect(ghwindow,0,FALSE);
//		glutPostRedisplay();
		//printf(" delta=%i\n",delta);
	}
	else{
		Sleep(1);
//		printf("delta=%i\n",delta);
	}

}
int worker_thread()
{
	while(TRUE){
		idle();
	}
}
/*
int main(int argc,char **argv)
{

	ghinstance=GetModuleHandle(NULL);
	WinMain(0,0,0,0);
	init_keys();
	init_world();
	init_entity_array(players,sizeof(players)/sizeof(ENTITY*));
	init_entity_array(non_players,sizeof(non_players)/sizeof(ENTITY*));
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowSize(800,500);
	glutInitWindowPosition(0,0);
	glutCreateWindow("test");
	gl_init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(glkey_down);
	glutSpecialFunc(glspecial_down);
	glutKeyboardUpFunc(glkey_up);
	glutSpecialUpFunc(glspecial_up);
	glutIdleFunc(idle);
	move_console();
	add_player();
	glutMainLoop();
	return 0;
}
*/
int setupPixelFormat(HDC hDC)
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  /* size */
        1,                              /* version */
        PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER,               /* support double-buffering */
        PFD_TYPE_RGBA,                  /* color type */
        32,                             /* prefered color depth */
        0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
        0,                              /* no alpha buffer */
        0,                              /* alpha bits (ignored) */
        0,                              /* no accumulation buffer */
        0, 0, 0, 0,                     /* accum bits (ignored) */
        16,                             /* depth buffer */
        0,                              /* no stencil buffer */
        0,                              /* no auxiliary buffers */
        PFD_MAIN_PLANE,                 /* main layer */
        0,                              /* reserved */
        0, 0, 0,                        /* no layer, visible, damage masks */
    };
    int pixelFormat;

    pixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (pixelFormat == 0) {
        MessageBox(WindowFromDC(hDC), "ChoosePixelFormat failed.", "Error",
                MB_ICONERROR | MB_OK);
        exit(1);
    }

    if (SetPixelFormat(hDC, pixelFormat, &pfd) != TRUE) {
        MessageBox(WindowFromDC(hDC), "SetPixelFormat failed.", "Error",
                MB_ICONERROR | MB_OK);
        exit(1);
    }
}
int set_focus(HWND hwnd)
{
	if(ghfocus!=hwnd){
		PostMessage(ghwindow,WM_APP,WM_SETFOCUS,hwnd);
		InvalidateRect(ghwindow,0,TRUE);
	}
	ghfocus=hwnd;
	return TRUE;
}
int print_globs()
{
	printf("%3.2f %3.2f %3.2f , %3.2f %3.2f %3.2f\n",grx,gry,grz,gx,gy,gz);
}
LRESULT CALLBACK win_view1_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static POINT mpoint;
#ifdef _DEBUG
	if(FALSE)
	if(msg!=WM_PAINT&&msg!=WM_SETCURSOR) //msg!=WM_NCHITTEST&&msg!=WM_ENTERIDLE&&
	{
		static DWORD tick;
		if((GetTickCount()-tick)>500)
			printf("--\n");
		printf("v");
		print_msg(msg,lparam,wparam,hwnd);
		tick=GetTickCount();
	}
#endif
	switch(msg){
	case WM_CREATE:
		return 0;
	case WM_SIZE:
		{
			int w,h;
			w=LOWORD(lparam);
			h=HIWORD(lparam);
			g_screenw=w;
			g_screenh=h;
			reshape(w,h);
		}
		break;
	case WM_KEYDOWN:
		switch(wparam){
		case 'R':
			grx=gry=grz=0;
			gx=gy=gz=0;
			print_globs();
			break;
		case VK_ESCAPE:
			exit(0);
			break;
		}
		break;
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		mpoint.x=LOWORD(lparam);
		mpoint.y=HIWORD(lparam);
		SetFocus(hwnd);
		print_globs();
		break;

	case WM_MOUSEMOVE:
		{
			POINT p;
			int deltax,deltay;
			p.x=LOWORD(lparam);
			p.y=HIWORD(lparam);
			deltax=p.x-mpoint.x;
			deltay=p.y-mpoint.y;

			if(wparam&MK_LBUTTON){
				grx+=deltax;
				gry+=deltay;
				print_globs();
			}
			if(wparam&MK_RBUTTON){
				gz+=deltay;
			//	gx+=deltay;
				print_globs();
			}
			mpoint=p;
		}
		set_focus(hwnd);
		break;
	}
	return DefWindowProc(hwnd,msg,wparam,lparam);
}
LRESULT CALLBACK win_params_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
#ifdef _DEBUG
	if(FALSE)
	if(msg!=WM_PAINT&&msg!=WM_SETCURSOR) //msg!=WM_NCHITTEST&&msg!=WM_ENTERIDLE&&
	{
		static DWORD tick;
		if((GetTickCount()-tick)>500)
			printf("--\n");
		printf("p");
		print_msg(msg,lparam,wparam,hwnd);
		tick=GetTickCount();
	}
#endif
	//param_win_message(&scparams,hwnd,msg,wparam,lparam);
	if(param_ui_threadid)
		PostThreadMessage(param_ui_threadid,msg,wparam,lparam);

	switch(msg){
	case WM_CREATE:
		return 0;
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		SetFocus(hwnd);
		break;
	case WM_MOUSEMOVE:
		set_focus(hwnd);
		break;
	}
	return DefWindowProc(hwnd,msg,wparam,lparam);
}
LRESULT CALLBACK win_pagelist_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
#ifdef _DEBUG
	if(FALSE)
	if(msg!=WM_PAINT&&msg!=WM_SETCURSOR) //msg!=WM_NCHITTEST&&msg!=WM_ENTERIDLE&&
	{
		static DWORD tick;
		if((GetTickCount()-tick)>500)
			printf("--\n");
		printf("l");
		print_msg(msg,lparam,wparam,hwnd);
		tick=GetTickCount();
	}
#endif
	switch(msg){
	case WM_CREATE:
		return 0;
	case WM_MOUSEMOVE:
		set_focus(hwnd);
		break;
	}
	return DefWindowProc(hwnd,msg,wparam,lparam);
}
LRESULT CALLBACK win_page_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static lmb_down=FALSE;
	static HMENU hmenu=0;
	typedef enum CMDMENU{
		CMD_DELETE,CMD_LIGHT,CMD_CUBE,CMD_MULTIPLY,CMD_ACTIVATE
	};
#ifdef _DEBUG
	if(FALSE)
	if(msg!=WM_PAINT&&msg!=WM_SETCURSOR&&msg!=WM_NCHITTEST&&msg!=WM_ENTERIDLE&&msg!=WM_MOUSEMOVE)
	{
		static DWORD tick;
		if((GetTickCount()-tick)>500)
			printf("--\n");
		printf("g");
		print_msg(msg,lparam,wparam,hwnd);
		tick=GetTickCount();
	}
#endif
	//page_win_message(&scpage,hwnd,msg,wparam,lparam);
	if(page_ui_threadid)
		PostThreadMessage(page_ui_threadid,msg,wparam,lparam);

	switch(msg){
	case WM_CREATE:
		hmenu=CreatePopupMenu();
		if(hmenu){
			InsertMenu(hmenu,0xFFFFFFFF,MF_BYPOSITION,CMD_LIGHT,"light");
			InsertMenu(hmenu,0xFFFFFFFF,MF_BYPOSITION,CMD_CUBE,"cube");
			InsertMenu(hmenu,0xFFFFFFFF,MF_BYPOSITION,CMD_MULTIPLY,"multiply");
			InsertMenu(hmenu,0xFFFFFFFF,MF_BYPOSITION,CMD_ACTIVATE,"activate");
			InsertMenu(hmenu,0xFFFFFFFF,MF_BYPOSITION|MF_SEPARATOR,0,0);
			InsertMenu(hmenu,0xFFFFFFFF,MF_BYPOSITION,CMD_DELETE,"delete");
		}
		return 0;
	case WM_KEYFIRST:
		if(wparam==0x1b)
			exit(0);
		break;
	case WM_MOUSEMOVE:
		set_focus(hwnd);
		break;
	case WM_CONTEXTMENU:
		if(hmenu){
			int x=LOWORD(lparam),y=HIWORD(lparam);
			TrackPopupMenu(hmenu,TPM_LEFTALIGN,x,y,0,hwnd,NULL);
		}
		break;
	case WM_COMMAND:
		{
			extern PAGE_LIST page_list;
			PAGE_DATA *p;
			OP *op=0;
			int x,y;
			p=page_list.current;
			if(p==0)
				break;
			x=p->cursorx-p->hscroll;
			y=p->cursory-p->vscroll;
			hittest_op(p,x+1,y+1,&op);
			switch(LOWORD(wparam)){
			case CMD_LIGHT:
				if(op==0)
					add_type_op(p,TLIGHT,x,y);
				break;
			case CMD_CUBE:
				if(op==0)
					add_type_op(p,TCUBE,x,y);
				break;
			case CMD_MULTIPLY:
				if(op==0)
					add_type_op(p,TMULTIPLY,x,y);
				break;
			case CMD_DELETE:
				del_op(p,op);
				break;
			case CMD_ACTIVATE:
				break;
			}
		}
		break;
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		SetFocus(hwnd);
		break;
	}
	return DefWindowProc(hwnd,msg,wparam,lparam);
}
int resize_main_window(HWND hwnd)
{
	RECT rect={0};
	int x,y,w,h;
	int line=2;
	GetClientRect(hwnd,&rect);
	if(page_divider<=0)
		page_divider=rect.right/5;
	if(horiz_divider<=0)
		horiz_divider=rect.bottom/2;
//	if(view1_divider<=0)
//		view1_divider=rect.right/2;
	if(params_divider<=0)
		params_divider=rect.right/2;

	x=y=0;
	w=params_divider-line;
	h=horiz_divider-line;
	SetWindowPos(ghview1,NULL,x,y,w,h,SWP_NOZORDER);

	x=params_divider+line;
	y=0;
	w=rect.right-x;
	h=horiz_divider-line;
	SetWindowPos(ghparams,NULL,x,y,w,h,SWP_NOZORDER);

	x=0;
	y=horiz_divider+line;
	w=page_divider-line;
	h=rect.bottom-y;
	SetWindowPos(ghpagelist,NULL,x,y,w,h,SWP_NOZORDER);

	x=page_divider+line;
	y=horiz_divider+line;
	w=rect.right-x;
	h=rect.bottom-y;
	SetWindowPos(ghpage,NULL,x,y,w,h,SWP_NOZORDER);
	return TRUE;
}
int create_tool_windows(HWND hwnd)
{
    WNDCLASS wnd;
	RECT rect={0};
	memset(&wnd,0,sizeof(wnd));
	wnd.style=CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	wnd.cbClsExtra=0;
	wnd.cbWndExtra=0;
	wnd.hInstance=ghinstance;
	wnd.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wnd.hCursor=LoadCursor(NULL,IDC_ARROW);
	wnd.hbrBackground=GetStockObject(BLACK_BRUSH);
	wnd.lpszMenuName=NULL;
	wnd.lpszClassName="VIEW1";
	wnd.lpfnWndProc=win_view1_proc;
	RegisterClass(&wnd);
	GetClientRect(hwnd,&rect);
	ghview1=CreateWindow(wnd.lpszClassName,"view1",WS_CHILD|WS_VISIBLE,
		0,0,0,0,hwnd,NULL,ghinstance,NULL);


	wnd.lpszClassName="PARAMS";
	wnd.lpfnWndProc=win_params_proc;
	RegisterClass(&wnd);
	ghparams=CreateWindow(wnd.lpszClassName,"params",WS_CHILD|WS_VISIBLE,
		0,0,0,0,hwnd,NULL,ghinstance,NULL);

	wnd.lpszClassName="PAGELIST";
	wnd.lpfnWndProc=win_pagelist_proc;
	RegisterClass(&wnd);
	ghpagelist=CreateWindow(wnd.lpszClassName,"pagelist",WS_CHILD|WS_VISIBLE,
		0,0,0,0,hwnd,NULL,ghinstance,NULL);

	wnd.lpszClassName="PAGE";
	wnd.lpfnWndProc=win_page_proc;
	RegisterClass(&wnd);
	ghpage=CreateWindow(wnd.lpszClassName,"page",WS_CHILD|WS_VISIBLE,
		0,0,0,0,hwnd,NULL,ghinstance,NULL);

	{
		SCREEN *s[3]={&scpage,&scpagelist,&scparams};
		int i;
		for(i=0;i<3;i++){
			s[i]->buffer=malloc(1024*1024*4);
			s[i]->w=1024;
			s[i]->h=1024;
			if(s[i]->buffer)
				memset(s[i]->buffer,0,1024*1024*4);
		}
	}
	resize_main_window(hwnd);
	CreateThread(NULL,0,page_ui_thread,0,0,&page_ui_threadid);
	CreateThread(NULL,0,param_ui_thread,0,0,&param_ui_threadid);
	return TRUE;

}
static int in_range(int x,int min,int max)
{
	if(x>=min && x<=max)
		return TRUE;
	else
		return FALSE;
}
static int adjust_dividers(HWND hwnd,int x,int y,int grab)
{
	int result=FALSE;
	static int *divider=0;
	static int xaxis=FALSE;
	RECT rect={0};
	GetClientRect(hwnd,&rect);
	if(grab){
		divider=0;
		xaxis=TRUE;
		if(in_range(x,params_divider-4,params_divider+4)){
			if(in_range(y,0,horiz_divider-1)){
				divider=&params_divider;
				result=TRUE;
				goto exit;
			}
		}
		if(in_range(x,page_divider-4,page_divider+4)){
			if(in_range(y,horiz_divider+1,rect.bottom)){
				divider=&page_divider;
				result=TRUE;
				goto exit;
			}
		}
		if(in_range(y,horiz_divider-4,horiz_divider+4)){
			if(in_range(x,0,rect.right)){
				divider=&horiz_divider;
				result=TRUE;
				xaxis=FALSE;
				goto exit;
			}
		}
	}
	if(divider){
		if(xaxis){
			if(in_range(x,5,rect.right-5)){
				*divider=x;
				result=TRUE;
			}
		}
		else{
			if(in_range(y,5,rect.bottom-5)){
				*divider=y;
				result=TRUE;
			}
		}
	}
exit:
	return result;
}
LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static HDC hDC=0;
	static lmb_drag=FALSE;
#ifdef _DEBUG
	if(FALSE)
	if(msg!=WM_PAINT&&msg!=WM_SETCURSOR) //msg!=WM_NCHITTEST&&msg!=WM_ENTERIDLE&&
	{
		static DWORD tick;
		if((GetTickCount()-tick)>500)
			printf("--\n");
		print_msg(msg,lparam,wparam,hwnd);
		tick=GetTickCount();
	}
#endif
	switch(msg){
    case WM_CREATE:
		{
			create_tool_windows(hwnd);
			hDC=GetDC(ghview1);
			if(hDC)
				setupPixelFormat(hDC);
			hGLRC=wglCreateContext(hDC);
			if(hGLRC)
				wglMakeCurrent(hDC,hGLRC);
			if(hDC){
				SelectObject(hDC,GetStockObject(SYSTEM_FONT));
				wglUseFontBitmaps(hDC,0,255,1000);
			}

			gl_init();
			init_keys();
			init_world();
			init_entity_array(players,sizeof(players)/sizeof(ENTITY*));
			init_entity_array(non_players,sizeof(non_players)/sizeof(ENTITY*));
			add_player();
			init_page_list();
			GetClientRect(hwnd,&gwinrect);
		}
        return 0;
	case WM_MBUTTONDOWN:
		move_rect(0,0,wparam);
		break;
	case WM_LBUTTONUP:
		lmb_drag=FALSE;
		ReleaseCapture();
		break;
	case WM_LBUTTONDOWN:
		SetCursor(LoadCursor(NULL,IDC_SIZEALL));
		lmb_drag=TRUE;
		SetCapture(hwnd);
		{
			int x,y;
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			adjust_dividers(hwnd,x,y,TRUE);
		}
		break;
	case WM_MOUSEMOVE:
		{
			static int x=0,y=0;
			move_rect(LOWORD(lparam)-x,HIWORD(lparam)-y,wparam);
			x=LOWORD(lparam);
			y=HIWORD(lparam);
			SetCursor(LoadCursor(NULL,IDC_SIZEALL));
			set_focus(0);
			if(lmb_drag){
				if(adjust_dividers(hwnd,x,y,FALSE))
					resize_main_window(hwnd);
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{
		}
		break;
	case WM_KEYFIRST:
		if(wparam==0x1b)
			exit(0);
		key_down(wparam);
		break;
	case WM_KEYUP:
		key_up(wparam);
		break;
	case WM_SYSKEYDOWN:
		key_down(wparam);
		return 0;
		break;
	case WM_SYSKEYUP:
		key_up(wparam);
		break;
	case WM_SIZE:
		{
			resize_main_window(hwnd);
		}
		break;
	case WM_APP:
		switch(wparam){
		case WM_SETFOCUS:
			if(lparam){
				SetFocus(lparam);
				printf("set focus %08X\n",lparam);
			}
			break;
		}
		break;
	case WM_PAINT:
		{
			static HBRUSH hbrush=0;
			if(hbrush==0){
				LOGBRUSH brush={0};
				DWORD color=0x00FF00;
				brush.lbColor=color;
				brush.lbStyle=BS_SOLID;
				hbrush=CreateBrushIndirect(&brush);
			}
			{
				PAINTSTRUCT ps;
				HDC hdc;
				hdc=BeginPaint(hwnd,&ps);
				if(hGLRC){
					if(FALSE){
						static DWORD tick=0;
						printf("%i\n",GetTickCount()-tick);
						tick=GetTickCount();
					}
					g_draw=1;
					display_view1(ghview1,hGLRC);
				}
				if(hdc){
					EnterCriticalSection(&mutex);
					display_page(ghpage,&scpage);
					display_params(ghparams,&scparams);
					LeaveCriticalSection(&mutex);
				}
				if(hbrush && ghfocus && hdc){
					RECT rect={0};
					GetClientRect(ghfocus,&rect);
					MapWindowPoints(ghfocus,hwnd,&rect,2);
					rect.left-=4;
					if(rect.left<0)
						rect.left=0;
					rect.top-=4;
					if(rect.top<0)
						rect.top=0;
					rect.bottom+=4;
					rect.right+=4;
					FillRect(hdc,&rect,hbrush);
				}

				if(hdc)
					EndPaint(hwnd,&ps);
				return 0;
			}
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wparam)){
		case IDCANCEL:
			exit(0);
			break;
		}
	case WM_CLOSE:
		exit(0);
		break;
	}
	return DefWindowProc(hwnd,msg,wparam,lparam);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine,int iCmdShow)
{
    WNDCLASS wnd;
	MSG msg;
	const char *class_name="DEMOTOOL";
	ghinstance=hInstance;

	open_console();
	move_console();
	memset(&wnd,0,sizeof(wnd));
	wnd.style=CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	wnd.lpfnWndProc=WndProc;
	wnd.cbClsExtra=0;
	wnd.cbWndExtra=0;
	wnd.hInstance=ghinstance;
	wnd.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wnd.hCursor=LoadCursor(NULL,IDC_ARROW);
	wnd.hbrBackground=GetStockObject(GRAY_BRUSH);
	wnd.lpszMenuName=NULL;
	wnd.lpszClassName=class_name;
	RegisterClass(&wnd);
	ghwindow=CreateWindow(class_name,class_name,WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE,
		0,0,800,600,NULL,NULL,ghinstance,NULL);
	if(!ghwindow){
		MessageBox(NULL,"Could not create main dialog","ERROR",MB_ICONERROR | MB_OK);
		return 0;
	}
	InitializeCriticalSection(&mutex);
	UpdateWindow(ghwindow);
	//ghaccel=LoadAccelerators(ghinstance,MAKEINTRESOURCE(IDR_ACCELERATOR));
	_beginthread(worker_thread,0,0);
	while(GetMessage(&msg,NULL,0,0))
	{
		{
			int _msg,lparam,wparam;
			_msg=msg.message;
			lparam=msg.lParam;
			wparam=msg.wParam;
			if(FALSE)
			if(_msg!=WM_MOUSEFIRST&&_msg!=WM_NCHITTEST&&_msg!=WM_SETCURSOR&&_msg!=WM_ENTERIDLE&&_msg!=WM_DRAWITEM
				&&_msg!=WM_CTLCOLORBTN&&_msg!=WM_CTLCOLOREDIT&&_msg!=WM_PAINT)
			//if(_msg!=WM_NCHITTEST&&_msg!=WM_SETCURSOR&&_msg!=WM_ENTERIDLE)
			{
				static DWORD tick=0;
				if((GetTickCount()-tick)>500)
					printf("--\n");
				printf("*");
				print_msg(_msg,lparam,wparam);
				tick=GetTickCount();
			}
		}
		if(ghaccel!=0)
			TranslateAccelerator(ghwindow,ghaccel,&msg);
		DispatchMessage(&msg);
//		if(!IsDialogMessage(ghwindow,&msg)){
//			TranslateMessage(&msg);
//		}
	}
	return msg.wParam;
}