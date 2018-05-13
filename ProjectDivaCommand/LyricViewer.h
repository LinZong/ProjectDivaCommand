#pragma once
#include <string>
#include <vector>
using namespace std;
struct LrcLine
{
	int LineNum;
	double MSec;
	string StartTime;
	string EndTime;
	string LyricContent;
};
extern vector<LrcLine> Lrc;
void Print(int);
void LyricAnalyzer(string RawLineLrc);
void LyricHeader(string RawLineLyric);
void StringTimeToMinSec();
void ReadLyricFiles(char* FilePath);