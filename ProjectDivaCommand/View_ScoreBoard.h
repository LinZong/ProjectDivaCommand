#include "InGame.h"
#include "SongsScanner.h"
#include <Windows.h>
#include <string>
void CalcScore(GameScore*, MusicInfo);
void Score_Draw();
void Score_DrawBoard();
void ScoreBoard_KeyCheck(INPUT_RECORD ir, WORD js_word);
