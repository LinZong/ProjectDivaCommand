#ifndef LAUNCHER_H_
#define LAUNCHER_H_
#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include <stack>
#include "CXBOXController.h"
using namespace std;
struct Resolution
{
	int x;
	int y;
	int bufferx;
	int buffery;
};
struct LauncherInfo
{
	double TagsAppearTimeOffset;
	vector<string> SongsPath;
	string SaveDataPath;
	double ButtonDelay;
	Resolution StartResolution;
	struct ConsoleWindowSize
	{
		int x;
		int y;
	} _ConsoleWindowSize;
	struct ConsoleBufferSize
	{
		int x;
		int y;
	} _ConsoleBufferSize;
	struct ConsoleWindowPixelSize
	{
		int x;
		int y;
	} _ConsoleWidowPixelSize;
};
extern RECT Client_Rect;
extern bool HasController;
extern CXBOXController* Controller;
extern bool GameIsRunning;
typedef void(*VIEWDRAWER)(void);
typedef void(*VIEWKEYHANDLER)(INPUT_RECORD,WORD);
extern stack<pair<VIEWDRAWER, VIEWKEYHANDLER>> StatueTree;
extern HWND Global_HWND;
extern HDC Global_HDC;
extern HANDLE Global_OUT_HANDLE;
extern LauncherInfo _LauncherInfo;
extern char* ConfigPath;
void CheckJoystick();
void GetEnvInfo();
void LoadConfig();
void BuildDefaultLauncherInfo();
#endif // !LAUNCHER_H_

//ÕÒÑ°configÎÄ¼þ