#define PLAYER1 1

typedef struct{
	int		type;
	double posx;
	double posy;
	double posz;

	double rotx;
	double roty;
	double rotz;

	double speedx;
	double speedy;
	double speedz;

	BYTE *texture;
	unsigned int tex_name;
	int tw,th;

	int mode;
	DWORD tick;

}ENTITY;


