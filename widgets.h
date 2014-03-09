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
#define DEFBUTTONW 80

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
}SCROLLBAR;

typedef struct{
	char *str;
	int x;
	int y;
}STATICTEXT;


typedef struct{
	float x,y,z;
	float w,h,l;
}CUBE_DATA;

typedef enum OPTYPES{
	TCUBE,TRECT
};

typedef enum CONTROLTYPES{
	TBUTTON,TSCROLL,TSTATIC
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
	int pressed;
}OP;

typedef struct{
	OP *list;
	char *name;
	int hscroll;
	int vscroll;
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