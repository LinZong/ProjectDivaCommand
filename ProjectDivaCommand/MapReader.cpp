#include <fstream>
#include <iostream>
#include <math.h>
#include "InGame.h"
#include "MapReader.h"
#include "SongsScanner.h"
#include "Launcher.h"
#include "json\json.h"
#include "CharsetConverter.h"
using namespace std;
double Ready_BPM, Ready_OneBeatTime, Ready_OneSectionTime, Ready_OneTagTime, Ready_OneRowTime;
MusicInfo _ThisMusic;
void GetCalcInfo(MusicInfo OnLoadMusicInfo, int MusicLength)
{
	_ThisMusic = OnLoadMusicInfo;
	Ready_BPM = OnLoadMusicInfo.BPM;
	Ready_OneBeatTime = 60 / Ready_BPM;
	Ready_OneSectionTime = Ready_OneBeatTime * 4;
	Ready_OneTagTime = Ready_OneBeatTime / 2;
	Ready_OneRowTime = Ready_OneBeatTime / 4;
	XPixelSpeed = YPixelSpeed = ((double)440) / (((double)15 / 4)*Ready_OneBeatTime);
}
queue<SymbolInfo> BuildSymbolQueue()
{
	queue<SymbolInfo> SymbolsQueue;
	Json::Reader reader;
	Json::Value value;
	ifstream fs;
	fs.open(_ThisMusic.Map, ios::binary);
	if (!fs.is_open())
	{
		cout << "无法正确加载谱面,谱面可能不正常" << endl;
	}
	bool res = reader.parse(fs, value, false);
	if (res)
	{
		for (auto sections : value["Maps"])
		{
			int _SectionIndex = sections["Index"].asInt();
			int FirstTag = -1;
			for (auto tag : sections["Tags"])
			{
				SymbolInfo si;
				int _TagIndex = tag["Index"].asInt();
				si.AtSection = _SectionIndex;
				si.AtTag = _TagIndex;
				si.Symbol = tag["Symbol"].asInt();
				si.PositionX = tag["Position"][(Json::UInt)0].asInt();
				si.PositionY = tag["Position"][(Json::UInt)1].asInt();
				si.PositionPixelX = (25.55556)*si.PositionX;
				si.PositionPixelY = (31.42857)*si.PositionY;
				si.FlyIntoDirectionX = tag["FlyIntoDirection"][(Json::UInt)0].asInt();
				si.FlyIntoDirectionY = tag["FlyIntoDirection"][(Json::UInt)1].asInt();
				si.FlyIntoPixelX = 20;
				si.FlyIntoPixelY = 20;
				si.PositionPixelX += 20;
				si.PositionPixelY += 20;
				si.AppearHolderTime = (si.AtSection-2)*Ready_OneSectionTime + si.AtTag * (Ready_OneTagTime);
				si.HitCenterTime = (si.AtSection-1)*Ready_OneSectionTime + si.AtTag * (Ready_OneTagTime);
				si.FlyIntoTime = si.HitCenterTime - AheadTimeCalc(si.FlyIntoDirectionX, si.FlyIntoDirectionY, si.PositionX,si.PositionY,si);
				//si.FlyIntoTime = si.AppearHolderTime + (0.25)*(OneBeatTime)*(si.PositionY + 1);
				//DirectionCalc(si);
				if (DataValidator(si)) SymbolsQueue.push(si);
			}
		}
	}
	else
	{
		cout << "无法正确加载谱面,谱面可能不正常" << endl;
	}
	return SymbolsQueue;
}

