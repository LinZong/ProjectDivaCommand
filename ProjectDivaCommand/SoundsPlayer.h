#pragma once
#include<string>
#include<Windows.h>
typedef  int(__stdcall* w32mci)(const char*, char*, int, int);
typedef int(__stdcall *  w32mcierror)(int, char*, int);
class Mci
{
private:
	HINSTANCE hins;
	w32mci wmci;
	w32mcierror wmcierror;
public:
	Mci();
	~Mci();
	char buf[256];
	bool send(std::string command);//error  return false   
};
class AudioClip
{
public:
	Mci mci;
	std::string filename;
	std::string alias;
	int length_ms;
	AudioClip();
	~AudioClip();
	bool load(const  std::string& _filename);
	bool play(int start_ms = 0, int end_ms = -1);
	bool stop();
	bool stop(std::string);
	bool pause();
	bool unpause();
	int milliseconds();
};
void AsyncPlayer(const std::string& _fn);