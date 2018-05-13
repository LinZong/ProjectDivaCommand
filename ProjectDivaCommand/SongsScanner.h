#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;
struct MusicInfo
{
	string Name;
	string Lyric;
	string Map;
	string Pic;
	string MD5;
	string FullPath;
	double BPM;
	int HighestScore;
	string Judge;
	int WithHelp;
	int PassTime;
};
extern bool MusicsLibraryChanged;
extern map<string, MusicInfo> MusicsLibrary;
extern vector<string> MusicLibPath;
void ScanMusicLibrary();
std::string BuildMetadata(char* Name, char* Lyric, char* Map, char* MD5, char* Pic, double BPM);
void WorkDirectory(std::string Path);
//void MatchDatabase(bool force_fresh);
//static int musiclib_callback(void *data, int argc, char **argv, char **azColName);
//static int result_tbl(void *data, int argc, char **argv, char **azColName);
//Access SQLite3 DB