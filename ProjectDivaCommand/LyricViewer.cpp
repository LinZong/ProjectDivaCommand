#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <Windows.h>
#include "LyricViewer.h"
#include "ProjectDivaException.h"
using namespace std;
double TotalTime, PastTime;
const int MAX_LYRIC_REPEAT_NUM = 100;
int TotalLine = 0, HeaderLength;
//可能要自己定义一个LyricTime类方便搞
vector<LrcLine> Lrc;
string HeaderTag[] = { "ti:","al:", "ar:", "mu:", "tot" ,"lr:"};
string ThisLrcHeader[6];
DWORD a;
void ReadLyricFiles(char* FilePath)
{
	TotalLine = 0;
	TotalTime = PastTime = 0;
	HeaderLength = 0;
	ifstream fin;
	fin.open(FilePath);
	if (!fin.is_open())
	{
		throw invalid_argument("Lyric file's path is incorrect or other reason cause fail to read.");
	}
	if (!Lrc.empty()) Lrc.clear();
	string TmpStr;
	getline(fin, TmpStr);
	while (fin)
	{
		if (TmpStr != "")
		{
			LyricAnalyzer(TmpStr);
		}
		getline(fin, TmpStr);
	}
	fin.close();
	StringTimeToMinSec();
	//Print();
}
void LyricHeader(string RawLineLyric)
{
	for (int i = 0; i < sizeof(HeaderTag) / sizeof(HeaderTag[0]); i++)
	{
		if (HeaderTag[i] == RawLineLyric.substr(1, 3))
		{
			HeaderLength++;
			ThisLrcHeader[i] = RawLineLyric;
			if (RawLineLyric.substr(1, 3) == "tot")
			{
				string tstr = RawLineLyric.substr(1, 3);
				stringstream tbuffer;
				tbuffer << tstr;
				tbuffer >> TotalTime;
			}
		}
	}
}
void LyricAnalyzer(string RawLineLrc) throw(InvalidLyricFileException)
{
	if (RawLineLrc[0] != '[')
	{
		throw InvalidLyricFileException();
	}
	//Check if it's head info.
	LyricHeader(RawLineLrc);
	int m, n, p = 0;
	string timeTemp[MAX_LYRIC_REPEAT_NUM];//保存信息  
	m = RawLineLrc.find_first_of('[');
	n = RawLineLrc.find_first_of(']');
	while (m >= 0 && m <= RawLineLrc.length() && n >= 0 && n <= RawLineLrc.length() && RawLineLrc != "")
	{
		timeTemp[p] = RawLineLrc.substr(m + 1, n - 1);
		p++;
		RawLineLrc = RawLineLrc.substr(n + 1, RawLineLrc.length());
		m = RawLineLrc.find_first_of('[');
		n = RawLineLrc.find_first_of(']');
	}

	string contentTemp = RawLineLrc;

	for (int i = 0; i < p; i++)
	{
		if (TotalLine == 1)
		{
			cout << endl;
		}
		LrcLine tline;

		tline.StartTime = timeTemp[i];

		if (timeTemp[i + 1] != "") //连续的  
		{
			tline.EndTime = timeTemp[i + 1];
			cout << timeTemp[i] << endl;
		}

		if (TotalLine - 1 >= 0 && i == 0) //设置上一个的endTime  
		{
			auto LastIter = Lrc.end();
			LastIter--;
			LastIter->EndTime = tline.StartTime;
		}
		tline.LineNum = TotalLine;
		tline.LyricContent = contentTemp;
		Lrc.push_back(tline);
		TotalLine++;
	}
}
void Print(int TL)
{
	//Show Header
	/*for (auto i = Lrc.begin(); i != Lrc.end(); i++)
	{
		if (i == Lrc.end() - 1)
		{
			i->MSec = TL-PastTime;
		}
		FillConsoleOutputCharacter(hd, ' ', 300, { 20 ,24 }, &a);
		int thislen = strlen(i->LyricContent.c_str());
		WriteConsoleOutputCharacter(hd, i->LyricContent.c_str(), thislen, { (SHORT)(60-thislen/2),24 }, &a);
		PastTime += i->MSec;
		Sleep(i->MSec);
	}*/
}
void StringTimeToMinSec()
{

	stringstream buffer;
	for (auto i = Lrc.begin(); i < Lrc.begin() + HeaderLength; i++)
	{
		i->StartTime = "00:00.00";
		i->EndTime = "00:00.00";
	}
	auto i = Lrc.begin();
	advance(i, HeaderLength - 1);
	i->EndTime = (i + 1)->StartTime;
	for (auto i = Lrc.begin(); i != Lrc.end() && i->StartTime != "" && i->EndTime != ""; ++i)
	{
		buffer.clear();
		double Min = 0, Sec = 0, EMin = 0, ESec = 0, Total = 0;
		buffer << i->StartTime.substr(0, 2);
		buffer >> Min;
		buffer.clear();
		buffer << i->EndTime.substr(0, 2);
		buffer >> EMin;
		buffer.clear();
		buffer << i->StartTime.substr(3, i->StartTime.length());
		buffer >> Sec;
		buffer.clear();
		buffer << i->EndTime.substr(3, i->EndTime.length());
		buffer >> ESec;
		Total = (EMin - Min) * 60 * 1000 + (ESec - Sec) * 1000;
		i->MSec = Total;
		buffer.str("");
	}
}