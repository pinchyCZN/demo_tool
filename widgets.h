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
	unsigned char *r,*g,*b;
}RECTANGLE;

typedef struct{
	int deltax,deltay;
	int filled;
	int color;
}CONTROLDRAG;

#define ETYPE_FLOAT 1
#define ETYPE_BYTE 2
#define ETYPE_INT 3
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
	char str[4];
	int cursor;
	int changed;
	int overwrite;
	unsigned char *byte;
	int x,y;
	int w,h;
}EDITBYTE;

typedef struct{
	char str[12];
	int cursor;
	int changed;
	int overwrite;
	unsigned int *integer;
	int x,y;
	int w,h;
}EDITINT;

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
	int light_num;
	unsigned char r_ambient,g_ambient,b_ambient;
	unsigned char r_diffuse,g_diffuse,b_diffuse;
	unsigned char r_specular,g_specular,b_specular;
	float posx,posy,posz,posw;
	float exponent;
	float cuttoff;
	int attenuation;
}LIGHT_DATA;

typedef struct{
 int a;
}TEXTURE_DATA;

typedef struct{
	float scalex,scaley,scalez;
	float rotx,roty,rotz;
	float transx,transy,transz;
}TRANSFORM_DATA;


typedef struct{
	float scalex,scaley,scalez;
	float rotx,roty,rotz;
	float transx,transy,transz;
	int count;
}MULTIPLY_DATA;

typedef enum OPTYPES{
	TLIGHT,TTEXTURE,TCUBE,TMULTIPLY,TTRANSFORM,
	TRECT,TDRAG,TRECTSELECT
};

typedef enum CONTROLTYPES{
	CBUTTON,CSCROLL,CRECT,CDRAG,CSTATIC,CEDIT,CEDITBYTE,CEDITINT,CEDITFLOAT
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
	void *list_prev,*list_next;
	int selected;
	int activated;
	int error;
}OP;

typedef struct{
	CONTROL control;
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
	void **links;
	void *op;
	int lcount;
}TREENODE;
