#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "entity.h"

float a=-75,b=0,c=0;
float x=-7,y=-100,z=-990;
//abc=-75 -2 -1 xyz=-7 -106 -990
int move_rect(int _x,int _y,int key)
{
	int print=FALSE;
	if(key&MK_LBUTTON){
		if(key&MK_CONTROL){
			z+=_y;
		}
		else{
			x+=_x;
			y-=_y;
		}
		print=TRUE;
	}
	else if(key&MK_RBUTTON){
		if(key&MK_CONTROL){
			a+=_y;
		}
		else{
			b+=_x;
			c+=_y;
		}
		print=TRUE;
	}
	else if(key&MK_MBUTTON){
		a=b=c=0;
		x=y=0;
		z=-100;
		print=TRUE;
	}
	if(print){
		printf("abc=%.0f %.0f %.0f xyz=%.0f %.0f %.0f\n",a,b,c,x,y,z);
	}
}

int render_rect(int w,int h)
{
	static float theta=0;
	int i;
	unsigned char indices[] = { 0, 1, 2, 0, 2, 3 };
	float vertices[] = { 
		0, 0, 0,
		1, 0, 0,
		1, 1, 0,
		0, 1, 0
	};
	float normals[] = { 
		0, 0, 1,
		0, 0, 1,
		0, 0, 1,
		0, 0, 1
	};
	float colors[] = { 
		0, 0, .5,
		0, .5, .8,
		0, 0, .9,
		0, 0, 1
	};
	for(i=0;i<4;i++){
		vertices[ 3 * i + 0 ] *= w;
		vertices[ 3 * i + 1 ] *= h;
		vertices[ 3 * i + 0 ] -= w/2;
		vertices[ 3 * i + 1 ] -= h/2;
	}
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glTranslatef(x, y, z);
	glRotatef(a, 1.0f, 0.0f, 0.0f);
	glRotatef(b, 0.0f, 1.0f, 0.0f);
	glRotatef(c, 0.0f, 0.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glNormalPointer(GL_FLOAT,0,normals);
	glColorPointer(3,GL_FLOAT,0,colors);
	glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,indices);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	theta-=1.0;
}

//int set_light(