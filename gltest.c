#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include "glut.h"
#define pi 3.14159265
int rotate(float rx,float ry,float rz,float *x,float *y,float *z);
BYTE *texture=0;
int tsize=0;
int move=TRUE;

int octaves=3;
float persistance=3;
float divider=1;

float gx=73,gy=-6,gz=-553;
float grx=-247,gry=-181,grz=-97;

typedef struct{
	float x,y,z; //position
	float rx,ry,rz; //rotation
	float v[3]; //velocity vector
}PLAYER;

typedef struct{
	float x,y,z; //position
#define MAX_TERRAIN_DIM 100
	int width,length;
	float data[MAX_TERRAIN_DIM][MAX_TERRAIN_DIM][3];
}SURFACE;

PLAYER bike;
SURFACE terrain;

static int lastx=0,lasty=0;
static int mousex=0,mousey=0;
GLfloat ambient[]={1.0,1.0,1.0,0.0};
GLfloat light_position[]={5.0,10.0,10.0,0.0};
GLfloat white_light[]={1.0,1.0,1.0,1.0};
GLfloat ctrlpoints[4][3]={{-4.0,-4.0,0.0},{-2.0,4.0,0.0},{2.0,-4.0,0.0},{4.0,4.0,0.0}};

#include "terrain.h"
#include "physics.h"

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



/*
int bezier()
{
	int i;
	glDisable(GL_LIGHTING);
	glMap1f(GL_MAP1_VERTEX_3,0.0,1.0,3,3,&ctrlpoints[0][0]);
	glEnable(GL_MAP1_VERTEX_3);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINE_STRIP);
	for(i=0;i<=30;i++)
		glEvalCoord1f((GLfloat) i/30.0);
	glEnd();
	glPointSize(5.0);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_POINTS);
	for(i=0;i<4;i++)
		glVertex3fv(&ctrlpoints[i][0]);
	glEnd();
	glEnable(GL_LIGHTING);
	glFlush();

}
*/
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

	t1=GetTickCount();
	create_terrain(&terrain,MAX_TERRAIN_DIM,MAX_TERRAIN_DIM,persistance,octaves);
	draw_terrain(&terrain);
	draw_bike(&bike);
//	printf("time=%u v=%f\n",GetTickCount()-t1,bike.v[0]);
	offset[0]+=bike.v[0];

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
}
void keyboard(unsigned char key,int x,int y)
{
	static lastmx=0,lastmy=0;
	static index=0;

	switch(key){
	case 0xD:
		index++;
		index%=4;
		break;
	case 'a':
		if(persistance<=1){
			if(persistance>0.000001)
				persistance/=2;
		}
		else if(persistance<=2.5){
			persistance-=.1;
		}
		else
			persistance--;
		if(persistance<0)
			persistance=1;
		break;
	case 'z':
		if(persistance<=1){
			persistance*=2;
		}
		else if(persistance<=2.5){
			persistance+=.1;
		}
		else
			persistance++;
		break;
	case 's':
		octaves--;
		if(octaves<1)
			octaves=1;
		break;
	case 'x':
		octaves++;
		break;
	case 'd':
		break;
	case 'c':

		break;
	case 'q':
		move=TRUE;
		break;
	case ' ':
		gx=gy=0;gz=-200;
		break;
	}
	printf("pers=%f,octaves=%i,divider=%f\n",persistance,octaves);
//	glMatrixMode(GL_MODELVIEW);

	if(key==0x1b)
		exit(0);
	glutPostRedisplay();
}
void special_keys(int key,int x,int y)
{
	switch(key){
	case GLUT_KEY_UP:
		//bike.v[0]+=sin(1/(bike.v[0]+.00001))+.01;
		bike.v[0]+=.001;
		if(bike.v[0]>1)
			bike.v[0]=1;
		break;
	case GLUT_KEY_DOWN:
		bike.v[0]-=.1;
		if(bike.v[0]<0)
			bike.v[0]=0;
		break;
	case GLUT_KEY_RIGHT:
		break;
	case GLUT_KEY_LEFT:
		break;
	case GLUT_KEY_END:
		break;
	}
}
void mouse_button(int button,int state,int x,int y)
{
	//state GLUT_UP GLUT_DOWN
	switch(button){
	case GLUT_LEFT_BUTTON:
		lastx=x;
		lasty=y;
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	}
	if(state==GLUT_UP){
		printf("pos=%f %f %f\n",gx,gy,gz);
		printf("rot=%f %f %f\n",grx,gry,grz);
	}
}
void mouse_move_no_button(int x, int y)
{
//	printf("no button %i %i\n",mousex,mousey);
}
void mouse_move_button(int x, int y)
{
	int dx,dy;

	dx=lastx-x;
	dy=lasty-y;
	mousex+=lastx-x;mousey+=lasty-y;
	lastx=x;lasty=y;


//	printf("made it %i %i\n",mousex,mousey);
	if(GetKeyState(VK_SHIFT)&0x8000){
		gz+=dy;
	}
	else if(GetKeyState(VK_CONTROL)&0x8000){
		grx+=dy;
		grz+=dx;
	}
	else{
		gx-=dx;
		gy+=dy;
	}
	glutPostRedisplay();
}
void animation(void)
{
	if(bike.v[0]!=0 || bike.v[2]!=0){
		physics(&terrain,&bike);
		display();
	}
	else
		Sleep(10);
}
void init_objects()
{
	memset(&bike,0,sizeof(bike));
	memset(&terrain,0,sizeof(terrain));
	terrain.length=MAX_TERRAIN_DIM;
	terrain.width=MAX_TERRAIN_DIM;
}
int main(int argc,char **argv)
{
	init_objects();
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(800,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[0]);
	gl_init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keys);
	glutMouseFunc(mouse_button);
	glutPassiveMotionFunc(mouse_move_no_button);
	glutMotionFunc(mouse_move_button);
	glutIdleFunc(animation);

	glutMainLoop();
	return 0;
}