#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
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
				/*
				error=lodepng_decode24_file(&data,&w,&h,"p2.png");
				if(error)
					printf("error %u: %s\n", error, lodepng_error_text(error));
				*/
				w=1024;
				h=32;
				data=malloc(3*w*h);
				if(data){
					int i,j;
					memset(data,0,3*w*h);
					for(i=0;i<w/h;i++){
						char str[10];
						sprintf(str,"%i",i);
						printstr(str,data,i*32+10,10,w,h);
					}
					for(i=0;i<w;i++){
						data[i*3+1]=0x7F;
						data[i*3+1+((h-1)*w*3)]=0x7F;
					}
					for(i=0;i<w;i+=32){
						for(j=0;j<h;j++){
							data[i*3+1+((j)*w*3)]=0x7F;
						}
					}
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
				e->pw=32;
				e->ph=32;
				e->trows=1;
				e->tcols=w/h;
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

int move_player1(ENTITY *e,int frame_time)
{
	int printinfo=FALSE;
	if(e==0)
		return 0;
	if(key_pressed('p')){
		switch(e->state_action){
		default:
			e->state_action=S_ACTION_PAUSE1;
			break;
		case S_ACTION_PAUSE3:
		case S_ACTION_PAUSE2:
			e->state_action=S_ACTION_PAUSE3;
			break;
		}
	}else{
		switch(e->state_action){
		default:
			e->state_action=0;
			break;
		case S_ACTION_PAUSE2:
		case S_ACTION_PAUSE1:
			e->state_action=S_ACTION_PAUSE2;
			break;
		}
	}
	if(e->state_action==S_ACTION_PAUSE1 || e->state_action==S_ACTION_PAUSE2)
		return 0;
	if(key_pressed(VK_LEFT)){
		//printf("delta=%i\n",delta);
		e->rotx=-1;
		e->speedx=frame_time/2;
	}
	else if(key_pressed(VK_RIGHT)){
		e->rotx=1;
		e->speedx=frame_time/2;
	}
	else{
		e->speedx-=frame_time;
	}
	if(key_pressed(VK_DOWN)){
		e->posz+=10;
		printinfo=TRUE;
	}
	if(key_pressed(VK_UP)){
		e->posz-=10;
		printinfo=TRUE;
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
	else if(e->posy<0){
		e->posy=0;
		e->speedy=0;
	}

	if(e->speedx==0)
		e->frame=0;
	else{
		e->time+=e->speedx;
		if(e->time>12){
			e->time=0;
			e->frame++;
			if(e->frame>=(e->tcols*e->trows))
				e->frame=0;
		}
	}
	if(e->speedx!=0 || e->speedy!=0 || e->speedz!=0 || printinfo)
		printf("sx=% 5.1f sy=% 5.1f sz=% 5.1f px=% 5.1f py=% 5.1f pz=% 5.1f\n",e->speedx,e->speedy,e->speedz,e->posx,e->posy,e->posz);
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