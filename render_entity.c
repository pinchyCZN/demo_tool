#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "entity.h"
#include "widgets.h"

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
	unsigned char indices[] = { 0, 1, 2, 0, 2, 3 };
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
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
	return 0;
}
int render_rect_(float x,float y,float z,float w,float h)
{
	int i;
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
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
}

int render_rect(float *size,float *rot,float *trans)
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
	for(i=0;i<4;i++){
		vertices[ 3 * i + 0 ] *= size[0];
		vertices[ 3 * i + 1 ] *= size[1];
	}
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(trans[0], trans[1], trans[2]);
	glRotatef(rot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
	glRotatef(rot[2], 0.0f, 0.0f, 1.0f);
	//glRotatef(theta, 0.0f, 1.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glNormalPointer(GL_FLOAT,0,normals);
	glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,indices);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	theta+=1;

}


int render_cube(float x,float y,float z,float w,float h,float d)
{

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
//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(e->posx,e->posy,e->posz);
	//glTranslatef(50-rand()%10,50-rand()%100,50-rand()%100);
	{
		int w,h;
		w=e->pw;
		h=e->ph;
		if(e->rotx<0){
			glTranslatef(w/2,0,0);
			glRotatef(180,0,1,0);
		}
//		render_rect(100,100);
		//render_texture_rect(e->tex_name,w,h,e->tw,e->th,e->frame*w,0);
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();

	if(e->type==PLAYER1){
		static unsigned int tick=0;
		static int last;
		unsigned int t,delta;
		char str[12];
		float fps=0;
		t=GetTickCount();
		delta=t-tick;
		if(delta!=0)
			fps=((float)1/(float)delta)*(float)1000;
		snprintf(str,sizeof(str),"FPS=%02.1f",fps);
//		if(last!=(t-tick))
//			printf("%s\n",str);
		last=delta;
		display_str(str,0,0);
		tick=t;
	}

	return 0;
}
int byte_to_float(float *f,unsigned char *b,int count)
{
	int i;
	for(i=0;i<count;i++){
		f[i]=(float)b[i]/(float)255;
	}
	return i;
}
int float_to_float(float *f,float *fsrc,int count)
{
	int i;
	for(i=0;i<count;i++){
		f[i]=fsrc[i];
	}
	return i;
}
int set_light(LIGHT_DATA *l)
{
	float f[4]={0,0,0,0};
	int light_num=GL_LIGHT0+l->light_num;
	byte_to_float(f,&l->r_ambient,3);
	glLightfv(light_num,GL_AMBIENT,f);
	byte_to_float(f,&l->r_diffuse,3);
	glLightfv(light_num,GL_DIFFUSE,f);
	byte_to_float(f,&l->r_specular,3);
	glLightfv(light_num,GL_SPECULAR,f);
	float_to_float(f,&l->posx,4);
	glLightfv(light_num,GL_POSITION,f);
	glLighti(light_num,GL_LINEAR_ATTENUATION,l->attenuation);
	glEnable(GL_LIGHTING);
	glEnable(light_num);
	return TRUE;
}
/*
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
	   */