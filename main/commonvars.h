// The game's globals, the ones every state of main.cpp shared on the GP2X
#ifndef COMMONVARS_H
#define COMMONVARS_H

#include "cursor.h"
#include "defines.h"
#include "gfx.h"
#include "levels.h"

// the longest path of a user level, DATA_DIR/userlevels/<name>.lev
#define LEVELPATHMAX 128

// the menu and the level title are drawn on a copy of their image before it goes on the screen
extern Surface Tmp, TmpTitle;
extern CCursor *Cursor;
extern GameStates GameState;
extern int HighScores[MAXSTATICLEVELS];
extern int lvl, MaxLevels, UserLevelCount, SelectedMusic, Volume;
extern bool StaticLevels, SaveEnabled;
extern const Color Brown, DarkBlue;
extern char UnlockData[30];
extern dm_user_lev UserLevel;
extern char (*UserLevelsFilenames)[LEVELPATHMAX];

#endif
