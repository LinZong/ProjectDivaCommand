#include <Windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include "SoundsPlayer.h"
#include "SongsScanner.h"
#include "View_RhythmGame.h"
#include "Drawer.h"
#include "SongsScanner.h"
#include "Fake_Controller.h"
#include "View_Ready.h"
#include "CharsetConverter.h"
using namespace std;
bool IsListeningMusic = false;
int Rhythm_CurrentSelect = 0;
int Rhythm_ArrowPosition = 0;
int R_CurrentPage = 0;
int R_cnt = 0;//Turn page Helper
int limit = 0;
int MusicLibrarySize = 0;
int UseHelp = 0;
vector<MusicInfo> Rhythm_View_MusicInfo;
AudioClip *ac = NULL;
string HelpType[3] = { ">","S","F" };
void Rhythm_Prelistening()
{
	if (!IsListeningMusic)
	{
		ac = new AudioClip;
		ac->load(Rhythm_View_MusicInfo.at(Rhythm_CurrentSelect).FullPath);
		IsListeningMusic = ac->play();
	}
	else
	{
		IsListeningMusic = !ac->stop();
		delete ac;
	}
}
void Rhythm_DrawFoot()
{
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	TextOut(_Drawer.hdc, 330, 450, (LPCSTR)"△试听  ○确定  □辅助道具  ×返回", 35);
}
void Rhythm_Draw()
{
	::SetBkColor(_Drawer.hdc, RGB(0, 0, 0));
	::SetTextColor(_Drawer.hdc, RGB(255, 255, 255));

	Rhythm_DrawBg();
	if (Rhythm_View_MusicInfo.empty()) Rhythm_GetMusicInfoFromScanner();
	else if (MusicsLibraryChanged)  Rhythm_GetMusicInfoFromScanner();
	Rhythm_Drawtitle();
	Rhythm_DrawMusicName(R_CurrentPage);
	if (!(Rhythm_CurrentSelect == MusicLibrarySize - 1))
	{
		Rhythm_DrawSelectionArrow(0);
		Rhythm_DrawSelectionArrow(1);
	}
	else
	{
		Rhythm_DrawSelectionArrow(0);
	}
	Rhythm_DrawFoot();
}

void Rhythm_Drawtitle()
{
	SelectObject(_Drawer.hdc, _Drawer.MUSICSYMBOLBIG);
	TextOutW(_Drawer.hdc, 45, 20, L"♬", 2);
	SelectObject(_Drawer.hdc, _Drawer.BIGFONT);
	TextOut(_Drawer.hdc, 100, 30, (LPCSTR)"乐曲选择", 9);
}

