#pragma once
#include <Windows.h>
#include <string>
#include "InGame.h"
using namespace std;
class Drawer
{
public:
	Drawer();
	~Drawer();
	void HideCursor();
	void TAG_DP_Right(int x, int y);
	void TAG_DP_Left(int x, int y);
	void TAG_DP_UP(int x, int y);
	void TAG_DP_DOWN(int x, int y);
	void Sym_DP_Right(int x, int y);
	void Sym_DP_Left(int x, int y);
	void Sym_DP_UP(int x, int y);
	void Sym_DP_DOWN(int x, int y);
	void TAG_Circle(int x, int y);
	void TAG_Rectangle(int x, int y);
	void TAG_Triangle(int x, int y);
	void TAG_Cross(int x, int y);
	void Sym_Circle(int x, int y);
	void Sym_Triangle(int x, int y);
	void Sym_Rect(int x, int y);
	void Sym_Cross(int x, int y);
	void Draw_Judge(JudgeInfo ji,int combo);
	void Draw_Explode(int,int,int);
	void DrawNotifications(string text);
	HWND hwnd;
	HANDLE stdhandle;
	HDC hdc;
	HFONT MUSICSYMBOL, MUSICSYMBOLBIG, BIGFONT, NORMALFONT, BIGBIGFONT;
	HPEN EDGELINE,CLR_EDGELINE,SYMBOLEDGE,EXPLODEEDGE,CIRCLE_EDGE,CIRCLE_SYM,TRIANGLE_SYM,RECT_SYM,CROSS_SYM, TRI_SYMBOLEDGE;
	HPEN CIRCLE_FLY, TRIANGLE_FLY, CROSS_FLY, RECTANGLE_FLY;
	HBRUSH NULLBRUSH,CLR_BRUSH,TAGBRUSH, BLOODBLUE, BLOODRED,REDDP,BLUEDP,PINKDP,GREENDP;
	COLORREF COOL, FINE, SAFE, BAD,WORST;
	RECT TITLERECT;
};
static Drawer _Drawer;
//继承出不同界面的Drawer出来,进入界面创建静态Drawer