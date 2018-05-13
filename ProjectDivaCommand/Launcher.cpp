#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include "Launcher.h"
#include "json\json.h"
#include "SongsScanner.h"
#include "SavedataManager.h"
using namespace std;
stack<pair<VIEWDRAWER,VIEWKEYHANDLER>> StatueTree;
LauncherInfo _LauncherInfo;
char* ConfigPath;
HWND Global_HWND;
HDC Global_HDC;
RECT Client_Rect;
HANDLE Global_OUT_HANDLE;
CXBOXController* Controller;
bool HasController = false;
bool GameIsRunning = false;
void CheckJoystick()
{
	Controller = new CXBOXController(1);
	HasController = Controller->IsConnected();
	if (HasController)
	{
		MessageBox(Global_HWND, (LPCSTR)"��⵽�������˿�����.�����ڿ���ʹ�ÿ��������汾��.��ע��,��Ȼ����Ϸ������֧�ֿ������ͼ���ͬʱ����,������Ȼ������ȫ��ֻѡ��һ�������豸���в���.", (LPCSTR)"��⵽������", MB_OK);
	}
}
void GetEnvInfo()
{
	Global_HWND = GetConsoleWindow();
	Global_HDC = GetDC(Global_HWND);
	Global_OUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	_LauncherInfo._ConsoleWindowSize.x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	_LauncherInfo._ConsoleWindowSize.y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	_LauncherInfo._ConsoleBufferSize.x = csbi.dwSize.X;
	_LauncherInfo._ConsoleBufferSize.y = csbi.dwSize.Y;
	//GetClientRect(Global_HWND, &Client_Rect);
	Client_Rect.left = Client_Rect.top = 0;
	Client_Rect.right = 960;
	Client_Rect.bottom = 490;
	_LauncherInfo._ConsoleWidowPixelSize.x = Client_Rect.right - Client_Rect.left;
	_LauncherInfo._ConsoleWidowPixelSize.y = Client_Rect.bottom - Client_Rect.top;
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
	//�ر������С��
	auto SysMenu = GetSystemMenu(Global_HWND, false);
	DeleteMenu(SysMenu, SC_MINIMIZE, MF_BYCOMMAND);
	DeleteMenu(SysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
	DeleteMenu(SysMenu, SC_SIZE, MF_BYCOMMAND);
}
void LoadConfig()
{
	GetEnvInfo();
	ConfigPath = (char*)"config/GlobalConfig.json";
	ifstream fs;
	fs.open(ConfigPath, ios::binary);
	if (!fs.is_open())
	{
		cout << "�޷���ϵͳ�����ļ���������Ĭ��������������ϵͳ�ļ�..." << endl;
		BuildDefaultLauncherInfo();
		fs.clear();
		fs.open(ConfigPath, ios::binary);
	}
	else
	{
		cout << "ϵͳ�����ļ�������" << endl;
	}
	Json::Reader reader;
	Json::Value ValueRoot;
	if (reader.parse(fs, ValueRoot, false))
	{
		_LauncherInfo.SaveDataPath = ValueRoot["DefaultSaveDataPath"].asString();
		for (int i = 0; i < ValueRoot["SongsPath"].size(); i++)
		{
			_LauncherInfo.SongsPath.push_back(ValueRoot["SongsPath"][i].asString());
		}
		_LauncherInfo.ButtonDelay = ValueRoot["ButtonDelay"].asDouble();
		_LauncherInfo.StartResolution.x = ValueRoot["StartResolution"]["x"].asInt();
		_LauncherInfo.StartResolution.y = ValueRoot["StartResolution"]["y"].asInt();
		_LauncherInfo.TagsAppearTimeOffset = ValueRoot["TagsAppearTimeOffset"].asDouble();
	}
	else
	{
		cout << "ϵͳ�����ļ�JSON��ʽ����,���������ֶ��޸������ļ�,����JSON���и�ʽ�Ƿ���ȷ" << endl;
	}
	//Resize window
	COORD size = {_LauncherInfo.StartResolution.x , _LauncherInfo.StartResolution.y };
	SMALL_RECT rc = { 0,0, _LauncherInfo.StartResolution.x - 1, _LauncherInfo.StartResolution.y - 1 };
	SetConsoleWindowInfo(Global_OUT_HANDLE, true, &rc);
	SetConsoleScreenBufferSize(Global_OUT_HANDLE, size);
	CheckJoystick();
	cout << "�ѳɹ���������ϵͳ����" << endl;
	cout << "��ʼɨ������" << endl;
	ScanMusicLibrary();
	cout << "����������,������Ϸ..." << endl;
	GameIsRunning = true;
	system("cls");
}
void BuildDefaultLauncherInfo()
{
	const char * DefLauncherInfo = "{"  \
		"\"SongsPath\":[\"Songs\"],"   \
		"\"DefaultSaveDataPath\" : \"SAVEDATA\"," \
		"\"ButtonDelay\" : 0," \
		"\"StartResolution\" : " \
		" {" \
		"\"x\":120," \
		"\"y\":30" \
		"}" \
		"}";
	ofstream ofs;
	ofs.open("config/GlobalConfig.json");
	ofs << DefLauncherInfo;
	ofs.close();
}

