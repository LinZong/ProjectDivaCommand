#pragma once
#include <queue>
#include "SongsScanner.h"
using namespace std;
struct SymbolInfo
{
	int AtSection;
	int AtTag;
	int Symbol;
	int PositionX;
	int PositionY;
	double PositionPixelX;
	double PositionPixelY;
	int FlyIntoDirectionX;
	int FlyIntoDirectionY;
	double FlyIntoPixelX;
	double FlyIntoPixelY;
	double CurrentPixelX;
	double CurrentPixelY;
	double AppearHolderTime;
	double FlyIntoTime;
	double HitCenterTime;
	int FlyDirection[2];
	double Min_tan;
	int FromEdge;
	//�������ռλ��ʱ�� (AtSection-2)*OneSectionTime + AtTag*(OneTagTime)
	//���㿪ʼ����Symbolʱ�� ����ռλ��ʱ��+(0.25)*(OneBeatTime)*(PositionY+1)
	//����ʱ��  (AtSection-1)*OneSectionTime+AtTag*(OneTagTime)
};
extern double Ready_BPM, Ready_OneBeatTime, Ready_OneSectionTime, Ready_OneTagTime, Ready_OneRowTime;
double AheadTimeCalc(int sx, int sy, int ex, int ey,SymbolInfo &si);
void DirectionCalc(SymbolInfo &);
void GetCalcInfo(MusicInfo,int);
queue<SymbolInfo> BuildSymbolQueue();
bool DataValidator(SymbolInfo);