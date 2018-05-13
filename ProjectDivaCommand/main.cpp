#include <Windows.h>
#include <thread>
#include "Launcher.h"
#include "Fake_Controller.h"
#include "Drawer.h"
#include "View_Welcome.h"
#include "SoundsPlayer.h"
using namespace std;
void KeepAlive()
{
	while (true)
	{
		Sleep(INT_MAX);
	}
}
int main(int argc,char **argv)
{
	SetConsoleTitle("Hatsune Miku Project Diva Command Prompt Version");
	GetEnvInfo();
	LoadConfig();
	thread keyhandler(MaintainMessageQueue);
	keyhandler.detach();
	if (HasController)
	{
		thread ControllerHandler(MaintainJoystickQueue);
		ControllerHandler.detach();
	}
	NextStateEntry(Wel_Draw, Wel_KeyCheck);
	KeepAlive();
	return 0;
}

