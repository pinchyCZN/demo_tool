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
		rotx=1;
		speedx+=delta/30;
		posx-=speedx;
	}
	else if(key_pressed(GLUT_KEY_RIGHT)){
		rotx=-1;
		speedx+=delta/30;
		posx+=speedx;
	}
	else{
		speedx-=delta/30;
	}
	if(key_pressed(VK_CONTROL)){
		posy+=delta;		
	}
	else{
		posy-=delta*1.2;
	}

	if(speedx>30)
		speedx=30;
	else if(speedx<0)
		speedx=0;

	if(posz>100)
		posz=0;
	else if(posz<0)
		posz=0;

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