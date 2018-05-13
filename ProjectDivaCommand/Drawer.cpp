#include <Windows.h>
#include <string>
#include "Drawer.h"
#include "Launcher.h"
#include "CharsetConverter.h"
using namespace std;
Drawer::Drawer()
{
	stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hwnd = GetConsoleWindow();
	hdc = GetDC(hwnd);
	MUSICSYMBOLBIG = CreateFont(70, 0, 0, 0, 0, 0, 0, 0, JOHAB_CHARSET, 0, 0, 0, 0, "Arial Unicode MS");
	MUSICSYMBOL = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, JOHAB_CHARSET, 0, 0, 0, 0, "Arial Unicode MS");
	NORMALFONT = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Microsoft Yahei");
	BIGFONT = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Microsoft Yahei");
	BIGBIGFONT = CreateFont(80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Microsoft Yahei");
	EDGELINE = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
	CLR_EDGELINE = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	NULLBRUSH = (HBRUSH)GetStockObject(NULL_BRUSH);
	CLR_BRUSH = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	::SetBkColor(_Drawer.hdc, RGB(0, 0, 0));
	::SetTextColor(_Drawer.hdc, RGB(255, 255, 255));
	SYMBOLEDGE = CreatePen(PS_SOLID, 3, RGB(203, 203, 203));//»Ò±ß
	EXPLODEEDGE=CreatePen(PS_DOT, 1, RGB(203, 203, 203));
	TRI_SYMBOLEDGE = CreatePen(PS_SOLID, 5, RGB(203, 203, 203));//»Ò±ß
	CIRCLE_EDGE = SYMBOLEDGE = CreatePen(PS_SOLID, 2, RGB(203, 203, 203));
	TAGBRUSH = (HBRUSH)CreateSolidBrush(RGB(33, 33, 66));
	CIRCLE_SYM = CreatePen(PS_SOLID, 5, RGB(231, 12, 12));
	RECT_SYM = CreatePen(PS_SOLID, 5, RGB(244, 82, 223));
	TRIANGLE_SYM = CreatePen(PS_SOLID, 5, RGB(25, 241, 121));
	CROSS_SYM = CreatePen(PS_SOLID, 8, RGB(33, 33, 66));
	COOL = RGB(246, 223, 26);
	FINE = RGB(217, 217, 217);
	SAFE = RGB(80, 235, 50);
	BAD = RGB(78, 178, 255);
	WORST = RGB(211, 78, 255);
	CIRCLE_FLY = CreatePen(PS_SOLID, 6, RGB(234, 75, 84));
	TRIANGLE_FLY = CreatePen(PS_SOLID, 7, RGB(0, 221, 45));
	CROSS_FLY = CreatePen(PS_SOLID, 6, RGB(65, 162, 252));
	RECTANGLE_FLY = CreatePen(PS_SOLID, 6, RGB(254, 102, 239));
	TITLERECT.left = 30; TITLERECT.right = 930; TITLERECT.top = 0; TITLERECT.bottom = 40;
	BLOODBLUE = (HBRUSH)CreateSolidBrush(RGB(43, 136, 251));
	BLOODRED = (HBRUSH)CreateSolidBrush(RGB(237, 60, 60));

	REDDP= (HBRUSH)CreateSolidBrush(RGB(234, 75, 84));
	BLUEDP= (HBRUSH)CreateSolidBrush(RGB(65, 162, 252));
	PINKDP = (HBRUSH)CreateSolidBrush(RGB(254, 102, 239));
	GREENDP= (HBRUSH)CreateSolidBrush(RGB(0, 221, 45));
}
Drawer::~Drawer()
{
	DeleteObject(MUSICSYMBOL);
	DeleteObject(MUSICSYMBOLBIG);
	DeleteObject(BIGFONT); DeleteObject(NORMALFONT);  DeleteObject(BIGBIGFONT);
	DeleteObject(EDGELINE);
	DeleteObject(CLR_EDGELINE);
	DeleteObject(NULLBRUSH);
	DeleteObject(CLR_BRUSH);
	ReleaseDC(hwnd, hdc);
	ReleaseDC(Global_HWND, Global_HDC);
}
void Drawer::HideCursor()
{
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(stdhandle, &cursor_info);
}

void Drawer::TAG_DP_Right(int x, int y)
{
	SelectObject(hdc, TAGBRUSH);
	SelectObject(hdc, EDGELINE);
	POINT TriPoint[7] = { x - 20,y - 10,x,y - 10,x,y - 20,x + 20,y,x,y + 20,x,y + 10,x - 20,y + 10 };//->
	Polygon(hdc, TriPoint, 7);
}


