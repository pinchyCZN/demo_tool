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
	for(i=0;i<4;i++){
		vertices[ 3 * i + 0 ] *= w;
		vertices[ 3 * i + 1 ] *= h;
	}
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,1024,1024,0,-1000,1000);

	glColor3f(1.0,1.0,1.0);
	x=rand()%500;
	y=rand()%500;
	glTranslatef(x,y,0);
	glRotatef(theta,0,1,0);
	theta+=.1;

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);





	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

}