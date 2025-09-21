/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */
#ifndef WFC_H
#define WFC_H

#include <OE/OE.h>
#include <OE/bridethread.h>
#include <OE/OELights.h>
#include "wavetiles.h"
#include "chunk.h"

#define MAXTILES 20
#define CHUNKNUM \
	((CHUNKSIZE)*(CHUNKSIZE))

#define WSIZE (MAPSIZE*TILESIZE)*(MAPSIZE*TILESIZE)

#define GENWORLDS 99

enum WFCState {
	NOTSTARTED,
	STARTED,
	FINISHED
};

enum sides {
	WRIGHT,
	WLEFT,
	WUP,
	WDOWN
};

/*
 * MAP INFO
 * In the ptr labeled "map"
 * 0 = Wall
 * 1 = Empty Space
 * 2 = Empty Space + Entity
 * 3 = Empty Space + Treasure
 * */
typedef struct {
	int *map;
	int *lightX, *lightY;
} mapData;

struct connections {
	int tileRight[MAXTILES][MAXTILES];
	int tileLeft[MAXTILES][MAXTILES];
	int tileUp[MAXTILES][MAXTILES];
	int tileDown[MAXTILES][MAXTILES];
};

struct tileData {
	int tileSet[MAXTILES][TILESIZE][TILESIZE];

	int entropyMap[MAPSIZE][MAPSIZE];
	int outputMap[MAPSIZE][MAPSIZE];
	int lightX[MAXLIGHTS];
	int lightY[MAXLIGHTS];
	int mapExpanded[MAPSIZE*TILESIZE][MAPSIZE*TILESIZE];
	
	int tileConnections[MAXTILES][MAXTILES];
	struct connections tileSideCons;

	enum WFCState state;

	int seed;
};

// 9x9 chunks = 81 chunks
struct dungeon {
 	
	char map[(MAPSIZE*TILESIZE)*(MAPSIZE*TILESIZE)];
 	char mapFull[MAPSIZE*TILESIZE][MAPSIZE*TILESIZE];
	Chunk chunks[CHUNKSIZE];

	vec3 playerpos;
 	vec3 playertarg;
	int curlev;
};

mapData *readMap(int level);
void *initWFC();
enum WFCState getWFCState();
void setState(enum WFCState st); 

#endif
