#ifndef __ENTITY_H__
#define __ENTITY_H__

#define PLAYER1 1
#define BULLET1 2

#define S_LIFE_DIE 1

#define S_ACTION_FIRE 1
#define S_ACTION_PAUSE1 9001
#define S_ACTION_PAUSE2 9002
#define S_ACTION_PAUSE3 9003

typedef struct{
	int type;
	double posx;
	double posy;
	double posz;

	double rotx;
	double roty;
	double rotz;

	double speedx;
	double speedy;
	double speedz;

	unsigned char *texture;
	unsigned int tex_name;
	int trows,tcols; //texture rows,cols
	int tw,th; //texture width,height
	int pw,ph; //player width,height

	int state_move;
	int state_action;
	int state_life;
	unsigned int stime_move;
	unsigned int stime_action;
	unsigned int stime_life;
	void *owner;
	int frame;
	unsigned int time;
}ENTITY;

#endif //__ENTITY_H__