void Drawer::TAG_DP_Left(int x, int y)
{
	SelectObject(hdc, TAGBRUSH);
	SelectObject(hdc, EDGELINE);
	POINT TriPoint[7] = { x + 20,y + 10,x,y + 10,x,y + 20,x - 20,y,x,y - 20,x,y - 10,x + 20,y - 10 };//<-
	Polygon(hdc, TriPoint, 7);
}

void Drawer::TAG_DP_UP(int x, int y)
{
	SelectObject(hdc, TAGBRUSH);
	SelectObject(hdc, EDGELINE);
	POINT TriPoint[7] = { x + 10,y + 20,x + 10,y ,x + 20,y ,x ,y - 20,x - 20,y,x - 10,y ,x - 10,y + 20 };//up
	Polygon(hdc, TriPoint, 7);
}

void Drawer::TAG_DP_DOWN(int x, int y)
{
	SelectObject(hdc, TAGBRUSH);
	SelectObject(hdc, EDGELINE);
	POINT TriPoint[7] = { x - 10,y - 20,x - 10,y ,x - 20,y ,x ,y + 20,x + 20,y,x + 10,y ,x + 10,y - 20 };//down
	Polygon(hdc, TriPoint, 7);
}
void Drawer::Sym_DP_Right(int x, int y)
{
	SelectObject(hdc, REDDP);
	SelectObject(hdc, EDGELINE);
	POINT TriPoint[7] = { x - 20,y - 10,x,y - 10,x,y - 20,x + 20,y,x,y + 20,x,y + 10,x - 20,y + 10 };//->
	Polygon(hdc, TriPoint, 7);
}
void Drawer::Sym_DP_Left(int x, int y)
{
	SelectObject(hdc, PINKDP);
	SelectObject(hdc, EDGELINE);
	POINT TriPoint[7] = { x + 20,y + 10,x,y + 10,x,y + 20,x - 20,y,x,y - 20,x,y - 10,x + 20,y - 10 };//<-
	Polygon(hdc, TriPoint, 7);
}

void Drawer::Sym_DP_UP(int x, int y)
{
	SelectObject(hdc, GREENDP);
	SelectObject(hdc, EDGELINE);
	POINT TriPoint[7] = { x + 10,y + 20,x + 10,y ,x + 20,y ,x ,y - 20,x - 20,y,x - 10,y ,x - 10,y + 20 };//up
	Polygon(hdc, TriPoint, 7);
}

void Drawer::Sym_DP_DOWN(int x, int y)
{
	SelectObject(hdc, BLUEDP);
	SelectObject(hdc, EDGELINE);
	POINT TriPoint[7] = { x - 10,y - 20,x - 10,y ,x - 20,y ,x ,y + 20,x + 20,y,x + 10,y ,x + 10,y - 20 };//down
	Polygon(hdc, TriPoint, 7);
}

void Drawer::TAG_Circle(int x, int y)
{
	int rx = x - 20, ry = y - 20;
	SelectObject(hdc, CIRCLE_EDGE);
	Ellipse(hdc, rx - 2, ry - 2, rx + 40 + 2, ry + 40 + 2);
	SelectObject(hdc, TAGBRUSH);
	Ellipse(hdc, rx, ry, rx + 40, ry + 40);
}

void Drawer::TAG_Rectangle(int x, int y)
{
	int rx = x - 20, ry = y - 20;
	SelectObject(hdc, SYMBOLEDGE);
	SelectObject(hdc, NULLBRUSH);
	Rectangle(hdc, rx, ry, rx + 40, ry + 40);
	SelectObject(hdc, TAGBRUSH);
	Rectangle(hdc, rx, ry, rx + 40, ry + 40);
}

void Drawer::TAG_Triangle(int x, int y)
{
	SelectObject(hdc, TRI_SYMBOLEDGE);
	POINT pt[3] = { {x, y - 26.667},{x + 23.094,y + 13.333},{ x - 23.094,y + 13.333 } };
	Polygon(hdc, pt, 3);
	SelectObject(hdc, TAGBRUSH);
	Polygon(hdc, pt, 3);
}

