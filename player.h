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
	BYTE *texture;
	unsigned int tex_name;
	int tw,th;

	int mode;
	int frame;
	DWORD tick;
	DWORD delta;

	player();
	int move();

	int render();
	int key_pressed(int key);
	void key_down(int key);
	void key_up(int key);
	void get_modifiers();
};
