#include <windows.h>
#include <stdio.h>
#include "glut.h"
#include "player.h"

player::player(){
	tick=delta=0;
	posx=posy=posz=0;
	rotx=roty=rotz=0;
	speedz=speedy=speedz=0;
	mode=0;
	frame=0;
	memset(keys,0,sizeof(keys));
	texture=(BYTE*)malloc(256*256*4);
	int i;
//	for(i=0;i<256*256*4;i++)
//		texture[i]=i*2;
	FILE *f=fopen("b1.bmp","rb");
	if(f!=0){
		fseek(f,0x120,SEEK_SET);
		fread(texture,256*256*4,1,f);
		fclose(f);
	}
	glGenTextures(1,&tex_name);
};
int player::render()
{
	glPushMatrix();
	glTranslatef(posx,posy,posz);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 80, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,tex_name);

    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);

	glutSolidCube(100);
	/*
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2f(0, 0);
	glVertex2d(0,0);
	glTexCoord2f(1, 0);
	glVertex2d(0,10);
	glTexCoord2f(1, 1);
	glVertex2d(10,10);
	glTexCoord2f(0, 1);
	glVertex2d(0,40);
	glEnd();
	*/

	glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);

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
	delta=current_tick-tick;
	
	if(delta==0)
		return 0;

	//GetKeyboardState(keys);

	if(key_pressed(GLUT_KEY_LEFT)){
		rotx=-1;
		speedx=20;
	}
	else if(key_pressed(GLUT_KEY_RIGHT)){
		rotx=1;
		speedx=20;
	}
	else{
		speedx-=delta/5;
	}
	if(key_pressed(GLUT_KEY_DOWN)){
		posz+=10;
	}
	if(key_pressed(GLUT_KEY_UP)){
		posz-=10;
	}

	if(key_pressed(VK_CONTROL)){
		speedy=40;
	}
	else{
		speedy-=delta/2;
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
		printf("sx=%f\n",speedx);

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

	tick=current_tick;
	return 0;

}