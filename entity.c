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
		int error;
		error=lodepng_decode24_file(&e->texture,&e->tw,&e->th,"p1.png");
		if(error)
			printf("error %u: %s\n", error, lodepng_error_text(error));
		else{
			/*
			int i;
			for(i=0;i<e->tw*e->th;i+=3){
				char r,g,b;
				r=e->texture[i];
				g=e->texture[i+1];
				b=e->texture[i+2];
				e->texture[i]=b;
				e->texture[i+1]=g;
				e->texture[i+2]=r;
			}
			*/
			glGenTextures(1,&e->tex_name);
		}
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
	{
		int i;
		int w,h;
		w=e->tw;
		h=e->th;
		for(i = 0; i < 4; i++) {
			vertices[ 3 * i + 0 ] *= w;
			vertices[ 3 * i + 1 ] *= h;
			uv[ 2 * i + 0 ] *= (float) w / (float) e->tw;
			uv[ 2 * i + 1 ] *= (float) h / (float) e->th;
		}
	}

	if(e==0)
		return 0;
	glPushMatrix();
	glTranslatef(e->posx,e->posy,e->posz);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, e->tw, e->th, 0, GL_RGB, GL_UNSIGNED_BYTE, e->texture);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,e->tex_name);


	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, uv);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, e->tex_name);
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

int entity_move(ENTITY *e,int frame_time)
{
	if(key_pressed(GLUT_KEY_LEFT)){
		//printf("delta=%i\n",delta);
		e->rotx=-1;
		e->speedx=12;
	}
	else if(key_pressed(GLUT_KEY_RIGHT)){
		e->rotx=1;
		e->speedx=12;
	}
	else{
		e->speedx-=frame_time;
	}
	if(key_pressed(GLUT_KEY_DOWN)){
		e->posz+=10;
	}
	if(key_pressed(GLUT_KEY_UP)){
		e->posz-=10;
	}

	if(key_pressed(VK_CONTROL)){
		e->speedy=25;
	}
	else{
		e->speedy-=frame_time*2;
	}

	if(e->speedx>100)
		e->speedx=100;
	else if(e->speedx<0)
		e->speedx=0;

	if(e->speedy>100)
		e->speedy=100;
	else if(e->speedy<-100)
		e->speedy=-100;
	
	e->posx+=e->speedx*e->rotx;

	e->posy+=e->speedy;

	if(e->speedx>0)
		printf("sx=%f posx=%f posy=%f posz=%f\n",e->speedx,e->posx,e->posy,e->posz);

	//if(posz>100)
	//	posz=0;
	//else if(posz<0)
	//	posz=0;

	if(e->posx>500)
		e->posx=500;
	else if(e->posx<-500)
		e->posx=-500;

	if(e->posy>500)
		e->posy=500;
	else if(e->posy<-100)
		e->posy=-100;
		
	return 0;
}