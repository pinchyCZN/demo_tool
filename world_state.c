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
int get_global_tick()
{
	return global_tick;
}
int set_global_tick(unsigned int t)
{
	return global_tick=t;
}

int update_world(ENTITY **e,int count,unsigned int gtick,unsigned int current_tick)
{

	int limit=0;
	while(gtick<current_tick){
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
		gtick+=16;
		limit++;
		if(limit>4){
			gtick=current_tick;
			break;
		}
	}
	return gtick;
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