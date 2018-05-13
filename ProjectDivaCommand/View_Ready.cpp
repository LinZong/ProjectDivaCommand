#include "View_Ready.h"
#include "MapReader.h"
#include "SongsScanner.h"
#include "SoundsPlayer.h"
#include "Drawer.h"
#include "Launcher.h"
#include "CharsetConverter.h"
#include "Fake_Controller.h"
#include "InGame.h"
#include "LyricViewer.h"
#include <string>
#include <queue>
using namespace std;
queue<SymbolInfo> WillPlayMusicSymbolInfo;
MusicInfo _WillPlayMusic;
void ReadyGame(MusicInfo WillPlayMusic)
{
	//test read music
	_WillPlayMusic = WillPlayMusic;
	ReadLyricFiles((char*)WillPlayMusic.Lyric.c_str());
	AudioClip ac;
	ac.load(WillPlayMusic.FullPath);
	int MusicLen = ac.length_ms;
	GetCalcInfo(WillPlayMusic, MusicLen);
	//read map
	WillPlayMusicSymbolInfo = BuildSymbolQueue();
}

void Ready_Draw()
{
	::SetBkColor(_Drawer.hdc, RGB(0, 0, 0));
	::SetTextColor(_Drawer.hdc, RGB(255, 255, 255));
	Ready_Drawtitle();
	Ready_DrawSelection();
}

void Ready_Drawtitle()
{
	RECT rect;
	GetClientRect(Global_HWND, &rect);
	SelectObject(_Drawer.hdc, _Drawer.BIGFONT);
	rect.top += 50;
	DrawText(_Drawer.hdc, (LPCSTR)_WillPlayMusic.Name.c_str(), _WillPlayMusic.Name.length(), &rect, DT_CENTER | DT_SINGLELINE);
	rect.top += 80;
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	rect.top += 170;
	string passt;
	passt = "通关次数 : " + I2S(_WillPlayMusic.PassTime);
	DrawText(_Drawer.hdc, (LPCSTR)passt.c_str(), passt.length(), &rect, DT_CENTER | DT_SINGLELINE);
}
void Ready_DrawSelection()
{
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	TextOut(_Drawer.hdc, 390, 450, (LPCSTR)"○确定              ×返回", 27);
}
void Ready_KeyCheck(INPUT_RECORD ir, WORD js_word)
{
	if (ir.Event.KeyEvent.bKeyDown || js_word)
	{
		int dir = js_word != NULL ? js_word : ir.Event.KeyEvent.wVirtualKeyCode;
		switch (dir)
		{
		case XINPUT_GAMEPAD_A:
		case VK_ESCAPE:
		{
			InvalidateRect(Global_HWND, NULL, true);
			LastStateEntry();
			break;
		}
		case XINPUT_GAMEPAD_B:
			GameStartByController = true;
		case VK_RETURN:
		{
			if (!GameStartByController) GameStartByKeyBoard = true;
			InvalidateRect(Global_HWND, NULL, true);
			StartPlaygame(_WillPlayMusic, WillPlayMusicSymbolInfo);
			NextStateEntry(Fake_Game_Drawer, InGame_KeyChecker);
			break;
		}
		default:
			break;
		}
	}
}