double AheadTimeCalc(int sx, int sy, int ex, int ey, SymbolInfo &si)
{
	if (sx == 0 && sy == 0)
	{
		si.FromEdge = 0;
		si.FlyIntoDirectionY--;
		si.Min_tan = ((double)((25.5556)*Absolute(ex - sx)) / ((31.4285)* Absolute(ey - sy)));
		si.FlyIntoPixelX += (25.55556)*(si.FlyIntoDirectionX);
		si.FlyIntoPixelY += (31.42857)*(si.FlyIntoDirectionY);
		DirectionCalc(si);
		//return ((double)15 / (14*16))*Ready_OneSectionTime*(DistanceInBlock(sx, sy, ex, ey) + 1);
	}//up
	else if (sx == 36 && sy == 0) 
	{
		si.FromEdge = 1;
		si.FlyIntoDirectionX++;
		si.Min_tan = ((double)((31.4285)*Absolute(ey - sy)) / ((25.5556)*Absolute(ex - sx)));
		si.FlyIntoPixelX += (25.55556)*(si.FlyIntoDirectionX);
		si.FlyIntoPixelY += (31.42857)*(si.FlyIntoDirectionY);
		DirectionCalc(si);
	} //right
	else if (sx == 36 && sy == 14) 
	{
		si.FromEdge = 0;
		si.FlyIntoDirectionY++;
		si.Min_tan = ((double)((25.5556)*Absolute(ex - sx)) / ((31.4285)* Absolute(ey - sy)));
		si.FlyIntoPixelX += (25.55556)*(si.FlyIntoDirectionX);
		si.FlyIntoPixelY += (31.42857)*(si.FlyIntoDirectionY);
		DirectionCalc(si);
	}//down
	else if (sx == 0 && sy == 14) 
	{
		si.FromEdge = 1;
		si.FlyIntoDirectionX--;
		si.Min_tan = ((double)((31.4285)*Absolute(ey - sy))/((25.5556)*Absolute(ex - sx)));
		si.FlyIntoPixelX += (25.55556)*(si.FlyIntoDirectionX);
		si.FlyIntoPixelY += (31.42857)*(si.FlyIntoDirectionY);
		DirectionCalc(si);
	} //left
	else if (sy == 14)
	{
		si.FlyIntoDirectionY++;
		si.FromEdge = 0;
		si.Min_tan = ((double)((25.5556)*Absolute(ex - sx)) / ((31.4285)* Absolute(ey - sy)));
		si.FlyIntoPixelX += (25.55556)*(si.FlyIntoDirectionX);
		si.FlyIntoPixelY += (31.42857)*(si.FlyIntoDirectionY);
		DirectionCalc(si);
	} //down
	else if (sy == 0)
	{
		si.FlyIntoDirectionY--;
		si.FromEdge = 0;
		si.Min_tan = ((double)((25.5556)*Absolute(ex - sx)) / ((31.4285)* Absolute(ey - sy)));
		si.FlyIntoPixelX += (25.55556)*(si.FlyIntoDirectionX);
		si.FlyIntoPixelY += (31.42857)*(si.FlyIntoDirectionY);
		DirectionCalc(si);
	}
	else if (sx == 0)
	{
		si.FromEdge = 1;
		si.FlyIntoDirectionX--;
		si.Min_tan = ((double)((31.4285)*Absolute(ey - sy)) / ((25.5556)*Absolute(ex - sx)));
		si.FlyIntoPixelX += (25.55556)*(si.FlyIntoDirectionX);
		si.FlyIntoPixelY += (31.42857)*(si.FlyIntoDirectionY);
		DirectionCalc(si);
		//return 3*((double)37 / (36 * 38))*Ready_OneSectionTime*(DistanceInBlock(sx, sy, ex, ey));
	} 
	else if (sx == 36)
	{
		si.FromEdge = 1;
		si.FlyIntoDirectionX++;
		si.Min_tan = ((double)((31.4285)*Absolute(ey - sy)) / ((25.5556)*Absolute(ex - sx)));
		si.FlyIntoPixelX += (25.55556)*(si.FlyIntoDirectionX);
		si.FlyIntoPixelY += (31.42857)*(si.FlyIntoDirectionY);
		DirectionCalc(si);
	}
	return (sqrt(pow((si.FlyIntoPixelX - si.PositionPixelX), 2) + pow((si.FlyIntoPixelY - si.PositionPixelY), 2))/YPixelSpeed);
}
void DirectionCalc(SymbolInfo &si)
{
	if (si.FlyIntoDirectionX < si.PositionX && si.FlyIntoDirectionY>si.PositionY)
	{
		if (si.FromEdge == 0)
		{
			si.FlyIntoPixelX -= si.Min_tan*(31.4285);
		}
		else if (si.FromEdge == 1)
		{
			si.FlyIntoPixelY += si.Min_tan*(25.5556);
		}
		
		si.FlyDirection[0] = 1;
		si.FlyDirection[1] = -1;
		return;
	}
	else if (si.FlyIntoDirectionX < si.PositionX && si.FlyIntoDirectionY < si.PositionY)
	{
		if (si.FromEdge == 0)
		{
			si.FlyIntoPixelX -= si.Min_tan*(31.4285);
		}
		else if (si.FromEdge == 1)
		{
			si.FlyIntoPixelY -= si.Min_tan*(25.5556);
		}
		si.FlyDirection[0] = 1;
		si.FlyDirection[1] = 1;
		return;
	}
	else if (si.FlyIntoDirectionX > si.PositionX && si.FlyIntoDirectionY < si.PositionY)
	{
		if (si.FromEdge == 0)
		{
			si.FlyIntoPixelX += si.Min_tan*(31.4285);
		}
		else if (si.FromEdge == 1)
		{
			si.FlyIntoPixelY -= si.Min_tan*(25.5556);
		}
		si.FlyDirection[0] = -1;
		si.FlyDirection[1] = 1;
		return;
	}
	else if (si.FlyIntoDirectionX > si.PositionX && si.FlyIntoDirectionY > si.PositionY)
	{
		if (si.FromEdge == 0)
		{
			si.FlyIntoPixelX += si.Min_tan*(31.4285);
		}
		else if (si.FromEdge == 1)
		{
			si.FlyIntoPixelY += si.Min_tan*(25.5556);
		}
		si.FlyDirection[0] = -1;
		si.FlyDirection[1] = -1;
		return;
	}
	else if (si.FlyIntoDirectionX == si.PositionX && si.FlyIntoDirectionY > si.PositionY)
	{
		si.FlyDirection[0] = 0;
		si.FlyDirection[1] = -1;
		return;
	}
	else if (si.FlyIntoDirectionX == si.PositionX && si.FlyIntoDirectionY < si.PositionY)
	{
		si.FlyDirection[0] = 0;
		si.FlyDirection[1] = 1;
		return;
	}
	else if (si.FlyIntoDirectionX > si.PositionX && si.FlyIntoDirectionY == si.PositionY)
	{
		si.FlyDirection[0] = -1;
		si.FlyDirection[1] = 0;
		return;
	}
	else if (si.FlyIntoDirectionX < si.PositionX && si.FlyIntoDirectionY == si.PositionY)
	{
		si.FlyDirection[0] = 1;
		si.FlyDirection[1] = 0;
		return;
	}
}
bool DataValidator(SymbolInfo _Forcheck)
{
	return true;
}
