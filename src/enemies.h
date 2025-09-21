/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef ENEMIES_H
#define ENEMIES_H
#include "world.h"

/* # Enemies:
 * ## Levels 0-10
 * Crust Bunnies - An accumulation of crystal shards/dust that forms a bunny type enemy,
 * 				   it simply jumps towards the player to attack them.
 *
 * */

#define EMCOUNT (((MAPSIZE*TILESIZE)*(MAPSIZE*TILESIZE))/8)

enum enemyType {
	CRUSTBUNNY=0
};

/*This holds all the ints for the current indexed enemy to be loaded by the entity system*/
typdef struct {
	int crustBunny :8;
	int crustBunnySize :8;
} EnemyController;

void initEnemies(WorldData *world);

#endif
