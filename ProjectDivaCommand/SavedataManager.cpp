#include <iostream>
#include <fstream>
#include <io.h>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <direct.h>
#include "Launcher.h"
#include "SongsScanner.h"
#include "MD5FILE.h"
#include "json\json.h"
#include "sqlite3.h"
#include "CharsetConverter.h"
#include "SavedataManager.h"
using namespace std;
sqlite3 *db;
vector<SaveDataInfo> LoadedSaveData;
string SelectedSaveDataName;
string ListAllSaveDataSQL = "select (name) from sqlite_master where type = 'table' and name like 'Save_%' order by name";
string AllSaveDataName[10];
int SavedataCount = 0;
int st = 0;
char *zErrMsg = 0;
int QueryEmptySlot()
{
	int Last = 0;
	LoadSaveDataToArray();
	if (SavedataCount == 0 || ((*(AllSaveDataName[0].end() - 1)) - 48)>1) return 1;
	else
	{
		Last = (*(AllSaveDataName[0].end() - 1)) - 48;
		for (int i = 1; i < SavedataCount; ++i)
		{
			int tmp = (*(AllSaveDataName[i].end() - 1) - 48);
			if (tmp == Last + 1)
			{
				Last = tmp;
				continue;
			}
			else return Last+1;
		}
	}
	return SavedataCount+1;
}
void InitialDatabase()
{
	if (_access(_LauncherInfo.SaveDataPath.c_str(), 6) == -1) _mkdir(_LauncherInfo.SaveDataPath.c_str());
	string DBPath = _LauncherInfo.SaveDataPath + "/savedata.db";
	int rc = sqlite3_open(DBPath.c_str(), &db);
	if (rc)
	{
		//fatal error
	}
	else
	{
		LoadSaveDataToArray();
	}
}
void CreateSaveData()
{

	string savedataname = "Save_" + I2S(QueryEmptySlot());
	string createtbl = "CREATE TABLE " + savedataname + "(" \
		"	MD5          VARCHAR," \
		"	HighestScore INT, " \
		"	Judge        VARCHAR, " \
		"	WithHelp     BOOLEAN, " \
		"	PassTime     INT, " \
		"	PRIMARY KEY( " \
		"		MD5 " \
		"	)," \
		"	UNIQUE(" \
		"		MD5 " \
		"	) " \
		")";

	int rc = sqlite3_exec(db, createtbl.c_str(), 0, 0, &zErrMsg);
	LoadSaveDataToArray();
}
void LoadSaveDataToArray()
{
	if (db == NULL) InitialDatabase();
	if (SavedataCount != 0)
	{
		SavedataCount = 0;
		for (int i = 0; i < 10; ++i) AllSaveDataName[i] = "";
	}
	int rc = sqlite3_exec(db, ListAllSaveDataSQL.c_str(), listsave_tbl, 0, &zErrMsg);
}
void LoadSaveData(int Selected)
{
	SelectedSaveDataName = AllSaveDataName[Selected];
	string LoadSelectedDataSql = "select * from " + AllSaveDataName[Selected];
	LoadedSaveData.clear();
	sqlite3_exec(db, LoadSelectedDataSql.c_str(), load_tbl, 0, &zErrMsg);
	FillMusicInfoMap();
}

bool DeleteSaveData(string FullSaveDataName)
{
	string dropsql = "drop table '" + FullSaveDataName + "'";
	int rc = sqlite3_exec(db, dropsql.c_str(), 0, 0, &zErrMsg);
	if (rc) return false;
	else return true;
}

void UpdateSaveData(MusicInfo _ForUpdateInfo)
{
	MusicsLibraryChanged = true;
	auto iter = MusicsLibrary.find(_ForUpdateInfo.MD5);
	iter->second.HighestScore = _ForUpdateInfo.HighestScore;
	iter->second.Judge = _ForUpdateInfo.Judge;
	iter->second.WithHelp = _ForUpdateInfo.WithHelp;
	iter->second.PassTime = _ForUpdateInfo.PassTime;
	string md5 = _ForUpdateInfo.MD5;
	md5.pop_back();
	string updatesql = "replace into " + SelectedSaveDataName + " (MD5,HighestScore,Judge,WithHelp,PassTime) values ('" + md5 + "'," + I2S(_ForUpdateInfo.HighestScore) + ",'" + _ForUpdateInfo.Judge + "'," + I2S(_ForUpdateInfo.WithHelp) + "," + I2S(_ForUpdateInfo.PassTime) + ")";
	int rc = sqlite3_exec(db, updatesql.c_str(), 0, 0, &zErrMsg);
}

void FillMusicInfoMap()
{
	for (auto iter = MusicsLibrary.begin(); iter != MusicsLibrary.end(); ++iter)
	{
		iter->second.HighestScore = 0;
		iter->second.Judge = "";
		iter->second.WithHelp = 0;
		iter->second.PassTime = 0;
	}
	for (auto sdi : LoadedSaveData)
	{
		sdi.MD5.push_back('\0');
		auto iter = MusicsLibrary.find(sdi.MD5);
		if (!(iter == MusicsLibrary.end()))
		{
			iter->second.HighestScore = sdi.HighestScore;
			iter->second.Judge = sdi.Judge;
			iter->second.WithHelp = sdi.WithHelp;
			iter->second.PassTime = sdi.PassTime;
		}
	}
}

int listsave_tbl(void * data, int argc, char ** argv, char ** azColName)
{
	SavedataCount++;
	//printf("%d  --  %s\n", SavedataCount-1, argv[0]);
	AllSaveDataName[SavedataCount - 1] = string(argv[0]);
	return 0;
}

int load_tbl(void * data, int argc, char ** argv, char ** azColName)
{
	SaveDataInfo currentDataInfo;
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(azColName[i], "MD5") == 0)
		{
			currentDataInfo.MD5 = argv[i];
		}
		if (strcmp(azColName[i], "Judge") == 0)
		{
			currentDataInfo.Judge = argv[i];
		}
		if (strcmp(azColName[i], "HighestScore") == 0)
		{
			currentDataInfo.HighestScore = S2I(argv[i]);
		}
		if (strcmp(azColName[i], "WithHelp") == 0)
		{
			currentDataInfo.WithHelp = S2I(argv[i]);
		}
		if (strcmp(azColName[i], "PassTime") == 0)
		{
			currentDataInfo.PassTime = S2I(argv[i]);
		}
	}
	LoadedSaveData.push_back(currentDataInfo);
	return 0;
}
