/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include "enemies.h"
#include "wfc.h"
#include "color.h"
#include "entity.h"
#include "path.h"
#include "controls.h"
#include "world.h"
#include "game.h"

EnemyController *controller = NULL;
WorldData *worldPtr = NULL;

void *runCrustBunnies() {
	if(controller->crustBunny>=controller->crustBunnySize) controller->crustBunny=0;
	char buff[1024];
	snprintf(buff, sizeof(buff), "%dEnemy%d", controller->crustBunny, CRUSTBUNNY);
	Entity *cb = getEntity(buff);
	if(worldPtr->currentLevel<=BASIC) {
		static Entity *player = NULL;
		if(_EXPZ(player==NULL)) player = getEntity("Player");
		vec2 e; vec2_dup(e, (vec2){cb->pos[0], cb->pos[2]});
		vec2 p; vec2_dup(p, (vec2){player->pos[0], player->pos[2]});
		int dist = getChunkDistanceFromPlayer(e[0],e[1]);
		if(!(dist>RDISTANCE||dist<-RDISTANCE)) {
			if(!cb->renderEnabled) cb->renderEnabled=1;
			vec3_dup(cb->renderPos, PPOSTOSPACE(cb->pos));
			Path *path = findEntityPathTo(worldPtr->levelData, cb, e, p);
			if(path) {
				moveEntityToPos(cb, (vec2){path->x, path->y});
				smoothEntityRotate(buff);
				freeEntityPath(path);
			}
		} else cb->renderEnabled=0;
	} else cb->renderEnabled=0;
	controller->crustBunny++;
	return NULL;
}

void initEnemies(WorldData *world) {
	if(worldPtr==NULL) worldPtr = world;
	if(controller==NULL) controller = calloc(1, sizeof(EnemyController));

	int i,j,k=0;
	for(i=0;i<(MAPSIZE*TILESIZE);i++) {
		for(j=0;j<(MAPSIZE*TILESIZE);j++) {
			if(world->levelData[i*(MAPSIZE*TILESIZE)+j]==2) {
				/*TODO: Add rng difficulty scaling for entities based on world level.*/
				char ebuff[1024];
				snprintf(ebuff, sizeof(ebuff), "%dEnemy%d", k, CRUSTBUNNY);
				createEntity(ebuff, "playerSkin", (vec3){(float)i,0.0f,(float)j},
						NULL, (ENTITYFUNC)runCrustBunnies);
				setEntityModel(ebuff, "Player");
				disableEntityRender(ebuff);
				controller->crustBunnySize++;
				k++;
			}
		}
	}
}

void clearEnemies(WorldData *world) {
		
}

