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

EnemyController controller = NULL;
WorldData *worldPtr = NULL;

void *testEnemy() {
	if(worldPtr==NULL) return NULL;
	static Entity *player = NULL;
	if(__builtin_expect(player==NULL,0)) player = getEntity("Player");
	static Entity *enemy = NULL;
	if(__builtin_expect(enemy==NULL,0)) enemy = getEntity("Enemy");
	vec2 p; vec2_dup(p, (vec2){player->pos[0], player->pos[2]});
	vec2 e; vec2_dup(e, (vec2){enemy->pos[0], enemy->pos[2]});
	vec3_dup(enemy->renderPos, PPOSTOSPACE(enemy->pos));
	if(worldPtr->currentLevel<=BASIC) {
		if(!enemy->renderEnabled) enableEntityRender("Enemy");
		/*if(canEntitySeeEntity(worldPtr->levelData, e, p)) {
		} else  {}*/
		Path *ePath = findEntityPathTo(worldPtr->levelData, e, p);
		printf("%f,%f\n", enemy->pos[0], enemy->pos[2]);
		if(ePath) {
			moveEntityToPos(enemy, (vec2){ePath->x,ePath->y});
			//vec3_dup(enemy->pos, (vec3){ePath->x, enemy->pos[1], ePath->y});
			freeEntityPath(ePath);
		}
	} else if(enemy->renderEnabled) disableEntityRender("Enemy");
}

void *runCrustBunnies() {
	if(controller->crustBunny>=controller->crustBunnySize) controller->crustBunny=0;
	char buff[1024];
	snprintf(buff, sizeof(buff), "%dEnemy", controller->crustBunny);
	Entity *cb = getEntity(buff);
	
	controller->crustBunny++;
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
				snprintf(ebuff, sizeof(ebuff), "%dEnemy", k);
				printf("%s\n", ebuff);
				createEntity(ebuff, "playerSkin", (vec3){(float)i*2,0.0f,(float)j*2},
						NULL, (ENTITYFUNC)runCrustBunnies);
				setEntityModel(ebuff, "Player");
				disableEntityRender(ebuff);
				controller->crustBunnySize++;
				k++;
			}
		}
	}
}

void cleanEnemies(WorldData *world) {
	
}

