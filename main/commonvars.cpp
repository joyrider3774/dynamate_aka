#include "commonvars.h"

Surface Tmp, TmpTitle;
CCursor *Cursor;
GameStates GameState = GSINTRO;
int HighScores[MAXSTATICLEVELS];
int lvl = 0, MaxLevels, UserLevelCount = 0, SelectedMusic = 0, Volume = 128;
bool StaticLevels, SaveEnabled = false;
const Color Brown = {118, 100, 62}, DarkBlue = {0, 99, 180};
char UnlockData[30];
dm_user_lev UserLevel;
char (*UserLevelsFilenames)[LEVELPATHMAX];
