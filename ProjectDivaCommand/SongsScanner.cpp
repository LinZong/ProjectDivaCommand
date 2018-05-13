#include <iostream>
#include <fstream>
#include <io.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "Launcher.h"
#include "SongsScanner.h"
#include "MD5FILE.h"
#include "json\json.h"
#include "sqlite3.h"
#include "CharsetConverter.h"
using namespace std;
bool MusicsLibraryChanged = false;
map<string,MusicInfo> MusicsLibrary;
vector<string> MusicLibPath;
string tPath;
void ScanMusicLibrary()
{
	for (string tp : _LauncherInfo.SongsPath)
	{
		tPath = tp;
		WorkDirectory(tp + "/*.*");
	}
}
string BuildMetadata(char* Name, char* Lyric, char* Map, char* MD5, char* Pic = 0, double BPM = 0)
{
	Json::Value value;
	value["Name"] = Name;
	value["Lyric"] = Lyric;
	value["Map"] = Map;
	value["MD5"] = MD5;
	value["Pic"] = Pic;
	value["BPM"] = BPM;
	string out = value.toStyledString();
	return out;
}
void WorkDirectory(string Path)
{
	_finddata_t file_info;
	int handle = _findfirst(Path.c_str(), &file_info);
	if (-1 == handle)
	{
		cout << "Invalid Songs Path" << endl;
		return;
	}
	do
	{   //兼容Win7的文件夹标记
		if (file_info.attrib == _A_SUBDIR  || file_info.attrib == 8208)
		{
			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)
			{
				//CollectMusicPath
				MusicLibPath.push_back(tPath + '/' + file_info.name);
			}
		}
	} while (!_findnext(handle, &file_info));
	_findclose(handle);

	//Then we can get all subdir in Songs.
	for (string p : MusicLibPath)
	{
		Json::Reader reader;
		Json::Value value;
		_finddata_t mp3, map, lrc, pic;
		int mp3handle, maphandle, lrchandle, pichandle;
		string mp3path = p + '/' + "*.mp3";
		string mappath = p + '/' + "*.map";
		string lrcpath = p + '/' + "*.lrc";
		string picpath = p + '/' + "*.pic";
		string jsonpath = p + '/' + "*.json";
		string TMD5;
		mp3handle = _findfirst(mp3path.c_str(), &mp3);
		maphandle = _findfirst(mappath.c_str(), &map);
		lrchandle = _findfirst(lrcpath.c_str(), &lrc);
		pichandle = _findfirst(picpath.c_str(), &pic);
		if (mp3handle == -1 || maphandle == -1 || lrchandle == -1)
		{
			cout << p << " 处歌曲文件不完整，跳过加载此歌曲." << endl;
			continue;
		}
		else
		{
			//歌曲基本要素完整，检测json是否正常
			string jsonp = p + '/' + "metadata.json";
			int existjson = _access(jsonp.c_str(), 0);
			if (existjson == 0)
			{
				//json存在,尝试解析json
				ifstream fi;
				fi.open(jsonp.c_str(), ios::binary);
				if (!fi.is_open())
				{
					//fatal error json无法打开
				}
				//json成功打开，解析Json
				bool ShouldUpdateJson = false;
				if (reader.parse(fi, value, false))
				{
					string TName = p + '/' + value["Name"].asCString();
					string OName = p + '/' + mp3.name;
					string OMD5 = value["MD5"].asCString();
					OMD5.push_back('\0');
					TMD5 = getFileMD5(OName);
					if (OMD5.compare(TMD5) == 0)
					{
						//do nothing
					}
					else
					{
						ShouldUpdateJson = true;
						value["MD5"] = TMD5;
					}
					if (OName.compare(TName) == 0)
					{
						//Do nothing
					}
					else
					{
						ShouldUpdateJson = true;
						value["Name"] = string(mp3.name);
					}
				}
				if (ShouldUpdateJson)
				{
					string out = value.toStyledString();
					ofstream ofs;
					ofs.open(jsonp.c_str());
					ofs << out;
					ofs.close();
				}
			}
			else
			{
				cout << p << "中json不存在，考虑构建json" << endl;
				//BuildMetadata;
				string OName = p + '/' + mp3.name;
				string TMD5 = getFileMD5(OName);
				string outMetadata = BuildMetadata(mp3.name, lrc.name, map.name, (char*)TMD5.c_str(), (pichandle == -1) ? 0 : pic.name, 0);
				string js = p + '/' + "metadata.json";
				ofstream ofs;
				ofs.open(js.c_str());
				ofs << outMetadata;
				ofs.close();
			}
			//放进结构体
			MusicInfo music;
			string trimName = mp3.name;
			int extensionpos = trimName.find_last_of('.');
			trimName.erase(extensionpos, trimName.length());
			music.Name = trimName;
			music.Lyric = p + '/' + lrc.name;
			music.Map = p + '/' + map.name;
			music.Pic = (pichandle != -1 ? pic.name : "");
			music.MD5 = TMD5;
			music.BPM = value["BPM"].asDouble();
			music.FullPath = p + '/' + mp3.name;
			MusicsLibrary.insert(make_pair(music.MD5, music));
		}
	}
}
