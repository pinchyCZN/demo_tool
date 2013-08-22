class player{
public:
	double posx;
	double posy;
	double posz;

	double rotx;
	double roty;
	double rotz;

	double speedx;
	double speedy;
	double speedz;

	BYTE keys[256];

	int mode;
	int frame;
	DWORD tick;
	DWORD delta;

	player();
	int move();

	int render();
	int key_pressed(int key);
	void player::key_down(int key);
	void player::key_up(int key);
};
