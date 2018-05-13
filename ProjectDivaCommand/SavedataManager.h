#include <string>
#include "Launcher.h"
#include "SongsScanner.h"
#include "sqlite3.h"
using namespace std;
struct SaveDataInfo
{
	string MD5;
	int HighestScore;
	string Judge;
	bool WithHelp;
	int PassTime;
};
extern int SavedataCount;
extern string AllSaveDataName[10];
extern string SelectedSaveDataName;
int QueryEmptySlot();
void InitialDatabase();
void CreateSaveData();
void LoadSaveDataToArray();
void LoadSaveData(int Selected);
bool DeleteSaveData(string);
void UpdateSaveData(MusicInfo _ForUpdateInfo);
void FillMusicInfoMap();
static int listsave_tbl(void *data, int argc, char **argv, char **azColName);
static int load_tbl(void *data, int argc, char **argv, char **azColName);
//static int count_tbl(void *data, int argc, char **argv, char **azColName);
//Load存档之前先检测savedata.db存不存在，如果不存在则新建一个。然后预设好存档表的格式，用户创建存档也就只能输入一个名字而已
//LoadSaveData检测有没有表，没有表就提示要建表，有表就加载
//Delete函数用于Drop 表
//如果玩家打完一首歌，就需要调用Update函数更新歌曲信息。