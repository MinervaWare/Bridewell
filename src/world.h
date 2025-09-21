/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef WORLD_H
#define WORLD_H

#include <OE/OE.h>
#include "game.h"
#include "player.h"
#include "wfc.h"

#define RDISTANCE 6

/*Every 10 levels is a boss, after each boss is a new game section.*/
enum dungeonStage {
	BASIC=9,
	ICE=19,
	AQUATIC=29,
	DESERT=39,
	HELL=49, /*Halfway point*/
	POISON=59,
	FUNGUS=69,
	SPIRIT=79,
	HEAVEN=89,
	_VOID=99
};

typedef struct {
	int state;
	int currentLevel, backup;
	/* DebugLevel meanings:
	 * 0 - no debug info
	 * 1 - cimgui development windows
	 * */
	int debugLevel;
	int *levelData;
	mapData *mapData;
} WorldData;

void loadWorldData(WorldData *world);
void initWorld(WorldData *world);
int getChunkDistanceFromPlayer(int x, int y);
void renderCurrentMap(WorldData *world);
void renderSurface(WorldData *world);

#endif
