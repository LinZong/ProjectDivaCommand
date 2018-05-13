#include "View_SaveData.h"
#include "Drawer.h"
#include "SavedataManager.h"
#include "Fake_Controller.h"
#include "View_RhythmGame.h"
#include <Windows.h>
int CurrentPage = 0;
int cnt = 0;//Turn page Helper
int CurrentSelected = 0;
bool HadInitDatabase = false;
int sv_limit;
void Save_Draw()
{
	::SetBkColor(_Drawer.hdc, RGB(0, 0, 0));
	::SetTextColor(_Drawer.hdc, RGB(255, 255, 255));
	if (!HadInitDatabase) InitialDatabase();
	if(CurrentMessageHandler!=Save_KeyCheck) CurrentMessageHandler = Save_KeyCheck;
	sv_limit = SavedataCount > 3 ? 3 : SavedataCount;
	Save_Drawtitle();
	Save_DrawDataName(CurrentPage);
	if (!(CurrentSelected == SavedataCount - 1))
	{
		Save_DrawSelectionEdge(0);
		Save_DrawSelectionEdge(1);
	}
	else Save_DrawSelectionEdge(0);
}
void Save_Drawtitle()
{
	SelectObject(_Drawer.hdc, _Drawer.BIGFONT);
	TextOut(_Drawer.hdc, 320, 60, (LPCSTR)"Select Savedata", 16);
}
void Save_DrawDataName(int Page)
{
	SelectObject(_Drawer.hdc, _Drawer.BIGFONT);
	int ypos = 200;
	for (int i = Page; i < Page + sv_limit; i++)
	{
		TextOut(_Drawer.hdc, 410, ypos, (LPCSTR)AllSaveDataName[i].c_str(), AllSaveDataName[i].length());
		ypos += 90;
	}
	SelectObject(_Drawer.hdc,_Drawer.EDGELINE);
	SelectObject(_Drawer.hdc, _Drawer.NULLBRUSH);
}
void Save_DrawSelectionEdge(int direction)
{
	if (SavedataCount == 0) return;
	switch (direction)
	{
		//0 向xia 1向shang
	case 0:
		SelectObject(_Drawer.hdc, _Drawer.CLR_EDGELINE);
		Rectangle(_Drawer.hdc, 386, 194 + 86 * cnt, 386 + 170, 194 + 86 * cnt + 72);
		cnt++;
		CurrentSelected++;
		if (cnt>sv_limit-1)
		{
			CurrentPage++;
			if (CurrentPage > (SavedataCount - sv_limit)) CurrentPage = (SavedataCount - sv_limit);
			Save_DrawDataName(CurrentPage);
			cnt = sv_limit-1;
		}
		SelectObject(_Drawer.hdc, _Drawer.EDGELINE);
		Rectangle(_Drawer.hdc, 386, 194 + 86 * cnt, 386 + 170, 194 + 86 * cnt + 72);
		break;
	case 1:
		SelectObject(_Drawer.hdc, _Drawer.CLR_EDGELINE);
		Rectangle(_Drawer.hdc, 386, 194 + 86 * cnt, 386 + 170, 194 + 86 * cnt + 72);
		SelectObject(_Drawer.hdc, _Drawer.EDGELINE);
		cnt--;
		if (cnt < 0)
		{
			CurrentPage--;
			if (CurrentPage<0) CurrentPage = 0;
			Save_DrawDataName(CurrentPage);
			cnt = 0;
		}
		CurrentSelected--;
		Rectangle(_Drawer.hdc, 386, 194 + 86 * cnt, 386 + 170, 194 + 86 * cnt + 72);
		break;
	default:
		break;
	}
	CurrentSelected = CurrentSelected > SavedataCount - 1 ? SavedataCount - 1 : CurrentSelected;
	CurrentSelected = CurrentSelected < 0 ? 0 : CurrentSelected;
}
void Save_KeyCheck(INPUT_RECORD ir,WORD js_word)
{
	if (ir.Event.KeyEvent.bKeyDown || js_word)
	{
		int dir = js_word != NULL ? js_word : ir.Event.KeyEvent.wVirtualKeyCode;
		switch (dir)
		{
			//delete savedata
		case XINPUT_GAMEPAD_Y:
		case VK_DELETE:
		{
			if (SavedataCount == 0) break;
			string DeleteTips = "是否确认删除 " + AllSaveDataName[CurrentSelected];
			auto Result = MessageBox(Global_HWND, (LPCSTR)DeleteTips.c_str(), "存档删除确认", MB_OKCANCEL);
			if (Result == 1)
			{
				DeleteSaveData(AllSaveDataName[CurrentSelected]);
				MessageBox(Global_HWND, (LPCSTR)"已成功删除存档", "存档删除完成", MB_OK);
				LoadSaveDataToArray();
				InvalidateRect(Global_HWND, NULL, true);
				Sleep(30);
				Save_Draw();
			}
			break;
		}
		case XINPUT_GAMEPAD_DPAD_DOWN:
		case VK_DOWN:
			Save_DrawSelectionEdge(0);
			break;
		case XINPUT_GAMEPAD_DPAD_UP:
		case VK_UP:
			Save_DrawSelectionEdge(1);
			break;
		case XINPUT_GAMEPAD_A:
		case VK_ESCAPE:
		{
			InvalidateRect(Global_HWND, NULL, true);
			LastStateEntry();
			break;
		}
		case XINPUT_GAMEPAD_B:
		case VK_RETURN:
		{
			if (SavedataCount > 0)
			{
				InvalidateRect(Global_HWND, NULL, true);
				LoadSaveData(CurrentSelected);
				string Tips = "已成功加载存档  " + AllSaveDataName[CurrentSelected];
				MusicsLibraryChanged = true;
				MessageBox(Global_HWND, (LPCSTR)Tips.c_str(), "Successful", MB_OK);
				NextStateEntry(Rhythm_Draw, Rhythm_KeyCheck);
			}
			break;
		}
		default:
			break;
		}
	}
}