#include <Windows.h>
#include <iostream>
#include <thread>
#include "Launcher.h"
#include "Fake_Controller.h"
#include "View_Welcome.h"
#include "InGame.h"
using namespace std;
HANDLE hStdin;
DWORD cNumRead, fdwMode, fdwSaveOldMode, i;
INPUT_RECORD irInBuf[256];
void(*CurrentMessageHandler)(INPUT_RECORD ir, WORD js_word) = NULL;
void(*GamingKeyHandler)() = NULL;
int cot = 0;
bool IsInGame = false,GameStartByController=false,GameStartByKeyBoard=false;
void SwitchKeyCheckStyle(bool ToGame)
{
	switch (ToGame)
	{
	case true:
	{
		IsInGame = true;
		thread gamingkey(MaintainGamingKeyQueue);
		gamingkey.detach();
		break;
	}
	case false:
	{
		IsInGame = false;
		//重新启动正常窗口的事件检测   使得其他退出
		break;
	}
	default:
		break;
	}
}
void MaintainGamingKeyQueue()
{
	while (1 && IsInGame && GamingKeyHandler)
	{
		GamingKeyHandler();
	}
	return;
}
void MaintainMessageQueue()
{
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	while (1 && GameIsRunning)
	{
		if (StopGameMusic&&GameStartByKeyBoard)
		{
			PlayerPtr->stop();
			GameStartByController = GameStartByKeyBoard = StopGameMusic = false;
		}
		// Wait for the events. 
		if (!ReadConsoleInput(
			hStdin,      // input buffer handle 
			irInBuf,     // buffer to read into 
			256,         // size of read buffer 
			&cNumRead))
			break;// number of records read 
		for (i = 0; i < cNumRead; i++)
		{
			if (CurrentMessageHandler != NULL)
			{
				CurrentMessageHandler(irInBuf[i], NULL);
			}
		}
	}
	
}
void MaintainJoystickQueue()
{
	while (1 && GameIsRunning)
	{
		if (StopGameMusic&&GameStartByController)
		{
			PlayerPtr->stop();
			GameStartByKeyBoard = GameStartByController = StopGameMusic = false;
		}
		int Message = Controller->GetState().Gamepad.wButtons;
		if (!Message)
		{
			cot = 0;
		}
		else if (Message && cot == 0 && CurrentMessageHandler != NULL)
		{
			cot++;
			INPUT_RECORD r = irInBuf[0];
			CurrentMessageHandler(r, Message);
		}
		Sleep(20);
	}
}
void NextStateEntry(VIEWDRAWER vr, VIEWKEYHANDLER vkh)
{
	StatueTree.push(make_pair(vr, vkh));
	CurrentMessageHandler = vkh;
	Sleep(20);
	vr();
}
void LastStateEntry()
{
	if (StatueTree.empty()) GameIsRunning = false;
	else
	{
		StatueTree.pop();
		CurrentMessageHandler = StatueTree.top().second;
		auto VDrawer = StatueTree.top().first;
		Sleep(20);
		VDrawer();
	}
}
