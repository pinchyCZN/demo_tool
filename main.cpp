#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include "glut.h"
#include "player.h"

float gx=73,gy=-6,gz=-553;
float grx=-247,gry=-181,grz=-97;

GLfloat ambient[]={1.0,1.0,1.0,0.0};
GLfloat light_position[]={5.0,10.0,10.0,0.0};
GLfloat white_light[]={1.0,1.0,1.0,1.0};

player *players[1]={0};

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

	players[0]->move();
	players[0]->render();


	t1=GetTickCount();
//	printf("time=%u v=%f\n",GetTickCount()-t1,bike.v[0]);

	glFlush();
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
void key_down(unsigned char key,int x,int y)
{
	players[0]->key_down(key);
	printf("keydown=%02X\n",key);
	switch(key){
	case 0x1B:
		exit(0);
		break;
	}
}
void special_down(int key,int x,int y)
{
	players[0]->key_down(key);
	printf("specdown=%02X\n",key);
}
void key_up(unsigned char key,int x,int y)
{
	printf("keyup=%02X\n",key);
	players[0]->key_up(key);
}
void special_up(int key,int x,int y)
{
	printf("specialup=%02X\n",key);
	players[0]->key_up(key);
}
void idle()
{
	static DWORD tick=0;
	DWORD delta,current=GetTickCount();
	delta=current-tick;
	if(delta>30){
		tick=current;
		glutPostRedisplay();
	}

}
int main(int argc,char **argv)
{
	players[0]=new player();
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(800,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[0]);
	gl_init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key_down);
	glutSpecialFunc(special_down);
	glutKeyboardUpFunc(key_up);
	glutSpecialUpFunc(special_up);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}