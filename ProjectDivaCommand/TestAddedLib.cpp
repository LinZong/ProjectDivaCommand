#ifndef TESTADDEDLIB_H_
#define TESTADDEDLIB_H_

#include <iostream>
#include "sqlite3.h"
#include "json\json.h"
#include "TestAddedLib.h"
#include <io.h>
#include <string>
#include "LyricViewer.h"
#include "MD5FILE.h"
using namespace std;
void TestJsonCpp()
{
	const char* str = "{\"uploadid\": \"UP000000\",\"code\": 100,\"msg\": \"这些依赖真是Relax\",\"files\": \"\"}";

	Json::Reader reader;
	Json::Value root;
	if (reader.parse(str, root))
	{
		std::string upload_id = root["uploadid"].asString();
		int code = root["code"].asInt();    // 访问节点，code = 100 
		cout << root["msg"].asCString();
	}
}
void WalkFolders(string path, int layer)
{
	_finddata_t file_info;
	string current_path = path + "/*.*";
	int handle = _findfirst(current_path.c_str(), &file_info);
	if (-1 == handle)
	{
		cout << "Invalid Songs Path" << endl;
		return;
	}
	do
	{
		if (file_info.attrib == _A_SUBDIR)
		{
			/*for (int i = 0; i<layer; i++)
				cout << "--";*/
			cout << file_info.name << endl;
			/*int layer_tmp = layer;
			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)
				WalkFolders(path + '/' + file_info.name, layer_tmp + 1); */
		}
		/*else
		{
			for (int i = 0; i < layer; i++)
			{
				cout << path <<"/"<< file_info.name;
				string str(file_info.name);
				if (str.find(".mp3")!=-1)
				{
					cout << "  " << getFileMD5(path + "/" + file_info.name) << endl;
				}
				else
				{
					cout << endl;
				}
			}
			
			
		}*/
	} while (!_findnext(handle, &file_info)); 						
	_findclose(handle);
}
void TestLrcReader(char * path)
{
	ReadLyricFiles(path);
}

#endif // !TESTADDEDLIB_H_