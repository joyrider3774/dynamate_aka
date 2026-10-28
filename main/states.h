// One function per game state, each runs while GameState is its own
#ifndef STATES_H
#define STATES_H

void Intro();
void TitleScreenMenu(void);
void LevelTypeMenu(void);
void LevelSelectMenu();
void Game(void);
void IngameMenu();
void WinMenu();
void DiedMenu();
void LevelEditor(void);
void LevelEditorMenu(void);
void LevelEditorLevelSelect(void);

#endif
