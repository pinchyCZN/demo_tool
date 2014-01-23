#include <windows.h>
#include <stdio.h>
#include "glut.h"
#include "entity.h"


int bind_texture(char *data,int w,int h,int *texname)
{
	int tn=0;
	int result=FALSE;
	glGenTextures(1,&tn);
	if(tn!=0){
		glBindTexture(GL_TEXTURE_2D,tn);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
		glTexImage2D(GL_TEXTURE_2D,0,3,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,data);
		*texname=tn;
		result=TRUE;
	}
	return result;
}
int init_entity(ENTITY *e,int type){
	if(e==0)
		return FALSE;
	memset(e,0,sizeof(ENTITY));
	e->type=type;
	switch(type){
	case PLAYER1:
		{
			int error;
			static char *data=0;
			static int w=0,h=0;
			if(data==0){
				error=lodepng_decode24_file(&data,&w,&h,"p2.png");
				if(error)
					printf("error %u: %s\n", error, lodepng_error_text(error));
			}
			if(data!=0){
				static int texname=0;
				if(texname==0)
					bind_texture(data,w,h,&texname);
				e->texture=data;
				e->tex_name=texname;
				e->tw=w;
				e->th=h;
				e->trows=5;
				e->tcols=6;
			}
		}
		break;
	case BULLET1:
		{
			static char *data=0;
			static int w=0,h=0;
			w=8;h=12*8;
			if(data==0){
				data=malloc(w*h*3);
				if(data){
					int i;
					for(i=0;i<w*h*3;i++){
						data[i]=0x7f;
					}
					for(i=0;i<8;i++)
						printchar('0'+i,data,0,i*12,w,h);
				}
			}
			if(data!=0){
				static int texname=0;
				if(texname==0)
					bind_texture(data,w,h,&texname);
				e->texture=data;
				e->tex_name=texname;
				e->tw=w;
				e->th=h;
				e->pw=8;
				e->ph=12;
				e->tcols=1;
				e->trows=8;
			}
		}
		break;
	}
};

int free_entity(ENTITY *e)
{
	int result=FALSE;
	if(e==0)
		return result;
	free(e);
	result=TRUE;
	return result;
}
int render(ENTITY *e)
{
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
		if(e->type==BULLET1){
			printf("frame=%i\n",e->frame);
		}

		for(i = 0; i < 4; i++) {
			vertices[ 3 * i + 0 ] *= w;
			vertices[ 3 * i + 1 ] *= h;
			uv[ 2 * i + 0 ] *= ((float) w / (float) e->tw);
			uv[ 2 * i + 1 ] *= ((float) h / (float) e->th);
			uv[ 2 * i + 0 ] += ((float) (e->frame%(cols+1))*w / (float) e->tw);
			uv[ 2 * i + 1 ] += ((float) ((e->frame/(rows))*h) / (float) e->th);
		}
	}

	glPushMatrix();
	glTranslatef(e->posx,e->posy,e->posz);
	if(e->rotx<0){
		glTranslatef(w/2,0,0);
		glRotatef(180,0,1,0);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,e->tex_name);


	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, uv);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, e->tex_name);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

//	glColor4f(1,1,1,1);
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

int move_player1(ENTITY *e,int frame_time)
{
	if(e==0)
		return 0;
	if(key_pressed(GLUT_KEY_LEFT)){
		//printf("delta=%i\n",delta);
		e->rotx=-1;
		e->speedx=frame_time/8;
	}
	else if(key_pressed(GLUT_KEY_RIGHT)){
		e->rotx=1;
		e->speedx=frame_time/8;
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
		e->speedy=frame_time;
	}
	else{
		e->speedy-=frame_time/6;
	}
	if(key_pressed(VK_MENU)){
		int s[3]={0,0,0},p[3]={0,0,0};
		if(e->rotx<0)
			s[0]=-2;
		else
			s[0]=2;
		p[0]=e->posx;
		p[1]=e->posy;
		p[2]=e->posz;
		if(e->state_action==0){
			e->state_action=S_ACTION_FIRE;
			e->stime_action=0;
			add_bullet(e,s,p);
		}
		printf("menu\n");
	}
	switch(e->state_action){
	case S_ACTION_FIRE:
		e->stime_action+=frame_time;
		if(e->stime_action>250)
			e->state_action=0;
		break;
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

	e->time+=e->speedx;
	if(e->time>12){
		e->time=0;
		e->frame++;
		if(e->frame>=(e->tcols*e->trows))
			e->frame=0;
	}
	return 0;
}

int move_bullet(ENTITY *e,int frame_time)
{
	if(e==0)
		return 0;
	e->posx+=e->speedx;
	e->posy+=e->speedy;
	e->posz+=e->speedz;
	e->time+=frame_time;
//	printf("time %i\n",e->time);
	if(e->owner){
		ENTITY *o=e->owner;
		if(o->texture){
			double d=o->posx-e->posx;
			if(d<-3000 || d>3000){
				e->state_life=S_LIFE_DIE;
				e->stime_life=0;
			}
		}
		if(e->time>200){
			e->time=0;
			e->frame++;
			if(e->frame>=(e->tcols*e->trows))
				e->frame=0;
		}
	}
	else if(e->time > 1000){
		e->state_life=S_LIFE_DIE;
		e->stime_life=0;
	}
	return 0;
}

int entity_move(ENTITY *e,int frame_time)
{
	if(e==0)
		return 0;
	switch(e->type){
	case PLAYER1:
		move_player1(e,frame_time);
		break;
	case BULLET1:
		move_bullet(e,frame_time);
		break;
	}
	return 0;
}