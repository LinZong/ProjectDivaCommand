#include "View_ScoreBoard.h"
#include "Drawer.h"
#include "InGame.h"
#include "CharsetConverter.h"
#include "Fake_Controller.h"
#include "Launcher.h"
#include "View_Ready.h"
#include "SavedataManager.h"
#include "SongsScanner.h"
#include <Windows.h>
#include <string>
using namespace std;
int total = 0, good = 0, MaxC = 0, totalscore = 0;
string judge;
GameScore *gs_res;
MusicInfo playMusic;
void CalcScore(GameScore* gs,MusicInfo playedMusic)
{
	gs_res = gs;
	playMusic = playedMusic;
	total = gs->COOL + gs->FINE + gs->SAFE + gs->BAD + gs->WORSE;
	totalscore = gs->TotalScore;
	good = gs->COOL + gs->FINE;
	MaxC = gs->MaxCombo;
	playMusic.PassTime += gs->IsFinished ? 1 : 0;
	double percent = (double)good / total;
	if (gs->IsFinished == 0) judge = "MISTAKE";
	else
	{
		if (percent == 1)
			judge = "PERFECT";
		else if (percent >= 0.90)
			judge = "EXCELLENT";
		else if (percent >= 0.85)
			judge = "GREAT";
		else if (percent >= 0.6)
			judge = "STANDARD";
		else judge = "CHEAP";
	}
	if (playMusic.HighestScore < totalscore)
	{
		playMusic.HighestScore = totalscore;
		playMusic.Judge = judge;
	}
	UpdateSaveData(playMusic);
}
void Score_Draw()
{
	::SetBkColor(_Drawer.hdc, RGB(0, 0, 0));
	::SetTextColor(_Drawer.hdc, RGB(255, 255, 255));
	Score_DrawBoard();
}
void Score_DrawBoard()
{
	string tot = I2S(total);
	string maxc = I2S(MaxC);
	string cool = I2S(gs_res->COOL);
	string fine = I2S(gs_res->FINE);
	string safe = I2S(gs_res->SAFE);
	string bad = I2S(gs_res->BAD);
	string worst = I2S(gs_res->WORSE);
	string score = I2S(totalscore);
	SelectObject(_Drawer.hdc, _Drawer.NULLBRUSH);
	SelectObject(_Drawer.hdc, _Drawer.EDGELINE);
	Rectangle(_Drawer.hdc, 10, 10, 10 + 500, 10 + 460);
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	RECT rect, rect2;
	rect.left = 10 + 10;
	rect.top = 10 + 10;
	rect.right = 510 - 10;
	rect.bottom = 470 - 10;
	DrawText(_Drawer.hdc, (LPCSTR)"Total Symbols : ", 17, &rect, DT_LEFT | DT_SINGLELINE);
	DrawText(_Drawer.hdc, (LPCSTR)tot.c_str(), tot.length(), &rect, DT_RIGHT | DT_SINGLELINE);
	rect.top += 50;
	DrawText(_Drawer.hdc, (LPCSTR)"Total Scores : ", 16, &rect, DT_LEFT | DT_SINGLELINE);
	DrawText(_Drawer.hdc, (LPCSTR)score.c_str(), score.length(), &rect, DT_RIGHT | DT_SINGLELINE);
	rect.top += 50;
	DrawText(_Drawer.hdc, (LPCSTR)"Max Combos : ", 14, &rect, DT_LEFT | DT_SINGLELINE);
	DrawText(_Drawer.hdc, (LPCSTR)maxc.c_str(), maxc.length(), &rect, DT_RIGHT | DT_SINGLELINE);
	rect.top += 50;
	::SetTextColor(_Drawer.hdc, _Drawer.COOL);
	DrawText(_Drawer.hdc, (LPCSTR)"COOL : ", 8, &rect, DT_LEFT | DT_SINGLELINE);
	DrawText(_Drawer.hdc, (LPCSTR)cool.c_str(), cool.length(), &rect, DT_RIGHT | DT_SINGLELINE);
	rect.top += 50;
	::SetTextColor(_Drawer.hdc, _Drawer.FINE);
	DrawText(_Drawer.hdc, (LPCSTR)"FINE : ", 8, &rect, DT_LEFT | DT_SINGLELINE);
	DrawText(_Drawer.hdc, (LPCSTR)fine.c_str(), fine.length(), &rect, DT_RIGHT | DT_SINGLELINE);
	rect.top += 50;
	::SetTextColor(_Drawer.hdc, _Drawer.SAFE);
	DrawText(_Drawer.hdc, (LPCSTR)"SAFE : ", 8, &rect, DT_LEFT | DT_SINGLELINE);
	DrawText(_Drawer.hdc, (LPCSTR)safe.c_str(), safe.length(), &rect, DT_RIGHT | DT_SINGLELINE);
	rect.top += 50;
	::SetTextColor(_Drawer.hdc, _Drawer.BAD);
	DrawText(_Drawer.hdc, (LPCSTR)"BAD : ", 7, &rect, DT_LEFT | DT_SINGLELINE);
	DrawText(_Drawer.hdc, (LPCSTR)bad.c_str(), bad.length(), &rect, DT_RIGHT | DT_SINGLELINE);
	rect.top += 50;
	::SetTextColor(_Drawer.hdc, _Drawer.WORST);
	DrawText(_Drawer.hdc, (LPCSTR)"WORST : ", 9, &rect, DT_LEFT | DT_SINGLELINE);
	DrawText(_Drawer.hdc, (LPCSTR)worst.c_str(), worst.length(), &rect, DT_RIGHT | DT_SINGLELINE);
	rect.top += 50;
	::SetTextColor(_Drawer.hdc, RGB(255, 255, 255));
	rect2.left = 550;
	rect2.right = 950;
	rect2.top = 200;
	rect2.bottom = 450;
	SelectObject(_Drawer.hdc, _Drawer.BIGBIGFONT);
	DrawText(_Drawer.hdc, (LPCSTR)judge.c_str(), judge.length(), &rect2, DT_CENTER | DT_SINGLELINE);
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	rect2.top += 150;
	DrawText(_Drawer.hdc, (LPCSTR)"¡ð  È·¶¨", 9, &rect2, DT_CENTER | DT_SINGLELINE);
}
void ScoreBoard_KeyCheck(INPUT_RECORD ir, WORD js_word)
{
	if (ir.Event.KeyEvent.bKeyDown || js_word)
	{
		int dir = js_word != NULL ? js_word : ir.Event.KeyEvent.wVirtualKeyCode;
		switch (dir)
		{
		case XINPUT_GAMEPAD_B:
		case VK_RETURN:
		{
			InvalidateRect(Global_HWND, NULL, true);
			while (StatueTree.top().first!= Ready_Draw)
			{
				StatueTree.pop();
			}
			LastStateEntry();
			break;
		}
		default:
			break;
		}
	}
}