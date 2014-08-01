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
	int xscroll;
	int yscroll;
}SCREEN;

#define DEFBUTTONH 25
#define DEFBUTTONW (DEFBUTTONH*3)
#define RESIZE_MARGIN 8

typedef struct{
	int id;
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
	int x;
	int y;
	int w;
	int h;
	int current;
	char *list;
	int dropped;
	void *child; //paramcontrol
}DROPLIST;

typedef struct{
	int x;
	int y;
	int w;
	int h;
	int highlighted;
	int count;
	char *list;
	void *parent; //paramcontrol
}POPUPLIST;


typedef struct{
	int deltax,deltay;
	int filled;
	int color;
}CONTROLDRAG;

typedef struct{
	int x,y,w,h;
	int filled;
	int color;
}RESIZERECT;

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
	float time; //x
	float val; //y
	void *prev,*next;
}SPLINE_KEY;

typedef struct{
	int x,y;
	int w,h;
	SPLINE_KEY *key;
	int selected;
	void *prev,*next;
}SPLINE_KEY_CONTROL;

typedef struct{
	int enabled;
	int type;
	float amount;
	SPLINE_KEY *key;
}ANIMATE_DATA;


typedef struct{
	float scalex,scaley,scalez;
	float rotx,roty,rotz;
	float transx,transy,transz;
	ANIMATE_DATA anim[9];
}TRANSFORM_DATA;

typedef struct{
	int current;
	int length;
	int start,end;
}TIME_DATA;


typedef struct{
	float scalex,scaley,scalez;
	float rotx,roty,rotz;
	float transx,transy,transz;
	int count;
}MULTIPLY_DATA;

typedef struct{
	ANIMATE_DATA *anim;
	SPLINE_KEY_CONTROL *keys;
	int x,y;
	int w,h;
	float zoomx,zoomy;
	int count;
	int selected;
}SPLINE_CONTROL;


typedef enum OPTYPES{
	TLIGHT,TTEXTURE,TCUBE,TMULTIPLY,TTRANSFORM,
	TRESIZE,TDRAG,TTIME
};

typedef enum CONTROLTYPES{
	CBUTTON,CSCROLL,CRECT,CDROPLIST,CPOPUPLIST,CRESIZERECT,CDRAG,CSTATIC,CEDIT,CEDITBYTE,CEDITINT,CEDITFLOAT,CSPLINE
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
	int isroot;
	int error;
}OP;

typedef struct{
	CONTROL control;
	int has_focus;
	void *next;
	void *prev;
}PARAM_CONTROL;

typedef struct{
	int hscroll;
	int vscroll;
	int vscroll_pressed;
	int hscroll_pressed;
}SCROLL_INFO;


typedef struct{
	PARAM_CONTROL *list;
	OP *ref;
	SCROLL_INFO si;
}PARAM_LIST;

struct PCLIST{
	int type;
	int x,y,w,h;
	void *data;
	int data_ex;
	int incypos;
};


typedef struct{
	OP *list;
	char name[20];
	SCROLL_INFO si;
	int cursorx,cursory;
	void *next;
	void *prev;
}PAGE_DATA;

typedef struct{
	PAGE_DATA *list;
	PAGE_DATA *current;
}PAGE_LIST;

typedef struct{
	PARAM_LIST plist;
}SPLINE_EDIT;

typedef struct{
	void **links;
	void *op;
	int lcount;
}TREENODE;
