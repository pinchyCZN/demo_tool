#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "entity.h"

float a=-75,b=0,c=0;
float x=-7,y=-100,z=-990;
//abc=-75 -2 -1 xyz=-7 -106 -990
int move_rect(int _x,int _y,int key)
{
	int print=FALSE;
	if(key&MK_LBUTTON){
		if(key&MK_CONTROL){
			z+=_y;
		}
		else{
			x+=_x;
			y-=_y;
		}
		print=TRUE;
	}
	else if(key&MK_RBUTTON){
		if(key&MK_CONTROL){
			a+=_y;
		}
		else{
			b+=_x;
			c+=_y;
		}
		print=TRUE;
	}
	else if(key&MK_MBUTTON){
		a=b=c=0;
		x=y=0;
		z=-100;
		print=TRUE;
	}
	if(print){
//		printf("abc=%.0f %.0f %.0f xyz=%.0f %.0f %.0f\n",a,b,c,x,y,z);
	}
}

