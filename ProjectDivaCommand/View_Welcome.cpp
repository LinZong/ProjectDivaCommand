#include <Windows.h>
#include "SavedataManager.h"
#include "View_Welcome.h"
#include "Drawer.h"
#include "View_SaveData.h"
#include "Fake_Controller.h"
#include "Launcher.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
int Wel_CurrentSelected = 0;
void PlayBGM()
{
	PlaySound((LPCSTR)"Resources/Bg.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}
void StopBGM()
{
	PlaySound(NULL, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}
//0 New game 1 LoadGame
void Wel_Draw()
{
	Wel_DrawTitle();
	Wel_DrawSelection();
	Wel_DrawFoot();
	PlayBGM();
}

void Wel_DrawTitle()
{
	HBITMAP hbm;
	HDC hdcBits;
	BITMAP bm;
	hbm = (HBITMAP)LoadImage(0, "Resources\\Welcome_Titile.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if (hbm) {
		hdcBits = CreateCompatibleDC(Global_HDC);
		GetObject(hbm, sizeof(BITMAP), &bm);
		SelectObject(hdcBits, hbm);
		BitBlt(Global_HDC, 170, 10, bm.bmWidth, bm.bmHeight, hdcBits, 0, 0, SRCCOPY);
		DeleteDC(hdcBits);
		DeleteObject(hbm);
	}
}

void Wel_DrawSelection()
{
	::SetBkColor(_Drawer.hdc, RGB(0, 0, 0));
	::SetTextColor(_Drawer.hdc, RGB(255, 255, 255));
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	TextOut(_Drawer.hdc, 250, 370, (LPCSTR)"NEW GAME", 9);
	TextOut(_Drawer.hdc, 600, 370, (LPCSTR)"LOAD GAME", 10);
	SelectObject(_Drawer.hdc, _Drawer.EDGELINE);
	SelectObject(_Drawer.hdc, _Drawer.NULLBRUSH);
}

void Wel_DrawFoot()
{

}

void Wel_SelectionEdge(int direction)
{
	//0 is to rifht  1 is to left
	switch (direction)
	{
	case 0:
		SelectObject(_Drawer.hdc, _Drawer.CLR_EDGELINE);
		Rectangle(_Drawer.hdc, 238, 365, 238 + 150, 365 + 40);
		SelectObject(_Drawer.hdc, _Drawer.EDGELINE);
		Rectangle(_Drawer.hdc, 238 + 353, 365, 238 + 353 + 150, 365 + 40);
		break;
	case 1:
		SelectObject(_Drawer.hdc, _Drawer.CLR_EDGELINE);
		Rectangle(_Drawer.hdc, 238 + 353, 365, 238 + 353 + 150, 365 + 40);
		SelectObject(_Drawer.hdc, _Drawer.EDGELINE);
		Rectangle(_Drawer.hdc, 238, 365, 238 + 150, 365 + 40);
		break;
	default:
		break;
	}
}

void Wel_KeyCheck(INPUT_RECORD ir,WORD js_word)
{
	if (ir.Event.KeyEvent.bKeyDown || js_word)
	{
		int dir = js_word != NULL ? js_word : ir.Event.KeyEvent.wVirtualKeyCode;
		switch (dir)
		{
		case XINPUT_GAMEPAD_DPAD_RIGHT:
		case VK_RIGHT:
		{
			Wel_SelectionEdge(0);
			Wel_CurrentSelected = (Wel_CurrentSelected >= 1 ? 1 : Wel_CurrentSelected + 1);
			break;
		}
		case XINPUT_GAMEPAD_DPAD_LEFT:
		case VK_LEFT:
		{
			Wel_CurrentSelected = (Wel_CurrentSelected <= 0 ? 0 : Wel_CurrentSelected - 1);
			Wel_SelectionEdge(1);
			break;
		}
		case XINPUT_GAMEPAD_B:
		case VK_RETURN:
		{
			//进行状态转移
			switch (Wel_CurrentSelected)
			{
			case 0:
			{
				LoadSaveDataToArray();
				if (SavedataCount > 8)
				{
					string Tips = "不允许新建超过9个存档";
					MessageBox(Global_HWND, (LPCSTR)Tips.c_str(), "Failed", MB_OK);
				}
				else
				{
					CreateSaveData();
					string Tips = "已成功创建存档!";
					MessageBox(Global_HWND, (LPCSTR)Tips.c_str(), "Successful", MB_OK);
				}
				break;
			}
			case 1:
			{
				InvalidateRect(Global_HWND, NULL, true);
				NextStateEntry(Save_Draw, Save_KeyCheck);
				StopBGM();
			}
			default:
				break;
			}
		}
		default:
			break;
		}
	}
}
