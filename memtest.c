#include <windows.h>
#include <stdio.h>

static int count=0;
#undef malloc
void * my_alloc(unsigned int size)
{
	count++;
	printf("count=%i\n",count);
#define malloc malloc
	return malloc(size);
}

#undef realloc
void * my_realloc(void *t,unsigned int size)
{
	if(t==0){
		count++;
		printf("count=%i\n",count);
	}
	else{
		printf("realloc=%08X\n",t);
	}
#define realloc realloc
	return realloc(t,size);
}

#undef free
void my_free(void *t)
{
	count--;
	printf("count=%i\n",count);
#define free free
	free(t);
}