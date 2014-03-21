#define WHITE	0xFFFFFF
#define RED		0xFF0000
#define GREEN	0x00FF00
#define BLUE	0x0000FF

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
	const char *str;
	int x;
	int y;
	int w;
	int h;
}STATICTEXT;

typedef struct{
	char *str;
	int maxlen;
	int cursor;
	int multiline;
	int overwrite;
	int x;
	int y;
	int w;
	int h;
}EDITBOX;

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
	char str[16];
	int cursor;
	int changed;
	int overwrite;
	float *fdata;
	int x,y;
	int w,h;
}EDITFLOAT;

typedef struct{
	const char *name;
	int x,y;
	int w,h;
	float a,b,c;
	char numa[16];
	char numb[16];
	char numc[16];
	int cursor;
	int animate;
	int selected; //0=none,1=a,b,c,animate
	int changed;
}C3FLOATA;


typedef struct{
	float tessx,tessy,tessz;
}CUBE_DATA;

typedef struct{
	float	scalex,scaley,scalez;
	float	rotatex,rotatey,rotatez;
	float	transx,transy,transz;
}MULTIPLY_DATA;

typedef enum OPTYPES{
	TCUBE,TMULTIPLY,
	TRECT,TDRAG,TRECTSELECT
};

typedef enum CONTROLTYPES{
	CBUTTON,CSCROLL,CSTATIC,CEDIT,CEDITFLOAT,CRECT,CDRAG
};

typedef struct{
	int type;
	void *data;
}CONTROL;

typedef struct{
	int type;
	void *data;
	CONTROL control;
	char name[20];
	void *op_prev,*op_next;
	void *list_prev,*list_next;
	int selected;
	int activated;
	int error;
}OP;

typedef struct{
//	int type;
	CONTROL control;
	const char *name;
	int x,y;
	int has_focus;
	void *dref;
	void *next;
	void *prev;
}PARAM_CONTROL;

typedef struct{
	PARAM_CONTROL *list;
	OP *ref;
	int hscroll;
	int vscroll;
}PARAM_LIST;

typedef struct{
	OP *list;
	char name[20];
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



typedef struct{
	void *links;
	void *op;
	int lcount;
}TREENODE;
