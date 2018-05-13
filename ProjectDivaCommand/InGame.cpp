#include "InGame.h"
#include "SongsScanner.h"
#include "MapReader.h"
#include "SoundsPlayer.h"
#include "Launcher.h"
#include "Drawer.h"
#include "CharsetConverter.h"
#include "Fake_Controller.h"
#include "View_ScoreBoard.h"
#include "LyricViewer.h"
#include <queue>
#include <deque>
#include <thread>
#include <atomic>
#include <math.h>
#ifndef KEYSTYLE
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#endif
using namespace std;
//KEY STATUE DEFINE
int ComboCnt = 0, MisCnt = 0, BloodUpCnt = 0, MaxComboCnt = 0;
atomic<int> BloodStatus;
bool ComboStatus = false, HadInitLyric = false, StopGameMusic = false;
bool CIR = false, CRO = false, REC = false, TRI = false, LE = false, DO = false, RI = false, UP = false;
atomic<bool> ShouldUpdateHBitmap;
double XPixelSpeed, YPixelSpeed;
GameScore * gs = NULL;
vector<LrcLine>::iterator LrcIter;
LARGE_INTEGER StartTime, NowTime, PauseTime, PauseStart, PauseEnd, PerformanceCounter;
double LastGetLyricTime;
AudioClip * PlayerPtr;
MusicInfo PlayingMusicInfo;
HBITMAP* CurrentMemoryBitmap, *BlackBitmap;//For redraw
atomic<bool> IsPause;
atomic<double> nowtime;
queue<SymbolInfo> TakePosSymbols;
deque<SymbolInfo> FlyintoRenderSymbols;
deque<JudgeInfo> JudgeSymbols;
string CurrentLyric;
HelperHandler* Helper;
void DrawFrame()
{
	SelectObject(_Drawer.hdc, _Drawer.EDGELINE);
	MoveToEx(_Drawer.hdc, 0, 40, NULL);
	LineTo(_Drawer.hdc, 960, 40);
	MoveToEx(_Drawer.hdc, 100, 430, NULL);
	LineTo(_Drawer.hdc, 960, 430);
}
void ShowLyric()
{
	if (!IsInGame) return;
	double t = 1000 * nowtime;
	if (LrcIter == Lrc.begin() && !HadInitLyric)
	{
		CurrentLyric = LrcIter->LyricContent;
		LastGetLyricTime = t;
		ShouldUpdateHBitmap = true;
		HadInitLyric = true;
	}
	else if (LrcIter != (Lrc.end() - 1) && t - LastGetLyricTime > LrcIter->MSec)
	{
		LrcIter++;
		CurrentLyric = LrcIter->LyricContent;
		LastGetLyricTime = t;
		ShouldUpdateHBitmap = true;
	}
	else return;
}
void UpdateHBitmap()
{
	LoadLastBitmap(*BlackBitmap);
	DrawScore();
	DrawBlood();
	for (auto iter : FlyintoRenderSymbols)
	{
		DrawTag((int)iter.PositionPixelX, (int)iter.PositionPixelY, iter.Symbol);
	}
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	TextOut(_Drawer.hdc, 120, 440, (LPCSTR)CurrentLyric.c_str(), CurrentLyric.length());
	if (Helper && (Helper->HelpType))
	{
		string HelperRemainCnt = I2S(Helper->RemainTime);
		TextOut(_Drawer.hdc, 850, 440, (LPCSTR)HelperRemainCnt.c_str(), HelperRemainCnt.length());
	}
	SaveCurrentBitmap(*CurrentMemoryBitmap);
	ShouldUpdateHBitmap = false;
}
void StatusMonitor()
{
	if (MisCnt > 3)
	{
		BloodStatus--;
		MisCnt = 0;
		ShouldUpdateHBitmap = true;
	}
	if (!ComboStatus) BloodUpCnt = 0;
	else if (BloodStatus <= 10 && (ComboCnt / 5) > BloodUpCnt)
	{
		BloodStatus++;
		BloodUpCnt = (ComboCnt / 5);
	}
	if (BloodStatus == 0)
	{
		StopGame(true, true);
	}
}
void DrawTitle()
{
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	DrawText(_Drawer.hdc, (LPCSTR)PlayingMusicInfo.Name.c_str(), PlayingMusicInfo.Name.length(), &_Drawer.TITLERECT, DT_VCENTER | DT_SINGLELINE);
}
void DrawScore()
{
	SelectObject(_Drawer.hdc, _Drawer.NORMALFONT);
	string Show_Score = I2S(gs->TotalScore);
	DrawText(_Drawer.hdc, (LPCSTR)Show_Score.c_str(), Show_Score.length(), &_Drawer.TITLERECT, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
}
void DrawBlood()
{
	if (BloodStatus < 4)SelectObject(_Drawer.hdc, _Drawer.BLOODRED);
	else SelectObject(_Drawer.hdc, _Drawer.BLOODBLUE);
	SelectObject(_Drawer.hdc, _Drawer.EDGELINE);
	Ellipse(_Drawer.hdc, 0, 380, 100, 480);
	SelectObject(_Drawer.hdc, _Drawer.CLR_EDGELINE);
	SelectObject(_Drawer.hdc, _Drawer.CLR_BRUSH);
	Rectangle(_Drawer.hdc, 0, 380, 100, 430 - (BloodStatus - 5) * 10);
	SelectObject(_Drawer.hdc, _Drawer.EDGELINE);
	SelectObject(_Drawer.hdc, _Drawer.NULLBRUSH);
	Ellipse(_Drawer.hdc, 0, 380, 100, 480);
}
bool AllKeyup()
{
	bool a, b, c, d, e, f, g, h;
	a = KEYDOWN(87);
	b = KEYDOWN(68);
	c = KEYDOWN(83);
	d = KEYDOWN(65);
	e = KEYDOWN(VK_RIGHT);
	f = KEYDOWN(VK_UP);
	g = KEYDOWN(VK_DOWN);
	h = KEYDOWN(VK_LEFT);
	if (HasController)
	{
		auto Btn = Controller->GetState().Gamepad.wButtons;
		a = Btn & XINPUT_GAMEPAD_Y || a;
		b = Btn & XINPUT_GAMEPAD_B || b;
		c = Btn & XINPUT_GAMEPAD_A || c;
		d = Btn & XINPUT_GAMEPAD_X || d;
		e = Btn & XINPUT_GAMEPAD_DPAD_RIGHT || e;
		f = Btn & XINPUT_GAMEPAD_DPAD_UP || f;
		g = Btn & XINPUT_GAMEPAD_DPAD_DOWN || g;
		h = Btn & XINPUT_GAMEPAD_DPAD_LEFT || h;
	}
	return (!(a || b || c || d || e || f || g || h));
}
inline void StatusRender()
{
	ShowLyric();
	StatusMonitor();
}
void StartPlaygame(MusicInfo mi, queue<SymbolInfo> si)
{
	PauseStart.QuadPart = PauseEnd.QuadPart = PauseTime.QuadPart = 0;
	nowtime = 0;
	//检测使用的道具类型:
	Helper = new HelperHandler;
	switch (mi.WithHelp)
	{
	case 0:
		Helper->HelpType = 0;
		Helper->RemainTime = 0;
		break;
	case 1:
		Helper->HelpType = 1;
		Helper->RemainTime = 10;
		break;
	case 2:
		Helper->HelpType = 2;
		Helper->RemainTime = 10;
		break;
	default:
		break;
	}
	//初始化Combo计数器
	MisCnt = BloodUpCnt = MaxComboCnt = ComboCnt = 0;
	BloodStatus = 5;
	StopGameMusic = HadInitLyric = ComboStatus = IsPause = ShouldUpdateHBitmap = false;
	JudgeSymbols.clear();
	FlyintoRenderSymbols.clear();
	//获取性能计数器频率
	QueryPerformanceFrequency(&PerformanceCounter);
	//加载音乐
	PlayerPtr = new AudioClip;
	PlayerPtr->load(mi.FullPath);
	PlayingMusicInfo = mi;
	//保存谱面
	TakePosSymbols = si;
	LrcIter = Lrc.begin();
	LastGetLyricTime = 0;
	//Switch Controller
	//记录开始时间
	gs = new GameScore;
	gs->TotalScore = gs->MaxCombo = gs->IsFinished = gs->COOL = gs->FINE = gs->SAFE = gs->BAD = gs->WORSE = 0;
	QueryPerformanceCounter(&StartTime);
	PlayerPtr->play();
	::SetBkColor(_Drawer.hdc, RGB(0, 0, 0));
	::SetTextColor(_Drawer.hdc, RGB(255, 255, 255));

}
void SaveCurrentBitmap(HBITMAP &tar)
{
	HDC        hMemDC;
	HBITMAP    hBitmap, hOldBitmap;
	int       nWidth, nHeight;
	nWidth = _LauncherInfo._ConsoleWidowPixelSize.x;
	nHeight = _LauncherInfo._ConsoleWidowPixelSize.y;
	hMemDC = CreateCompatibleDC(_Drawer.hdc);
	hBitmap = CreateCompatibleBitmap(_Drawer.hdc, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	StretchBlt(hMemDC, 0, 0, nWidth, nHeight, _Drawer.hdc, 0, 0, nWidth, nHeight, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
	DeleteObject(hOldBitmap);
	tar = hBitmap;
}
void LoadLastBitmap(HBITMAP &load)
{
	HDC        hMemDC;
	int       nWidth, nHeight;
	nWidth = _LauncherInfo._ConsoleWidowPixelSize.x;
	nHeight = _LauncherInfo._ConsoleWidowPixelSize.y;
	hMemDC = CreateCompatibleDC(_Drawer.hdc);
	SelectObject(hMemDC, load);
	BitBlt(_Drawer.hdc, 0, 0, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);
	DeleteDC(hMemDC);
}
void RendeTakePlaceSymbol()
{
	while (IsInGame)
	{
		if (IsPause) continue;
		if (!TakePosSymbols.empty())
		{
			SymbolInfo front_queue = TakePosSymbols.front();

			if (Absolute(nowtime - front_queue.AppearHolderTime) < _LauncherInfo.TagsAppearTimeOffset)
			{
				FlyintoRenderSymbols.push_back(front_queue);
				TakePosSymbols.pop();
				ShouldUpdateHBitmap = true;
			}
			//Sleep(lockframe-rendertime);
		}
		nowtime = PastTime();
		RendeFlyintoSymbol();
		StatusRender();
		if (nowtime - (PlayerPtr->length_ms / 1000) > 2) StopGame(true, false);
	}
	return;
}
void RendeFlyintoSymbol()
{
	if (ShouldUpdateHBitmap)
	{
		UpdateHBitmap();
	}
	else LoadLastBitmap(*CurrentMemoryBitmap);
	for (auto iter : FlyintoRenderSymbols)
	{
		auto _this_past = nowtime - iter.FlyIntoTime;
		if (_this_past >= 0)
		{
			double cx, cy;
			switch (iter.FromEdge)
			{
			case 0:
				cy = iter.FlyIntoPixelY + _this_past * YPixelSpeed * cos(atan(iter.Min_tan)) * iter.FlyDirection[1];
				cx = iter.FlyIntoPixelX + _this_past * YPixelSpeed * sin(atan(iter.Min_tan)) * iter.FlyDirection[0];
				DrawSymbol(cx, cy, iter.Symbol);
				break;
			case 1:
				cx = iter.FlyIntoPixelX + _this_past * XPixelSpeed * cos(atan(iter.Min_tan)) * iter.FlyDirection[0];
				cy = iter.FlyIntoPixelY + _this_past * XPixelSpeed * sin(atan(iter.Min_tan)) * iter.FlyDirection[1];
				DrawSymbol(cx, cy, iter.Symbol);
				break;
			default:
				break;
			}
		}
	}
	if (!JudgeSymbols.empty())
	{
		if (!JudgeSymbols.empty() && nowtime - JudgeSymbols.front().GetTagTime <= 0.4)
		{
			_Drawer.Draw_Judge(JudgeSymbols.front(), ComboCnt);
			_Drawer.Draw_Explode(JudgeSymbols.front().PositionPixelX+30, JudgeSymbols.front().PositionPixelY+35, 40 + (JudgeSymbols.front().ExplodeCnt));
			JudgeSymbols.front().ExplodeCnt += 0.05;
		}
		else if (!JudgeSymbols.empty())JudgeSymbols.pop_front();
	}
	Judger();
}
void DrawSymbol(int x, int y, int type)
{
	switch (type)
	{
	case 0: //circle
		_Drawer.Sym_Circle(x, y);
		break;
	case 1: // cross
		_Drawer.Sym_Cross(x, y);
		break;
	case 2: //rectangle
		_Drawer.Sym_Rect(x, y);
		break;
	case 3: //triangle
		_Drawer.Sym_Triangle(x, y);
		break;
	case 4:
		_Drawer.Sym_DP_Right(x, y);
		break;
	case 5:
		_Drawer.Sym_DP_DOWN(x, y);
		break;
	case 6:
		_Drawer.Sym_DP_Left(x, y);
		break;
	case 7:
		_Drawer.Sym_DP_UP(x, y);
		break;
	default:
		break;
	}
}
void DrawTag(int x, int y, int type)
{
	switch (type)
	{
	case 0: //circle
		_Drawer.TAG_Circle(x, y);
		break;
	case 1: // cross
		_Drawer.TAG_Cross(x, y);
		break;
	case 2: //rectangle
		_Drawer.TAG_Rectangle(x, y);
		break;
	case 3: //triangle
		_Drawer.TAG_Triangle(x, y);
		break;
	case 4:
		_Drawer.TAG_DP_Right(x, y);
		break;
	case 5:
		_Drawer.TAG_DP_DOWN(x, y);
		break;
	case 6:
		_Drawer.TAG_DP_Left(x, y);
		break;
	case 7:
		_Drawer.TAG_DP_UP(x, y);
		break;
	default:
		break;
	}
}
void PauseGame()
{
	if (!IsPause)
	{
		//Stop render1 and 2
		PlayerPtr->pause();
		IsPause = true;
		QueryPerformanceCounter(&PauseStart);
		Sleep(100);
		_Drawer.DrawNotifications("暂停");
	}
	else
	{
		PlayerPtr->unpause();
		IsPause = false;
		QueryPerformanceCounter(&PauseEnd);
		PauseTime.QuadPart += PauseEnd.QuadPart - PauseStart.QuadPart;
		ShouldUpdateHBitmap = true;
	}
}
void StopGame(bool ToScoreBar, bool IsMistake)
{
	delete Helper;
	StopGameMusic = true;
	SwitchKeyCheckStyle(false);
	gs->IsFinished = !IsMistake;
	InvalidateRect(Global_HWND, NULL, false);
	system("cls");
	::DeleteObject(*CurrentMemoryBitmap);
	::DeleteObject(*BlackBitmap);
	delete CurrentMemoryBitmap;
	delete BlackBitmap;
	CurrentMemoryBitmap = NULL;
	BlackBitmap = NULL;
	if (ToScoreBar)
	{
		CalcScore(gs, PlayingMusicInfo);
		NextStateEntry(Score_Draw, ScoreBoard_KeyCheck);
	}
}
void InGame_KeyChecker(INPUT_RECORD ir, WORD js_word)
{
	if (ir.Event.KeyEvent.bKeyDown || js_word)
	{
		int dir = js_word != NULL ? js_word : ir.Event.KeyEvent.wVirtualKeyCode;
		switch (dir)
		{
		case XINPUT_GAMEPAD_BACK:
		case VK_ESCAPE:
			StopGame(false, false);
			Sleep(100);
			InvalidateRect(Global_HWND, NULL, true);
			LastStateEntry();
			break;
		case XINPUT_GAMEPAD_START:
		case VK_RETURN:
		{
			PauseGame();
			break;
		}
		case XINPUT_GAMEPAD_LEFT_SHOULDER:
		case 66://Fake
			StopGame(true, false);
		default:
			break;
		}
	}
}
void Gaming_KeyChecker()
{
	int cot1 = 0, cot2 = 0;
	while (IsInGame)
	{
		if (IsPause) continue;
		if (cot2 == 0)
		{
			RI = KEYDOWN(VK_RIGHT);
			UP = KEYDOWN(VK_UP);
			DO = KEYDOWN(VK_DOWN);
			LE = KEYDOWN(VK_LEFT);
			if (HasController)
			{
				auto Btn = Controller->GetState().Gamepad.wButtons;
				RI = Btn & XINPUT_GAMEPAD_DPAD_RIGHT || RI;
				UP = Btn & XINPUT_GAMEPAD_DPAD_UP || UP;
				DO = Btn & XINPUT_GAMEPAD_DPAD_DOWN || DO;
				LE = Btn & XINPUT_GAMEPAD_DPAD_LEFT || LE;
			}
			if (RI || UP || DO || LE) cot2++;
		}
		if (cot1 == 0)
		{
			TRI = KEYDOWN(87);
			CIR = KEYDOWN(68);
			CRO = KEYDOWN(83);
			REC = KEYDOWN(65);
			if (HasController)
			{
				auto Btn = Controller->GetState().Gamepad.wButtons;
				TRI = Btn & XINPUT_GAMEPAD_Y || TRI;
				CIR = Btn & XINPUT_GAMEPAD_B || CIR;
				CRO = Btn & XINPUT_GAMEPAD_A || CRO;
				REC = Btn & XINPUT_GAMEPAD_X || REC;
			}
			if (TRI || CIR || CRO || REC)
			{
				cot1++;
			}
		}
		if (AllKeyup())
		{
			TRI = CIR = CRO = REC = RI = UP = DO = LE = false;
			cot2 = cot1 = 0;
		}
	}
}
void ShowJudge(int type, SymbolInfo si)
{
	JudgeInfo ji;
	ji.Judge = type;
	ji.PositionPixelX = si.PositionPixelX - 30;
	ji.PositionPixelY = si.PositionPixelY - 35;
	ji.GetTagTime = nowtime;
	ji.ExplodeCnt = 0;
	JudgeSymbols.push_back(ji);
}
void Judger()
{
	if (FlyintoRenderSymbols.empty()) return;
	if (!IsInGame) return;
	if (IsPause) return;
	int type = FlyintoRenderSymbols.front().Symbol;
	double ThisjudgeStamp = nowtime;
	double Flytime = FlyintoRenderSymbols.front().HitCenterTime;
	double AbsoluteTime = Absolute(Flytime - ThisjudgeStamp);
	if (AbsoluteTime > 0.13)
	{
		if (nowtime - Flytime > 0.13)
		{
			//Worse
			MisCnt = MisCnt > 0 ? MisCnt + 1 : 1;
			if (Helper->HelpType == 1&&Helper->RemainTime>0)
			{
				Helper->RemainTime--;
				if (gs) gs->SAFE++;
				ShowJudge(2, FlyintoRenderSymbols.front());
			}
			else
			{
				if (gs) gs->WORSE++;
				ShowJudge(4, FlyintoRenderSymbols.front());
			}
			FlyintoRenderSymbols.pop_front();
			ComboCnt = ComboStatus = false;
			ShouldUpdateHBitmap = true;
			return;
		}
		else
		{
			return;
		}
	}
	switch (type)
	{
	case 0:
		if (CIR || RI)
		{
			GetScore(AbsoluteTime);
		}
		break;
	case 1:
		if (CRO || DO)
		{
			GetScore(AbsoluteTime);
		}
		break;
	case 2:
		if (REC || LE)
		{
			GetScore(AbsoluteTime);
		}
		break;
	case 3:
		if (TRI || UP)
		{
			GetScore(AbsoluteTime);
		}
		break;
	case 4:
		if (CIR && RI)
		{
			GetScore(AbsoluteTime);
		}
		break;
	case 5:
		if (CRO && DO)
		{
			GetScore(AbsoluteTime);
		}
		break;
	case 6:
		if (REC && LE)
		{
			GetScore(AbsoluteTime);
		}
		break;
	case 7:
		if (TRI && UP)
		{
			GetScore(AbsoluteTime);
		}
		break;
	default:
		break;
	}
}
void GetScore(double t)
{
	if (!gs) return;
	auto front = FlyintoRenderSymbols.front();
	if (!FlyintoRenderSymbols.empty())
	{
		FlyintoRenderSymbols.pop_front();
	}
	if (t < 0.03)
	{
		gs->COOL++;
		gs->TotalScore += 200;
		MisCnt = 0;
		ComboStatus = true;
		ShowJudge(0, front);
	}
	else if (0.03 <= t && t < 0.06)
	{
		gs->FINE++;
		gs->TotalScore += 150;
		MisCnt = 0;
		ComboStatus = true;
		ShowJudge(1, front);
	}
	else if (0.06 <= t && t < 0.09)
	{
		if (Helper->HelpType == 2 && Helper->RemainTime>0)
		{
			Helper->RemainTime--;
			gs->FINE++;
			gs->TotalScore += 150;
			MisCnt = 0;
			ComboStatus = true;
			ShowJudge(1, front);
		}
		else
		{
			gs->SAFE++;
			gs->TotalScore += 100;
			MisCnt = MisCnt > 0 ? MisCnt + 1 : 1;
			ComboCnt = ComboStatus = false;
			ShowJudge(2, front);
		}
	}
	else if (0.09 <= t && t < 0.13)
	{
		gs->BAD++;
		gs->TotalScore += 50;
		MisCnt = MisCnt > 0 ? MisCnt + 1 : 1;
		ComboCnt = ComboStatus = false;
		ShowJudge(3, front);
	}
	if (ComboStatus) ComboCnt++;
	gs->MaxCombo = MaxComboCnt = (ComboCnt > MaxComboCnt ? ComboCnt : MaxComboCnt);
	CIR = false, CRO = false, REC = false, TRI = false, LE = false, DO = false, RI = false, UP = false;
	ShouldUpdateHBitmap = true;
}
void Fake_Game_Drawer()
{
	CurrentMemoryBitmap = new HBITMAP;
	BlackBitmap = new HBITMAP;
	GamingKeyHandler = Gaming_KeyChecker;
	SwitchKeyCheckStyle(true);
	DrawFrame();
	DrawTitle();
	SaveCurrentBitmap(*BlackBitmap);
	SaveCurrentBitmap(*CurrentMemoryBitmap);
	DrawBlood();
	thread rendertags(RendeTakePlaceSymbol);
	rendertags.detach();
}
inline double PastTime()
{
	QueryPerformanceCounter(&NowTime);
	return (NowTime.QuadPart - StartTime.QuadPart - PauseTime.QuadPart)*1.0 / PerformanceCounter.QuadPart;
}