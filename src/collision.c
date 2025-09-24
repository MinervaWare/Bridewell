/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include "collision.h"
#include "wfc.h"
#include "chunk.h"
#include "world.h"

#define WALLSIZE 2.0f
#define HALFWSIZE ((WALLSIZE)/2.0f)

#define ESIZE 0.3f
#define HALFESIZE ((ESIZE)/2.0f)

int isEntityColliding(WorldData *world, Entity *e, float x, float y) {
	if(!world||!world->levelData) return 0;
	vec3 pos; vec3_dup(pos, e->renderPos);
	vec3 t; vec3_dup(t, (vec3){x, 0.0f, y});
	vec3 p; vec3_dup(p, PPOSTOSPACE(t));
	float r = 0.1f;
	int i,j;
	int bx = (int)floorf(x);
	int by = (int)floorf(y);
	int worldSize = (MAPSIZE*TILESIZE)-1;
	for(i=bx-1;i<=x+1;i++) {
		for(j=by-1;j<=by+1;j++) {
			int isWall = 0;
			if(i<0||i>worldSize||j<0||j>worldSize) isWall = 1;
			else if(world->levelData[i*(MAPSIZE*TILESIZE)+j]==0) isWall=1;
			if(!isWall) continue;
			vec3 w; vec3_dup(w, (vec3){(float)i,0.0f,(float)j});
			vec3_dup(w, PPOSTOSPACE(w));
			float minX = w[0]-HALFWSIZE, maxX = w[0]+HALFWSIZE;
			float minZ = w[2]-HALFWSIZE, maxZ = w[2]+HALFWSIZE;
			float cx = p[0]<minX ? minX : (p[0]>maxX ? maxX : p[0]);
			float cz = p[2]<minZ ? minZ : (p[2]>maxZ ? maxZ : p[2]);
			float dx = p[0]-cx, dz = p[2]-cz;
			if(dx*dx+dz*dz<r*r) return 1;
		}
	}
	return 0;
}

int isEntityCollidingEntity(WorldData *world, Entity *eptr, float x, float y) {
	EntityList *list = getEntityList();
	vec3 pos; vec3_dup(pos, (vec3){x, 0.0f, y});
	int worldSize = (MAPSIZE*TILESIZE)-1;
	int i,j,k;
	for(i=0;i<list->size;i++) {
		Entity *curEntity = &list->entities[i];
		if(eptr!=NULL) {
			if(curEntity&&curEntity->renderEnabled&&curEntity!=eptr) {
				vec3 ePos; vec3_dup(ePos, curEntity->renderPos);
				float dx = pos[0]-ePos[0];
				float dz = pos[2]-ePos[2];
				float distsq = (dx*dx)+(dz*dz);
				if(distsq<ESIZE) return 1;
			}
		} else { /*if eptr is null we try by testing the positions instead of the entity address*/
			vec3 ePos; vec3_dup(ePos, curEntity->renderPos);
			if(curEntity&&curEntity->renderEnabled&&
					(ePos[0]!=x&&ePos[2]!=y)) {
				float dx = pos[0]-ePos[0];
				float dz = pos[2]-ePos[2];
				float distsq = (dx*dx)+(dz*dz);
				if(distsq<ESIZE) return 1;
			}
		}
	}
	return 0;
}
