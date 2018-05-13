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
//Load�浵֮ǰ�ȼ��savedata.db�治���ڣ�������������½�һ����Ȼ��Ԥ��ô浵��ĸ�ʽ���û������浵Ҳ��ֻ������һ�����ֶ���
//LoadSaveData�����û�б�û�б����ʾҪ�����б�ͼ���
//Delete��������Drop ��
//�����Ҵ���һ�׸裬����Ҫ����Update�������¸�����Ϣ��