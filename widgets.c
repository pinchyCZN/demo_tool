#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>


int draw_button(int x,int y,int w,int h,int color)
{
	int i;
	static float theta=0;
	unsigned char indices[] = { 0, 1, 2, 0, 2, 3 };
	float vertices[] = { 
		0, 0, 0,
		1, 0, 0,
		1, 1, 0,
		0, 1, 0
	};
	unsigned char colors[] = { 
		0, 0xFF, 0,
		0, 0xFF, 0,
		0, 0xFF, 0,
		0, 0xFF, 0
	};
	for(i=0;i<4;i++){
		vertices[ 3 * i + 0 ] *= w;
		vertices[ 3 * i + 1 ] *= h;
		colors[i*3]=color>>16;
		colors[i*3+1]=color>>8;
		colors[i*3+2]=color;
	}
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,1024,1024,0,-1000,1000);

	glColor3f(1.0,1.0,1.0);
	glTranslatef(x,y,0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
	glDrawElements(GL_LINES, 6, GL_UNSIGNED_BYTE, indices);





	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

}