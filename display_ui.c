#include <windows.h>

int build_page(int *buffer,int w,int h)
{
	draw_button(buffer,w,h,10,10,50,50,0xFF,FALSE);
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
int display_page(HWND hwnd,int *buffer,int w,int h)
{
	HDC hdc;
	memset(buffer,0x10,1024*1024*4);
	build_page(buffer,w,h);
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