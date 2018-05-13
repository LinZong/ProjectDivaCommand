#pragma once
#include "SongsScanner.h"
#include "MapReader.h"
#include "SoundsPlayer.h"
#include <queue>
#include <Windows.h>
#include <string>
using namespace std;
struct GameScore
{
	int COOL;
	int FINE;
	int SAFE;
	int BAD;
	int WORSE;
	int MaxCombo;
	int IsFinished;
	int TotalScore;
};
struct JudgeInfo
{
	double PositionPixelX;
	double PositionPixelY;
	int Judge;
	double GetTagTime;
	double ExplodeCnt;
	//计算出现占位符时间 (AtSection-2)*OneSectionTime + AtTag*(OneTagTime)
	//计算开始飞入Symbol时间 出现占位符时间+(0.25)*(OneBeatTime)*(PositionY+1)
	//正中时间  (AtSection-1)*OneSectionTime+AtTag*(OneTagTime)
};
struct HelperHandler
{
	int HelpType;
	int RemainTime;
};
extern double XPixelSpeed, YPixelSpeed;
extern bool StopGameMusic;
extern AudioClip * PlayerPtr;
bool AllKeyup();
void StatusRender();
void StartPlaygame(MusicInfo, queue<SymbolInfo>);
void SaveCurrentBitmap(HBITMAP &);
void LoadLastBitmap(HBITMAP &);
void RendeTakePlaceSymbol();
void RendeFlyintoSymbol();
void PauseGame();
void DrawFrame();
void ShowLyric();
void UpdateHBitmap();
void StatusMonitor();
void DrawTitle();
void DrawScore();
void DrawBlood();
void DrawSymbol(int x, int y, int type);
void DrawTag(int x, int y, int type);
void InGame_KeyChecker(INPUT_RECORD ir, WORD js_word);
void Fake_Game_Drawer();
void GetScore(double);
void StopGame(bool, bool);
double PastTime();
void Judger();
void Gaming_KeyChecker();
void ShowJudge(int, SymbolInfo);