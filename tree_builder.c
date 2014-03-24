#include "types.h"
#include "widgets.h"

TREENODE rootnode={0};

int is_stacked(CONTROL *root,CONTROL *c)
{
	int rx,ry,rw,rh;
	int cx,cy,cw,ch;
	int result=FALSE;
	if(root==0 || c==0)
		return result;
	if(get_control_pos(root,&rx,&ry,&rw,&rh) &&
		get_control_pos(c,&cx,&cy,&cw,&ch)){
		int rxa,rxb,cxa,cxb; //x pos of left and right sides of root and c
		int rya,ryb,cya,cyb; //ypos of top and bottom of root and c
		rxa=rx;
		rxb=rx+rw-1;
		cxa=cx;
		cxb=cx+cw-1;
		cya=cy;
		cyb=cy+ch-1;
		rya=ry;
		ryb=ry+rh-1;
		if((cxa>=rxa && cxa<=rxb)
			||(cxb>=rxa && cxb<=rxb)
			||(rxa>=cxa && rxa<=cxb)
			||(rxb>=cxa && rxb<=cxb)){
			if(cyb+1==rya){
				result=TRUE;
			}
		}
	}
	return result;
}
int clear_page_errors(PAGE_DATA *p)
{
	OP *oplist=0;
	int result=FALSE;
	if(p==0)
		return result;
	oplist=p->list;
	while(oplist){
		if(oplist->error)
			result++;
		oplist->error=0;
		oplist=oplist->list_next;
	}
	return result;
}
int test_build(PAGE_DATA *p)
{
	OP *o=0;
	if(p==0)
		return 0;
	find_selected_op(p,&o);
	if(o){
		build_tree(p,o);
	}
	return 0;
}
int build_tree(PAGE_DATA *p,OP *root)
{
	int result=FALSE;
	OP *oplist=0;
	if(p==0 || root==0 || root->selected==0)
		return result;
	clear_page_errors(p);
	oplist=p->list;
	while(oplist){
		int x,y,w,h;
		if(root!=oplist){
			if(is_stacked(&root->control,&oplist->control)){
				oplist->error=1;
			}
		}
		oplist=oplist->list_next;
	}
}