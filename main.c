#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include "glut.h"
#include "entity.h"

float gx=0,gy=0,gz=-500;
float grx=0,gry=0,grz=0;

GLfloat ambient[]={1.0,1.0,1.0,0.0};
GLfloat light_position[]={5.0,10.0,10.0,0.0};
GLfloat white_light[]={1.0,1.0,1.0,1.0};

ENTITY *players[1]={0};

void gl_init(void)
{
	glClearColor(0.0,0.0,0.0,0.0);
//	glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
	glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
//		glLightfv(GL_LIGHT0,GL_POSITION,light_position);
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

	update_world(players,1);
	render(players[0]);
	get_modifiers();


	t1=GetTickCount();
//	printf("time=%u v=%f\n",GetTickCount()-t1,bike.v[0]);

	glutSwapBuffers();
	glFlush();
	glFinish();
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

void reshape(int w, int h)
{
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0,(GLfloat)w/(GLfloat)h,1.0,10000.0);
	//gluPerspective(65.0,1,1.0,1000.0);
	//glOrtho(-5,5,-5,5,0,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
//	glTranslatef(0.0,0.0,-5.0);
	glutPostRedisplay();
}
void glkey_down(unsigned char key,int x,int y)
{
	key_down(key);
	printf("keydown=%02X\n",key);
	switch(key){
	case 0x1B:
		exit(0);
		break;
	}
}
void glspecial_down(int key,int x,int y)
{
	key_down(key);
//	printf("specdown=%02X\n",key);
}
void glkey_up(unsigned char key,int x,int y)
{
//	printf("keyup=%02X\n",key);
	key_up(key);
}
void glspecial_up(int key,int x,int y)
{
//	printf("specialup=%02X\n",key);
	key_up(key);
}
void idle()
{
	static DWORD tick=0;
	DWORD delta,current=GetTickCount();
	delta=current-tick;
	if(delta>=17){
		tick=current;
		glutPostRedisplay();
		//printf(" delta=%i\n",delta);
	}
	else{
		Sleep(1);
		//printf("delta=%i\n",delta);
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
int add_player()
{
	ENTITY *e;
	e=malloc(sizeof(ENTITY));
	if(e){
		init_entity(e,PLAYER1);
		players[0]=e;
	}

}
int main(int argc,char **argv)
{
	init_keys();
	init_world();
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
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