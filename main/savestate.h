// The game's files in DATA_DIR: records (high.dat), unlocked levels (dynamate.dat), the volume
// (settings.dat) and the user levels (userlevels/*.lev, the GP2X's format)
#ifndef SAVESTATE_H
#define SAVESTATE_H

void LoadHighScores();
void SaveHighScores();
void LoadSettings();
void SaveSettings();
void LoadUnlockData(void);
void SaveUnlockData(void);
void LoadLevel(const char *FileName);
void SaveLevel(const char *FileName);
void SearchForLevels();
void UpdateUserLevelFilenames(const char *filename);

#endif