void Drawer::TAG_Cross(int x, int y)
{
	SelectObject(hdc, SYMBOLEDGE);

	/*MoveToEx(hdc, x - 22, y - 14,NULL);
	LineTo(hdc, x + 14, y + 22);
	MoveToEx(hdc, x - 14, y - 22, NULL);
	LineTo(hdc, x + 22, y + 14);*/
	MoveToEx(hdc, x + 14, y + 22, NULL);
	LineTo(hdc, x + 22, y + 14);
	MoveToEx(hdc, x + 22, y + 14, NULL);
	LineTo(hdc, x - 14, y - 22);
	MoveToEx(hdc, x - 14, y - 22, NULL);
	LineTo(hdc, x - 22, y - 14);
	MoveToEx(hdc, x - 22, y - 14, NULL);
	LineTo(hdc, x + 14, y + 22);
	MoveToEx(hdc, x + 22, y - 14, NULL);
	LineTo(hdc, x + 14, y - 22);
	MoveToEx(hdc, x + 14, y - 22, NULL);
	LineTo(hdc, x - 22, y + 14);
	MoveToEx(hdc, x - 22, y + 14, NULL);
	LineTo(hdc, x - 14, y + 22);
	MoveToEx(hdc, x - 14, y + 22, NULL);
	LineTo(hdc, x + 22, y - 14);
	SelectObject(hdc, CROSS_SYM);
	MoveToEx(hdc, x - 14, y - 14, NULL);
	LineTo(hdc, x + 14, y + 14);
	MoveToEx(hdc, x + 14, y - 14, NULL);
	LineTo(hdc, x - 14, y + 14);
}

void Drawer::Sym_Circle(int x, int y)
{
	int rx = x - 20, ry = y - 20;
	SelectObject(hdc, CIRCLE_FLY);
	SelectObject(hdc, NULLBRUSH);
	Ellipse(hdc, rx - 2, ry - 2, rx + 40 + 2, ry + 40 + 2);
}

void Drawer::Sym_Triangle(int x, int y)
{
	SelectObject(hdc, TRIANGLE_FLY);
	SelectObject(hdc, NULLBRUSH);
	POINT pt[3] = { { x, y - 26.667 },{ x + 23.094,y + 13.333 },{ x - 23.094,y + 13.333 } };
	Polygon(hdc, pt, 3);
}

void Drawer::Sym_Rect(int x, int y)
{
	int rx = x - 20, ry = y - 20;
	SelectObject(hdc, RECTANGLE_FLY);
	SelectObject(hdc, NULLBRUSH);
	Rectangle(hdc, rx, ry, rx + 40, ry + 40);
}

void Drawer::Sym_Cross(int x, int y)
{
	SelectObject(hdc, CROSS_FLY);
	MoveToEx(hdc, x - 15, y - 15, NULL);
	LineTo(hdc, x + 15, y + 15);
	MoveToEx(hdc, x + 15, y - 15, NULL);
	LineTo(hdc, x - 15, y + 15);
}

void Drawer::Draw_Judge(JudgeInfo ji,int combo)
{
	
	SelectObject(hdc, NORMALFONT);
	string textpcombo;
	switch (ji.Judge)
	{
	case 0:

		SetTextColor(hdc, COOL);
		SetBkMode(hdc, TRANSPARENT);
		textpcombo = "COOL " + I2S(combo);
		TextOut(hdc, (int)ji.PositionPixelX, (int)ji.PositionPixelY, (LPCSTR)textpcombo.c_str(), textpcombo.length());
		break;
	case 1:
		SetTextColor(hdc, FINE);
		SetBkMode(hdc, TRANSPARENT);
		textpcombo = "FINE " + I2S(combo);
		TextOut(hdc, (int)ji.PositionPixelX, (int)ji.PositionPixelY, (LPCSTR)textpcombo.c_str(), textpcombo.length());
		break;
	case 2:
		SetTextColor(hdc, SAFE);
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, (int)ji.PositionPixelX, (int)ji.PositionPixelY, (LPCSTR)"SAFE", 5);
		break;
	case 3:
		SetTextColor(hdc, BAD);
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, (int)ji.PositionPixelX, (int)ji.PositionPixelY, (LPCSTR)"BAD", 4);
		break;
	case 4:
		SetTextColor(hdc, WORST);
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, (int)ji.PositionPixelX, (int)ji.PositionPixelY, (LPCSTR)"WORST", 6);
		break;
	default:
		break;
	}
	SetTextColor(hdc, RGB(255, 255, 255));
}

void Drawer::Draw_Explode(int x, int y, int scale)
{
	int rx = x - scale / 2, ry = y - scale / 2;
	SelectObject(hdc, EXPLODEEDGE);
	SelectObject(hdc, NULLBRUSH);
	Ellipse(hdc, rx - 2, ry - 2, rx + scale + 2, ry + scale + 2);
}



void Drawer::DrawNotifications(string text)
{
	SelectObject(hdc, BIGBIGFONT);
	DrawText(hdc, (LPCSTR)text.c_str(), text.length() + 1, &Client_Rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
}
