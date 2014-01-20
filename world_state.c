#include <windows.h>
#include "entity.h"

unsigned int global_tick=0;
static int frame_time=16;

int init_world()
{
	global_tick=GetTickCount();
	frame_time=16;
	return 1;
}
int update_world(ENTITY **e,int count)
{

	unsigned int current_tick=GetTickCount();
	int limit=0;

	while(global_tick<current_tick){
		int i;
		for(i=0;i<count;i++){
			entity_move(e[i],frame_time);
		}
		for(i=0;i<count;i++){
			if(e[i]!=0){
				if(e[i]->state==STATE_DIE){
					free_entity(e[i]);
					e[i]=0;
				}
			}
		}
		global_tick+=16;
		limit++;
		if(limit>4){
			global_tick=current_tick;
			break;
		}
	}
	return 1;
}

int render_entitys(ENTITY **e,int count)
{
	int i;
	for(i=0;i<count;i++){
		if(e[i]!=0)
			render(e[i]);
	}
	return 1;
}