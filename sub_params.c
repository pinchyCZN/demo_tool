#define _WIN32_WINNT 0x400
#include <windows.h>
#include <math.h>
#include "widgets.h"


int create_subparams(OP *op,PARAM_CONTROL *pc)
{
	if(op==0 || pc==0)
		return FALSE;
	if(pc->control.type==CBUTTON){
		switch(op->type){
		case TTRANSFORM:
			{
				int i;
				i++;
			}
			break;
		}
	}
}