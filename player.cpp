#include <windows.h>
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
};
int player::render()
{
	glPushMatrix();
	glTranslatef(posx,posy,posz);
	glutSolidCube(100);
	glPopMatrix();
	return 0;
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
		speedx+=delta/30;
		posx+=speedx;
	}
	else if(key_pressed(GLUT_KEY_RIGHT)){
		rotx=1;
		speedx+=delta/30;
		posx-=speedx;
	}
	else{
		speedx-=delta/30;
	}
	if(key_pressed(VK_CONTROL)){
		posz+=delta/30;		
	}
	else{
		posz-=delta/30;		
	}

	if(speedx>3)
		speedx=3;
	else if(speedx<0)
		speedx=0;

	if(posz>100)
		posz=0;
	else if(posz<0)
		posz=0;

	if(posx>100)
		posx=0;
	else if(posx<0)
		posx=0;

	tick=current_tick;
	return 0;

}