void Rhythm_DrawMusicName(int Page)
{
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	int ypos = 180;
	for (int i = Page; i < Page + limit; i++)
	{
		string tmp = Rhythm_View_MusicInfo[i].Name + "                  ";
		TextOut(_Drawer.hdc, 82, ypos, (LPCSTR)tmp.c_str(), tmp.length());
		ypos += 50;
	}
}
void Rhythm_DrawPlayInfo()
{
	if (Rhythm_View_MusicInfo.empty()) return;
	auto node = Rhythm_View_MusicInfo.at(Rhythm_CurrentSelect);
	if (node.PassTime > 0)
	{
		SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
		string j = node.Judge + "        ";
		string hs_r = "最高分 : " + I2S(node.HighestScore) + "        ";
		string ps_r = "通关次数 : " + I2S(node.PassTime) + "        ";
		TextOut(_Drawer.hdc, 730, 220, (LPCSTR)j.c_str(), j.length());
		TextOut(_Drawer.hdc, 730, 260, (LPCSTR)hs_r.c_str(), hs_r.length());
		TextOut(_Drawer.hdc, 730, 300, (LPCSTR)ps_r.c_str(), ps_r.length());
	}
	else
	{
		SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
		TextOut(_Drawer.hdc, 730, 220, (LPCSTR)"                        ", 25);
		TextOut(_Drawer.hdc, 730, 260, (LPCSTR)"                        ", 25);
		TextOut(_Drawer.hdc, 730, 300, (LPCSTR)"                            ", 29);
	}
}
void Rhythm_DrawSelectionArrow(int Position)
{
	if (Rhythm_View_MusicInfo.empty()) return;
	//这个负责箭头的转换和Turn page
	//0是向下，1是向上
	SetBkMode(_Drawer.hdc, OPAQUE);
	SelectObject(_Drawer.hdc, _Drawer.MUSICSYMBOL);
	for (int i = 0; i < limit; i++)
	{
		TextOutW(_Drawer.hdc, 50, 180 + 50 * i, L"♫", 2);
	}
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	switch (Position)
	{
	case 0:
		R_cnt++;
		Rhythm_CurrentSelect++;
		if (R_cnt > limit - 1)
		{
			R_CurrentPage++;
			if (R_CurrentPage > (Rhythm_View_MusicInfo.size() - limit)) R_CurrentPage = (Rhythm_View_MusicInfo.size() - limit);
			int msize = Rhythm_View_MusicInfo.size() - 1;
			Rhythm_CurrentSelect = Rhythm_CurrentSelect > msize ? msize : Rhythm_CurrentSelect;
			Rhythm_CurrentSelect = Rhythm_CurrentSelect < 0 ? 0 : Rhythm_CurrentSelect;
			Rhythm_DrawMusicName(R_CurrentPage);
			R_cnt = limit - 1;
		}
		if (R_cnt < 0) R_cnt = 0;
		Rhythm_DrawPlayInfo();
		TextOut(_Drawer.hdc, 50, 180 + 50 * R_cnt, (LPCSTR)HelpType[UseHelp % 3].c_str(), 2);
		break;
	case 1:
		R_cnt--;
		Rhythm_CurrentSelect--;
		if (R_cnt < 0)
		{
			R_CurrentPage--;
			if (R_CurrentPage < 0) R_CurrentPage = 0;
			int msize = Rhythm_View_MusicInfo.size() - 1;
			Rhythm_CurrentSelect = Rhythm_CurrentSelect > msize ? msize : Rhythm_CurrentSelect;
			Rhythm_CurrentSelect = Rhythm_CurrentSelect < 0 ? 0 : Rhythm_CurrentSelect;
			Rhythm_DrawMusicName(R_CurrentPage);
			R_cnt = 0;
		}
		Rhythm_DrawPlayInfo();
		TextOut(_Drawer.hdc, 50, 180 + 50 * R_cnt, (LPCSTR)HelpType[UseHelp%3].c_str(), 2);
		break;
	default:
		break;
	}
}
void Rhythm_GetMusicInfoFromScanner()
{
	if (!Rhythm_View_MusicInfo.empty()) Rhythm_View_MusicInfo.clear();
	if (!MusicsLibrary.empty())
	{
		for (auto i : MusicsLibrary)
		{
			Rhythm_View_MusicInfo.push_back(i.second);
		}
	}
	MusicLibrarySize = Rhythm_View_MusicInfo.size();
	limit = MusicLibrarySize > 5 ? 5 : MusicLibrarySize;
	MusicsLibraryChanged = false;
}
void Rhythm_KeyCheck(INPUT_RECORD ir, WORD js_word)
{
	if (ir.Event.KeyEvent.bKeyDown || js_word)
	{
		int dir = js_word != NULL ? js_word : ir.Event.KeyEvent.wVirtualKeyCode;
		switch (dir)
		{
		case XINPUT_GAMEPAD_Y:
		case 87://代表按下W键或者三角形键试听
		{
			Rhythm_Prelistening();
			break;
		}
		case XINPUT_GAMEPAD_X:
		case 65:
		{
			UseHelp++;
			TextOut(_Drawer.hdc, 50, 180 + 50 * R_cnt, (LPCSTR)HelpType[UseHelp % 3].c_str(), 2);
			break;
		}
		case XINPUT_GAMEPAD_DPAD_DOWN:
		case VK_DOWN:
			Rhythm_DrawSelectionArrow(0);
			break;
		case XINPUT_GAMEPAD_DPAD_UP:
		case VK_UP:
			Rhythm_DrawSelectionArrow(1);
			break;
		case XINPUT_GAMEPAD_A:
		case VK_ESCAPE:
		{
			InvalidateRect(Global_HWND, NULL, true);
			if (IsListeningMusic) Rhythm_Prelistening();
			LastStateEntry();
			break;
		}
		case XINPUT_GAMEPAD_B:
		case VK_RETURN:
		{
			if (!Rhythm_View_MusicInfo.empty())
			{
				InvalidateRect(Global_HWND, NULL, true);
				if (IsListeningMusic) Rhythm_Prelistening();
				Rhythm_View_MusicInfo.at(Rhythm_CurrentSelect).WithHelp = UseHelp % 3;
				ReadyGame(Rhythm_View_MusicInfo.at(Rhythm_CurrentSelect));
				NextStateEntry(Ready_Draw, Ready_KeyCheck);
			}
			break;
		}
		default:
			break;
		}
	}
}

void Rhythm_DrawBg()
{
	HBITMAP hbm;
	HDC hdcBits;
	BITMAP bm;
	hbm = (HBITMAP)LoadImage(0, "Resources\\RhythmBG.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if (hbm) {
		hdcBits = CreateCompatibleDC(Global_HDC);
		GetObject(hbm, sizeof(BITMAP), &bm);
		SelectObject(hdcBits, hbm);
		BitBlt(Global_HDC, 330, 170, bm.bmWidth, bm.bmHeight, hdcBits, 0, 0, SRCCOPY);
		DeleteDC(hdcBits);
		DeleteObject(hbm);
	}
}
