/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef CHUNK_H
#define CHUNK_H

#include <OE/OE.h>

#define TILESIZE 3
#define MAPSIZE 8

//9x9 chunks
#define CHUNKSIZE 9

typedef struct {
	int data[CHUNKSIZE][CHUNKSIZE];
} Chunk;

#endif
