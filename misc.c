#include <windows.h>
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>

int set_console_buffer(int y)
{
	COORD size;
	size.X=80;
	size.Y=y;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),size);
	return TRUE;
}

int rotate(float rx,float ry,float rz,float *x,float *y,float *z)
{
	float cx,cy,cz;
	float sx,sy,sz;
	float fx,fy,fz;

	cx=cos(rx);
	cy=cos(ry);
	cz=cos(rz);
	sx=sin(rx);
	sy=sin(ry);
	sz=sin(rz);

	fx = x[0]*cy*cz + y[0]*cy*sz - z[0]*sy;
	fy = x[0]*(sx*sy*cz-cx*sz) + y[0]*(sx*sy*sz+cx*cz) + z[0]*sx*cy;
	fz = x[0]*(cx*sy*cz+sx*sz) + y[0]*(cx*sy*sz-sx*cz) + z[0]*cx*cy;
	x[0]=fx;y[0]=fy;z[0]=fz;
	return 0;
}

int snprintf(char *buf,int buf_len,char *fmt,...)
{
	int r;
	va_list va;
	va_start(va,fmt);
	r=_vsnprintf(buf,buf_len,fmt,va);
	if(buf_len>0)
		buf[buf_len-1]=0;
	return r;
}