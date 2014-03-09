typedef struct{
	int *buffer;
	int w;
	int h;
	int clip_enabled;
	int clipxl;
	int clipxr;
	int clipyt;
	int clipyb;
}SCREEN;
typedef struct{
	int type;
	int x;
	int y;
	int w;
	int h;
	char *text;
	int pressed;
}BUTTON;

typedef struct{
	int x;
	int y;
	int w;
	int h;
	int pos;
	int range;
}SCROLLBAR;