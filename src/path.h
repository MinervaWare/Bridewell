/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef PATH_H
#define PATH_H
#include <OE/OE.h>
#include "world.h"

#define MAXPATHDIST (MAPSIZE*TILESIZE)
#define PATHSTEP 1

typedef struct {
	vec2 target;
	vec2 source;
	int x, y; /*The next cell*/
} Path;

/*DDA (Digital Differential Analyzer) algorithm*/
int canEntitySeeEntity(int *map, vec2 source, vec2 dest);

/*Returns NULL if source and dest are in the same space*/
Path *findEntityPathTo(int *map, vec2 source, vec2 dest);
void freeEntityPath(Path *path);

#endif
