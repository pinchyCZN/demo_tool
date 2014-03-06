#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
/*
B=COLOR_3DDKSHADOW
2=COLOR_3DLIGHT
1=COLOR_BTNHIGHLIGHT
3=COLOR_BTNSHADOW
F=COLOR_BTNFACE
0=COLOR_WINDOWFRAME
*/
/* button
	0000000000000000000000000
	01111111111111111111111B0
	01222222222222222222223B0
	012FFFFFFFFFFFFFFFFFFF3B0
	012FFFFFFFFFFFFFFFFFFF3B0
	01333333333333333333333B0
	0BBBBBBBBBBBBBBBBBBBBBBB0
	0000000000000000000000000
  button pressed
	0000000000000000000000000
	0333333333333333333333330
	03FFFFFFFFFFFFFFFFFFFFF30
	03FFFFFFFFFFFFFFFFFFFFF30
	03FFFFFFFFFFFFFFFFFFFFF30
	03FFFFFFFFFFFFFFFFFFFFF30
	0333333333333333333333330
	0000000000000000000000000

*/
int draw_button(int x,int y,int w,int h,int color,int pressed)
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
	unsigned char R,G,B;
	int list[3]={GL_TEXTURE_2D,GL_DEPTH_TEST,GL_LIGHTING};
	int setting[3];
	for(i=0;i<4;i++){
		vertices[ 3 * i + 0 ] *= w;
		vertices[ 3 * i + 1 ] *= h;
	}
	for(i=0;i<sizeof(list)/sizeof(int);i++){
		setting[i]=0;
		if(glIsEnabled(list[i])){
			glDisable(list[i]);
			setting[i]=1;
		}
	}
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,1024,1024,0,-1000,1000);

	glTranslatef(x,y,0);

	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	R=color>>16;
	G=color>>8;
	B=color;
	glColor3ub(R,G,B);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
	glColor3ub(0x70,0x70,0x70);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_BYTE, indices);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_BYTE, indices+1);
	//glDrawElements(GL_LINES, 2, GL_UNSIGNED_BYTE, indices+2);
	//glIndexPointer
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_BYTE, indices+4);





	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	for(i=0;i<sizeof(list)/sizeof(int);i++){
		if(setting[i])
			glEnable(list[i]);
	}
	return 0;

}