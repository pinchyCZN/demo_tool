#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "entity.h"


int render_rect(float w,float h)
{
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
		1, 1, 1,
		1, 1, 1,
		1, 1, 1,
		1, 1, 1
	};
	for(i=0;i<4;i++){
		vertices[ 3 * i + 0 ] *= w;
		vertices[ 3 * i + 1 ] *= h;
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glNormalPointer(GL_FLOAT,0,normals);
	glColorPointer(3,GL_FLOAT,0,colors);
	glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,indices);
}

//int set_light(