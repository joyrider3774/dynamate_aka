// The level format, and the 30 levels of the game (levels.cpp, the original dynamate_levels.h)
#ifndef LEVELS_H
#define LEVELS_H

#include "dynamate_base.h"

typedef struct{
	const char *name;
	const char *hs;
	dm_u8 field[256];
}dml_lev;

typedef struct{
	char name[256];
	char hs[256];
	dm_u8 field[256];
}dm_user_lev;

extern const dml_lev dml_levels[];

#endif
