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

#define DEFBUTTONH 25
#define DEFBUTTONW (DEFBUTTONH*3)

typedef struct{
	int type;
	int x;
	int y;
	int w;
	int h;
	char *text;
	int pressed;
}BUTTON;

#define SCROLL_VERT 0
#define SCROLL_HORZ 1
#define SCROLL_WIDTH 20
typedef struct{
	int type; //vert or horz
	int x;
	int y;
	int w;
	int h;
	int pos;
	int range;
	int pressed;
}SCROLLBAR;

typedef struct{
	char *str;
	int x;
	int y;
	int w;
	int h;
}STATICTEXT;

typedef struct{
	int x;
	int y;
	int w;
	int h;
	int filled;
	int color;
}RECTANGLE;

typedef struct{
	int deltax,deltay;
	int filled;
	int color;
}CONTROLDRAG;


typedef struct{
	float x,y,z;
	float w,h,l;
}CUBE_DATA;

typedef enum OPTYPES{
	TCUBE,TRECT,TDRAG,TRECTSELECT
};

typedef enum CONTROLTYPES{
	CBUTTON,CSCROLL,CSTATIC,CRECT,CDRAG
};

typedef struct{
	int type;
	void *data;
}CONTROL;

typedef struct{
	int type;
	void *data;
	CONTROL control;
	char name[80];
	void *op_prev,*op_next;
	void *list_prev,*list_next;
	int selected;
	int activated;
	int error;
}OP;

typedef struct{
	OP *list;
	char *name;
	int hscroll;
	int vscroll;
	int vscroll_pressed;
	int hscroll_pressed;
	int cursorx,cursory;
	void *next;
	void *prev;
}PAGE_DATA;

typedef struct{
	PAGE_DATA *list;
	PAGE_DATA *current;
	int hscroll;
	int vscroll;
}PAGE_LIST;