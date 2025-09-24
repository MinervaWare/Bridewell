/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef COLLISION_H
#define COLLISION_H
#include "world.h"
#include "entity.h"

int isEntityColliding(WorldData *world, Entity *e, float x, float y);
int isEntityCollidingEntity(WorldData *world, Entity *eptr, float x, float y);

#endif
