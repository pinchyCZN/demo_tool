#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "entity.h"
#include "resource.h"

HDC			ghDC=0;
HWND		ghwindow=0;
HINSTANCE	ghinstance=0;
HACCEL		ghaccel=0;
int g_draw=0;

float gx=0,gy=0,gz=0;
float grx=0,gry=0,grz=0;

GLfloat ambient[]={1.0,1.0,1.0,0.0};
GLfloat light_position[]={5.0,10.0,10.0,0.0};
GLfloat white_light[]={1.0,1.0,1.0,1.0};

ENTITY *players[2];
ENTITY *non_players[100];

void gl_init(void)
{
	
	glClearColor(0.0,0.0,0.0,0.0);
//	glShadeModel(GL_FLAT);
//	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
	glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
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

void display(void)
{

	static float x,y,m,b,h;
	static float position[3],offset[3];
	DWORD t1;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glColor3f(1.0,0.0,0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(gx,gy,gz);
	glRotatef(grx,1,0,0);
	glRotatef(gry,0,1,0);
	glRotatef(grz,0,0,1);
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


	test();

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
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	perspectiveGL(65.0,(GLfloat)w/(GLfloat)h,-1000.0,10000.0);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();

}
void glkey_down(unsigned char key)
{
	key_down(key);
	printf("keydown=%02X\n",key);
}
void glspecial_down(int key)
{
	key_down(key);
//	printf("specdown=%02X\n",key);
}
void glkey_up(unsigned char key)
{
//	printf("keyup=%02X\n",key);
	key_up(key);
}
void glspecial_up(int key)
{
//	printf("specialup=%02X\n",key);
	key_up(key);
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
//		test();
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

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	static HDC hDC=0;
	static HGLRC hGLRC=0;
	print_msg(msg,lparam,wparam,hwnd);
	switch(msg){
    case WM_CREATE:
		{
			hDC=GetDC(hwnd);
			ghDC=hDC;
			if(hDC)
				setupPixelFormat(hDC);
			hGLRC=wglCreateContext(hDC);
			if(hGLRC)
				wglMakeCurrent(hDC,hGLRC);
			gl_init();
			init_keys();
			init_world();
			init_entity_array(players,sizeof(players)/sizeof(ENTITY*));
			init_entity_array(non_players,sizeof(non_players)/sizeof(ENTITY*));
			add_player();
		}
        return 0;
	case WM_KEYFIRST:
		if(wparam==0x1b)
			exit(0);
		glkey_down(wparam);
		break;
	case WM_KEYUP:
		glkey_up(wparam);
		break;
	case WM_SYSKEYDOWN:
		glspecial_down(wparam);
		break;
	case WM_SYSKEYUP:
		glspecial_up(wparam);
		break;
	case WM_SIZE:
		reshape(LOWORD(lparam),HIWORD(lparam));
		break;
	case WM_APP:
	case WM_PAINT:
		{
			int i;
			i=i+1;
		}
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd,&ps);
            if(hGLRC){
				static DWORD tick=0;
				//printf("%i\n",GetTickCount()-tick);
				tick=GetTickCount();
				g_draw=1;
				display();
			    SwapBuffers(ghDC);

			}
            EndPaint(hwnd,&ps);
            return 0;
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
	const char *class_name="GL_TEST";
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
	wnd.hbrBackground=GetStockObject(BLACK_BRUSH);
	wnd.lpszMenuName=NULL;
	wnd.lpszClassName=class_name;
	RegisterClass(&wnd);
	ghwindow=CreateWindow(class_name,"gametest",WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		0,0,640,480,NULL,NULL,ghinstance,NULL);
	if(!ghwindow){
		
		MessageBox(NULL,"Could not create main dialog","ERROR",MB_ICONERROR | MB_OK);
		return 0;
	}
	ShowWindow(ghwindow,iCmdShow);
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
				&&_msg!=WM_CTLCOLORBTN&&_msg!=WM_CTLCOLOREDIT)
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