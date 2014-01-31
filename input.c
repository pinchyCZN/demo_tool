#include <windows.h>
#include "entity.h"

static unsigned char keys[256]={0};

void get_modifiers(ENTITY *e)
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
void key_down(int key)
{
	keys[key&0xFF]=1;
}
void key_up(int key)
{
	keys[key&0xFF]=0;
}
int key_state(int key)
{
	int result=keys[key&0xFF];
	if(result)
		result=result;
	return result;
}

int init_keys()
{
	memset(keys,0,sizeof(keys));
	return TRUE;
}

