#include <windows.h>
#include "widgets.h"

int build_page(SCREEN *page,RECT *rect)
{
	BUTTON b={0};
	SCROLLBAR scroll={0};
	int i;
	for(i=0;i<10;i++){
		char str[80];
		b.w=40;
		b.h=20;
		b.x=10;
		b.y=i*30;
		sprintf(str,"cube%i",i);
		b.text=str;
		draw_button(page,&b);
	}
	scroll.w=20;
	scroll.h=rect->bottom-rect->top;
	scroll.pos=0;
	scroll.range=1000;
	scroll.x=(rect->right-rect->left)-20;
	scroll.y=0;
	draw_vscroll(page,&scroll);
}

int show_page_list()
{
}

int show_params()
{
}
int display_view1(HWND hwnd,HGLRC hglrc)
{
	HDC hdc;
	if(hwnd==0 || hglrc==0)
		return FALSE;
	hdc=GetDC(hwnd);
	if(hdc){
		wglMakeCurrent(hdc,hglrc);
		display();
		SwapBuffers(hdc);
	}
}
int display_page(HWND hwnd,SCREEN *page)
{
	HDC hdc;
	RECT rect={0};
	int *buffer,w,h;
	if(page==0 || page->buffer==0)
		return 0;
	buffer=page->buffer;
	w=page->w;
	h=page->h;
	memset(buffer,0x10,1024*1024*4);
	GetWindowRect(hwnd,&rect);
	build_page(page,&rect);
	hdc=GetDC(hwnd);
	if(hdc){
		BITMAPINFO bmi;
		memset(&bmi,0,sizeof(BITMAPINFO));
		bmi.bmiHeader.biBitCount=32;
		bmi.bmiHeader.biWidth=w;
		bmi.bmiHeader.biHeight=h;
		bmi.bmiHeader.biPlanes=1;
		bmi.bmiHeader.biSize=40;
		SetDIBitsToDevice(hdc,0,0,w,h,0,0,0,w,buffer,&bmi,DIB_RGB_COLORS);
	}
	return 0;
}
int display_page_list(HWND hwnd,HGLRC hglrc)
{

}
int display_params(HWND hwnd,HGLRC hglrc)
{

}