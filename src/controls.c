/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include "controls.h"
#include "render.h"
#include "player.h"
#include "entity.h"
#include "collision.h"

/*[0]=RIGHT,[1]=LEFT,[2]=FRONT,[3]=BACK*/
void moveCam(vec4 dist) {
	OEMoveCam(RIGHT, dist[0]);
	OEMoveCam(LEFT, dist[1]);
	OEMoveCam(FRONT, dist[2]);
	OEMoveCam(BACKWARD, dist[3]);
}

void smoothCamUpdate() {
	static Entity *player = NULL;
	if(__builtin_expect(player==NULL,0)) player = getEntity("Player");
	/*This offset is the one set in OE.c*/
	vec3 offset;
	vec3_dup(offset, (vec3){15.0f,15.0f,15.0f});
	vec3 ppos;
	vec3_dup(ppos, PPOSTOSPACE(player->pos));
	vec3 target;
	vec3_dup(target, (vec3){
			ppos[0]+offset[0],
			ppos[1]+offset[1],
			ppos[2]+offset[2]});
	Vec3 Ccur = OEGetCamPos();
	vec3 cur;
	vec3_dup(cur, (vec3){Ccur.x,Ccur.y,Ccur.z});
	float dist = 2.0f*OEGetFrameTime();
	vec3 output;
	WV3LERP(output, cur, target, dist);
	OECamSet(output);
}

void smoothPlayerRotUpdate() {
	static Entity *player = NULL;
	if(__builtin_expect(player==NULL,0)) player = getEntity("Player");
	float speed = 6.0f*OEGetFrameTime();
	if(player->nextRotation-player->rotation>180.0f) {
		float angle = -(360.0f-player->nextRotation);
		player->rotation = WLERP(player->rotation, angle, speed);
	} else player->rotation = WLERP(player->rotation, player->nextRotation, speed);	
}

void smoothEntityRotate(char *ID) {
	Entity *e = getEntity(ID);
	float speed = 6.0f*OEGetFrameTime();
	if(e->nextRotation-e->rotation>180.0f) {
		float angle = -(360.0f-e->nextRotation);
		e->rotation = WLERP(e->rotation, angle, speed);
	} else e->rotation = WLERP(e->rotation, e->nextRotation, speed);	
}


void moveEntityToPos(Entity *e, vec2 dest) {
	int x = dest[0], y = dest[1];
	float speed = 1.8f*OEGetFrameTime();
	float dx = x-e->pos[0];
	float dy = y-e->pos[2];
	float dydx = (dx*dx)+(dy*dy);
	float dist = QSQRT(dydx);
	if(dist<speed) {
		e->pos[0] = x;
		e->pos[2] = y;
	} else {
		e->pos[0] += (dx/dist)*speed;
		e->pos[2] += (dy/dist)*speed;
	}
}

void keyEvent() {
	int i;
	float speed = OEGetFrameTime();
	const Uint8 *kb = SDL_GetKeyboardState(NULL);
	static Entity *player = NULL;
	if(__builtin_expect(player==NULL,0)) player = getEntity("Player");
	Vec3 pos = OEGetCamPos();
	vec2 dist;
	vec2_dup(dist, (vec2){0.0f,0.0f}); 
	
	/*Player Movement WASD*/
	if(kb[SDL_SCANCODE_W]) {dist[0]-=1.0f;dist[1]-=1.0f;}
	if(kb[SDL_SCANCODE_S]) {dist[0]+=1.0f;dist[1]+=1.0f;}
	if(kb[SDL_SCANCODE_A]) {dist[0]-=1.0f;dist[1]+=1.0f;}
	if(kb[SDL_SCANCODE_D]) {dist[0]+=1.0f;dist[1]-=1.0f;}
	
	/*Player Dash*/
	if(kb[SDL_SCANCODE_LSHIFT]) speed*=2.4f;
	else speed*=1.8f;

	/*Apply Player Movements*/
	if(dist[0]!=0.0f||dist[1]!=0.0f) {
		vec2_norm(dist,dist);
		for(i=0;i<2;i++) dist[i]*=speed;

		float x = player->pos[0]+dist[0];
		float y = player->pos[2]+dist[1];
		if(!isEntityColliding(getWorldData(), player, x, y)) {
			player->pos[0] = x;
			player->pos[2] = y;
		} else {
			if(!isEntityColliding(getWorldData(), player, 
						(player->pos[0]+dist[0]), player->pos[2]))
				player->pos[0] += dist[0];
			else if(!isEntityColliding(getWorldData(), player, 
						player->pos[0], (player->pos[2]+dist[1])))
				player->pos[2] += dist[1];
		}
		float rotang = atan2f(dist[0],dist[1]);
		if(rotang<0.0f) rotang+=2*PI;
		player->nextRotation = RAD2DEG(rotang);
	}

	/*Function controls*/
	if(OEIsKeyPressed()) {
		switch(OEGetKeySym()) {
			case SDLK_F1: {
				WorldData *wDataPtr = getWorldData();
				if(wDataPtr->debugLevel==0) {
					OEEnableDebugInfo();
					wDataPtr->debugLevel = 1;
				} else {
					OEDisableDebugInfo();
					wDataPtr->debugLevel = 0;
				}
			}
		};
	}
}

