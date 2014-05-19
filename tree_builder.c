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
	find_root_op(p,&o);
	if(o){
		build_tree(p,o);
	}
	return 0;
}
int free_tree(TREENODE *root)
{
	int result=FALSE;
	if(root){
		int i;
		for(i=0;i<root->lcount;i++){
			free_tree(root->links[i]);
			if(root->links[i]){
				free(root->links[i]);
				root->links[i]=0;
				result=TRUE;
			}
		}
		if(root->links)
			free(root->links);
		memset(root,0,sizeof(TREENODE));
	}
	return result;
}
int add_tree_node(TREENODE *t,OP *o)
{
	int result=FALSE;
	if(t==0 || o==0)
		return result;
	{
		TREENODE *node=0;
		node=malloc(sizeof(TREENODE));
		if(node){
			int newsize;
			TREENODE **tmp=t->links;
			memset(node,0,sizeof(TREENODE));
			node->op=o;
			newsize=sizeof(void*)*(t->lcount+1);
			tmp=realloc(tmp,newsize);
			if(tmp){
				tmp[t->lcount]=node;
				t->lcount++;
				t->links=tmp;
				result=TRUE;
			}
			else{
				free(node);
				node=0;
			}
		}
	}
	return result;
}
int add_stackedops(PAGE_DATA *p,TREENODE *node,OP *current_op)
{
	int result=FALSE;
	if(p==0 || node==0 || current_op==0)
		return result;
	{
		OP *oplist=p->list;
		while(oplist){
			if(current_op!=oplist){
				if(is_stacked(&current_op->control,&oplist->control)){
					oplist->error=1;
					add_tree_node(node,oplist);
					if(node->lcount > 0)
						add_stackedops(p,node->links[node->lcount-1],oplist);
					result=TRUE;
				}
			}
			oplist=oplist->list_next;
		}
	}
	return result;
}
int handle_multiply(TREENODE *t,MULTIPLY_DATA *m)
{
	if(m && t){
		int i;
		push_model_matrix();
		for(i=0;i<m->count;i++){
			int j;
			float *scale,*rot,*trans;
			scale=&m->scalex;
			rot=&m->rotx;
			trans=&m->transx;
			transform_mesh(scale,rot,trans);
			for(j=0;j<t->lcount;j++){
				dump_tree(t->links[j],TRUE);
			}
		}
		pop_model_matrix();
	}
}
int dump_tree(TREENODE *t,int render)
{
	if(t){
		int i;
		OP *o=t->op;
		if(o){
			if(render){
				switch(o->type){
				case TCUBE:
					{
						static float theta=0;
						float rot[3],trans[3],size[2];
						rot[0]=theta; rot[2]=0;
						rot[1]=theta;
						trans[0]=trans[1]=trans[2]=0;
						size[0]=1;
						size[1]=1;
						//trans[2]=-100;
						//render_rect(&rot,&trans);
						render_cube();
					}
					break;
				case TLIGHT:
					{
						LIGHT_DATA *l=o->data;
						if(l){
							set_light(l);
						}
					}
					break;
				case TMULTIPLY:
					{
						MULTIPLY_DATA *m=o->data;
						if(m)
							handle_multiply(t,m);
					}
					break;
				case TTRANSFORM:
					{
						TRANSFORM_DATA *t=o->data;
						if(t){
							//push_model_matrix();
							transform_mesh(&t->scalex,&t->rotx,&t->transx);
							//pop_model_matrix();
						}
					}
					break;
				}

			}
			else
				printf("opname=%s\n",o->name);
		}
		else if(!render)
			printf("node has no OP!\n");

		for(i=0;i<t->lcount;i++){
			dump_tree(t->links[i],render);
		}
	}
}
int build_tree(PAGE_DATA *p,OP *current_op)
{
	int result=FALSE;
	if(p==0 || current_op==0)
		return result;
	clear_page_errors(p);
	free_tree(&rootnode);
	memset(rootnode,0,sizeof(TREENODE));
	rootnode.op=current_op;
	add_stackedops(p,&rootnode,current_op);
	dump_tree(&rootnode,FALSE);
}

int display_tree()
{
	dump_tree(&rootnode,TRUE);
}