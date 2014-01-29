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

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glTranslatef(0.0F, 0.0F, -10.4F);
	glRotatef(theta, 0.0f, 1.0f, 1.0f);


	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);   glVertex2f(0.0f,   1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);   glVertex2f(0.87f,  -0.5f);
	glColor3f(0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);
	glEnd();


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	theta+=1.0;
}
int display_str(char *str,int x,int y)
{
extern int g_screenw,g_screenh;
	int i,len;
	int list[3]={GL_TEXTURE_2D,GL_DEPTH_TEST,GL_LIGHTING};
	int setting[3];
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,g_screenw,g_screenh,0,-1000,1000);

	for(i=0;i<sizeof(list)/sizeof(int);i++){
		setting[i]=0;
		if(glIsEnabled(list[i])){
			glDisable(list[i]);
			setting[i]=1;
		}
	}
	glColor3f(1.0,1.0,1.0);
	glRasterPos3f(x,y+15,0);

	glListBase(1000); 
	len=strlen(str);
	glCallLists(len,GL_UNSIGNED_BYTE,str);

	for(i=0;i<sizeof(list)/sizeof(int);i++){
		if(setting[i])
			glEnable(list[i]);
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
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
	glDisable(GL_DEPTH_TEST);

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
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	//glTranslatef(e->posx,e->posy,e->posz);
	glTranslatef(50-rand()%10,50-rand()%100,50-rand()%100);
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
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
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
