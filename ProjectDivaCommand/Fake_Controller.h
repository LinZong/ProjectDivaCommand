#include <Windows.h>
#include "Launcher.h"
extern bool IsInGame, GameStartByController, GameStartByKeyBoard;
extern INPUT_RECORD MessageList[256];
extern void(*CurrentMessageHandler)(INPUT_RECORD,WORD);
extern void(*GamingKeyHandler)();
void SwitchKeyCheckStyle(bool);
void MaintainMessageQueue();
void MaintainJoystickQueue();
void MaintainGamingKeyQueue();
void NextStateEntry(VIEWDRAWER, VIEWKEYHANDLER); 
void LastStateEntry();
//多线程维护消息队列