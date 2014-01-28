#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "entity.h"

int test()
{
	static float theta=0;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glRotatef(theta, 0.0f, 0.0f, 1.0f);

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);   glVertex2f(0.0f,   1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);   glVertex2f(0.87f,  -0.5f);
	glColor3f(0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);
	glEnd();


	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	theta+=1.0;
}
int display_str(char *str,int x,int y)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,1024,768,0,-1000,1000);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0,1.0,1.0);
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glRasterPos3f(x,y+15,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{
		char *data=malloc(640*480*3);
		int i;
		//glPixelTransferi(
		glPixelZoom(10.0,10.0);
		glRasterPos2i(100,100);
		if(data){
			for(i=0;i<640*480*3;i++){
				data[i]=0xFF;
			}
			glDrawPixels(640,480,GL_RGB,GL_BYTE,data);
			free(data);
		}
	}
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	test();
	return 0;
}
int render_texture_rect(int tex_name,int w,int h,int tw,int th,int toffx,int toffy)
{
	int i;
	unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };
	float vertices[] = { 
		0, 0, 0,
		1, 0, 0,
		1, 1, 0,
		0, 1, 0
	};
	float uv[] = { 
		0, 1,
		1, 1,
		1, 0,
		0, 0,
	};
	for(i=0;i<4;i++){
		vertices[ 3 * i + 0 ] *= w;
		vertices[ 3 * i + 1 ] *= h;
		uv[ 2 * i + 0 ] *= (float) w / (float) tw;
		uv[ 2 * i + 1 ] *= (float) h / (float) th;
		uv[ 2 * i + 0 ] += (float) toffx / (float) tw;
		uv[ 2 * i + 1 ] += (float) toffy / (float) th;
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex_name);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, uv);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
	return 0;
}
int render(ENTITY *e)
{
/*
	int i;
	float w,h;
	float vertices[] = { 
		0, 0, 0,
		1, 0, 0,
		1, 1, 0,
		0, 1, 0
	};
	unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };
	float uv[] = { 
		0, 1,
		1, 1,
		1, 0,
		0, 0,
	};
	if(e==0)
		return 0;

	{
		int cols=e->tcols;
		int rows=e->trows;
		if(cols<=0)cols=1;
		if(rows<=0)rows=1;
		w=e->pw;
		h=e->ph;
		if(w==0)w=(float)e->tw/(float)e->tcols;
		if(h==0)h=(float)e->th/(float)e->trows;

		for(i = 0; i < 4; i++) {
			vertices[ 3 * i + 0 ] *= w;
			vertices[ 3 * i + 1 ] *= h;
			uv[ 2 * i + 0 ] *= ((float) w / (float) e->tw);
			uv[ 2 * i + 1 ] *= ((float) h / (float) e->th);
			//uv[ 2 * i + 0 ] += ((float) (e->frame%(rows))*w / (float) e->tw);
			//uv[ 2 * i + 1 ] += ((float) ((e->frame/(cols+1))*h) / (float) e->th);
			uv[ 2 * i + 0 ] += (float) ((e->frame*w) / (float) e->tw);
			uv[ 2 * i + 1 ] += (float) ((e->frame*h*0) / (float) e->th);
		}
	}
*/
	glPushMatrix();
	glTranslatef(e->posx,e->posy,e->posz);
	{
		int w,h;
		w=e->pw;
		h=e->ph;
		if(e->rotx<0){
			glTranslatef(w/2,0,0);
			glRotatef(180,0,1,0);
		}
		render_texture_rect(e->tex_name,w,h,e->tw,e->th,e->frame*w,0);
	}
	glPopMatrix();

	if(e->type==PLAYER1){
		static unsigned int tick=0;
		static int last;
		unsigned int t;
		char str[12];
		t=GetTickCount();
		snprintf(str,sizeof(str),"FPS=%3u",t-tick);
//		if(last!=(t-tick))
//			printf("%s\n",str);
		last=t-tick;
		display_str(str,0,0);
		tick=t;
	}

	return 0;
}
