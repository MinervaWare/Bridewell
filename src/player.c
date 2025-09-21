/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include "player.h"
#include "entity.h"
#include "controls.h"

void *runPlayerEntity() {
	static Entity *player = NULL;
	if(__builtin_expect(player==NULL,0)) player = getEntity("Player");
	smoothCamUpdate();
	smoothPlayerRotUpdate();
	vec3 pos;
	vec3_dup(pos, PPOSTOSPACE(player->pos));
	vec3 lightPos;
	vec3_dup(lightPos, PPOSTOSPACEPY(player->pos));
	float bob = pos[1]+(fabs(cos(1.2f*OEGetTick()))/3.0f);
	vec3_dup(player->renderPos, (vec3){pos[0], bob, pos[2]});
	vec3 finalLightPos;
	vec3_dup(finalLightPos, (vec3){lightPos[0], lightPos[1]+bob, lightPos[2]});
	OESetLightPosition("Player", finalLightPos);
}

