#include <windows.h>
#include <stdio.h>
#include "glut.h"
#include "entity.h"

int init_entity(ENTITY *e,int type){
	if(e==0)
		return FALSE;
	memset(e,0,sizeof(ENTITY));
	e->type=type;
	if(type==PLAYER1){
		int i;
		e->texture=(BYTE*)malloc(256*256*4);
	//	for(i=0;i<256*256*4;i++)
	//		texture[i]=i*2;
		FILE *f=fopen("b1.bmp","rb");
		if(f!=0){
			fseek(f,0x120,SEEK_SET);
			fread(e->texture,256*256*4,1,f);
			fclose(f);
		}
		for(i=0;i<256*256*4-3;i+=3){
			char r,g,b;
			r=e->texture[i];
			g=e->texture[i+1];
			b=e->texture[i+2];
			e->texture[i]=b;
			e->texture[i+1]=g;
			e->texture[i+2]=r;
		}
		glGenTextures(1,&tex_name);
		tw=80;
		th=100;
	}
};
int free_entity(ENTITY *e)
{
	int result=FALSE;
	if(e==0)
		return result;
	if(e->texture!=0)
		free(e->texture);
	memset(e,0,sizeof(ENTITY));
	result=TRUE;
	return result;
}
int render(ENTITY *e)
{
	glPushMatrix();
	glTranslatef(posx,posy,posz);


	// draw quad with approritate texcoords
	float vertices[] = { 
		0, 0, 0,
		1, 0, 0,
		1, 1, 0,
		0, 1, 0
	};
	unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };
	float uv[] = { 
		0, 0,
		1, 0,
		1, 1,
		0, 1
	};
	int i;
	int w,h;
	w=100;
	h=100;
	for(i = 0; i < 4; i++) {
		vertices[ 3 * i + 0 ] *= w;
		vertices[ 3 * i + 1 ] *= h;
		uv[ 2 * i + 0 ] *= (float) w / (float) tw;
		uv[ 2 * i + 1 ] *= (float) h / (float) th;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 80, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,tex_name);


	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, uv);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, tex_name);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColor4f(1,1,1,1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);


    //glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    //glEnable(GL_TEXTURE_GEN_T);

	//glutSolidCube(100);
	/*
	int g_iTextureHeight,g_iTextureWidth;
	g_iTextureWidth=80;
	g_iTextureHeight=100;

	glBegin( GL_QUADS );
	glTexCoord2i( 0, g_iTextureHeight );                           
	glVertex2i( 0, 0 );
	glTexCoord2i( g_iTextureWidth, g_iTextureHeight );     
	glVertex2i( g_iTextureWidth, 0 );
	glTexCoord2i( g_iTextureWidth, 0 );    
	glVertex2i( g_iTextureWidth, g_iTextureHeight );
	glTexCoord2i( 0, 0 );          
	glVertex2i( 0, g_iTextureHeight );
	glEnd();
	*/
	//glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    //glDisable(GL_TEXTURE_GEN_T);

	glPopMatrix();
	return 0;
}
void player::get_modifiers()
{
	int i;
	int list[3]={VK_CONTROL,VK_MENU,VK_SHIFT};
	for(i=0;i<3;i++){
		if(GetKeyState(list[i])&0x8000)
			keys[list[i]]=1;
		else
			keys[list[i]]=0;
	}
}
void player::key_down(int key)
{
	keys[key&0xFF]=1;
}
void player::key_up(int key)
{
	keys[key&0xFF]=0;
}
int player::key_pressed(int key)
{
	int result=keys[key&0xFF];
	if(result)
		result=result;
	return result;
}
int player::move()
{
	DWORD current_tick=GetTickCount();
	int limit=0;
	int frame_time=16;
	//GetKeyboardState(keys);
	while(tick<current_tick){
		if(key_pressed(GLUT_KEY_LEFT)){
			//printf("delta=%i\n",delta);
			rotx=-1;
			speedx=12;
		}
		else if(key_pressed(GLUT_KEY_RIGHT)){
			rotx=1;
			speedx=12;
		}
		else{
			speedx-=frame_time;
		}
		if(key_pressed(GLUT_KEY_DOWN)){
			posz+=10;
		}
		if(key_pressed(GLUT_KEY_UP)){
			posz-=10;
		}

		if(key_pressed(VK_CONTROL)){
			speedy=25;
		}
		else{
			speedy-=frame_time*2;
		}

		if(speedx>100)
			speedx=100;
		else if(speedx<0)
			speedx=0;

		if(speedy>100)
			speedy=100;
		else if(speedy<-100)
			speedy=-100;
		
		posx+=speedx*rotx;

		posy+=speedy;

		if(speedx>0)
			printf("sx=%f posx=%f posy=%f posz=%f\n",speedx,posx,posy,posz);

		//if(posz>100)
		//	posz=0;
		//else if(posz<0)
		//	posz=0;

		if(posx>500)
			posx=500;
		else if(posx<-500)
			posx=-500;

		if(posy>500)
			posy=500;
		else if(posy<0)
			posy=0;
		
		tick+=16;
		limit++;
		if(limit>4){
			tick=current_tick;
			break;
		}
	}
	return 0;

}