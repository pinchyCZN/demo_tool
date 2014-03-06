#include <windows.h>

int show_page()
{
	display_str("page",10,10);
	draw_button(10,10,100,100,0xFF,FALSE);
}

int show_page_list()
{
	display_str("page list",10,20);
	draw_button(10,10,100,100,0xFF,FALSE);
}

int show_params()
{
	display_str("params",10,30);
	draw_button(10,10,100,100,0xFF,FALSE);
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
int display_page(HWND hwnd,HGLRC hglrc)
{
	HDC hdc;
	if(hwnd==0 || hglrc==0)
		return FALSE;
	hdc=GetDC(hwnd);
	if(hdc){
		wglMakeCurrent(hdc,hglrc);
		show_page();
		SwapBuffers(hdc);
	}

}
int display_page_list(HWND hwnd,HGLRC hglrc)
{
	HDC hdc;
	if(hwnd==0 || hglrc==0)
		return FALSE;
	hdc=GetDC(hwnd);
	if(hdc){
		wglMakeCurrent(hdc,hglrc);
		show_page_list();
		SwapBuffers(hdc);
	}

}
int display_params(HWND hwnd,HGLRC hglrc)
{
	HDC hdc;
	if(hwnd==0 || hglrc==0)
		return FALSE;
	hdc=GetDC(hwnd);
	if(hdc){
		wglMakeCurrent(hdc,hglrc);
		show_params();
		SwapBuffers(hdc);
	}

}