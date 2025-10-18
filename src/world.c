/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include <OE/macky.h>
#include <OE/util.h>
#include "world.h"
#include "game.h"
#include "wfc.h"
#include "color.h"
#include "controls.h"
#include "entity.h"
#include "enemies.h"

void loadWorldData(WorldData *world) {
	//if(world->levelData!=NULL) free(world->levelData);
	world->mapData = readMap(world->currentLevel);
	if(world->mapData==NULL) {
		WLOG(ERROR, "Failed to load current level");
		exit(1);
	}
	world->levelData = world->mapData->map;
}


/*This puts in all the data for the different NPCs, enemies, player, etc.*/
void initEntities(WorldData *world) {
	/*Player*/
	vec3 playerPos;
	int i,j;
	for(i=0,j=0;i<MAPSIZE*TILESIZE&&j<MAPSIZE*TILESIZE;i++,j++) {
		int current = world->levelData[i*(MAPSIZE*TILESIZE)+j];
		if(current==1) {
			vec3_dup(playerPos, (vec3){i,0.0f,j});
			break;
		}
	}
	vec3 lightPos;
	vec3_dup(lightPos, PPOSTOSPACEPY(playerPos));
	OEAddLight("Player", 
			lightPos, 
			RGBA255TORGBA1(PLAYER_BLUE));
	OECamSet((vec3){playerPos[0]+15.0f,15.0f,playerPos[2]+15.0f});
	createEntity("Player", "playerSkin", playerPos, NULL, (ENTITYFUNC)runPlayerEntity);
}

void testMusicPlayback(WorldData *world) {
	playAudioSource(&world->audioHandle, A_MUSIC, (vec3){0.0f,0.0f,0.0f}, 
			getExecDir("assets/Music/WIP.wav"), 1);
}

void test3DAudioPlayback(WorldData *world) {
	playAudioSource(&world->audioHandle, A_EFFECT, (vec3){5.0f,0.0f,5.0f},
			getExecDir("assets/Music/WIP.wav"), 1);
}

void initWorldAudio(WorldData *world) {
	initAudio(&world->audioHandle);
	world->audioData = NULL;
	world->audioData = calloc(1, sizeof(PlayingAudio));
	world->audioData->cap = ASIZESTEP;
	world->audioData->size = 0;
	world->audioData->infos = calloc(world->audioData->cap, sizeof(PlayingInfo));
}

/*
* Loads config file, world save data, current level, etc.
 * */
void initWorld(WorldData *world) {
	world->levelData = NULL;
	world->currentLevel = 0;
	world->debugLevel = 0;
	world->state = TITLE;

	mky_data *config = mky_init(getExecDir("config.mky"));
	if(config!=NULL) {
		char buf[256];
		int level = mky_getIntAt("PLAYER", "CurrentLevel");
		world->currentLevel = level;
		snprintf(buf, sizeof(buf), "CurrentLevel = %d", level);
		WLOG(INFO_PLAYER, buf);
		int debug = mky_getIntAt("DEV", "DebugLevel");
		world->debugLevel = debug;
		snprintf(buf, sizeof(buf), "DebugLevel = %d", debug);
		WLOG(INFO_DEV, buf);
		char *state = mky_getStrAt("DEV", "SpecialState");
		STATEFSTR(state, world->state);
		snprintf(buf, sizeof(buf), "SpecialState = %s:%d", state, world->state);
		WLOG(INFO_DEV, buf);
	}

	world->backup = world->currentLevel;
	loadWorldData(world);
	initEntities(world);
	initEnemies(world);
	initWorldAudio(world);
	//testMusicPlayback(world);
	//test3DAudioPlayback(world);

	int i;
	for(i=0;i<MAXLIGHTS-1;i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "wlight_%d", i);
		vec3 lPos; vec3_dup(lPos, (vec3){world->mapData->lightX[i], 0.5f, world->mapData->lightY[i]});
		vec3_dup(lPos, PPOSTOSPACE(lPos));
		Color l = ORANGE; l.a = 255.0f;
		OEAddLight(buf, lPos, RGBA255TORGBA1(l));
	}
}

int isWall(WorldData *world, int x, int y) {
	if(x<0||y<0||x>(MAPSIZE*TILESIZE)||
			y>(MAPSIZE*TILESIZE)) return 0;
	return world->levelData[x*(MAPSIZE*TILESIZE)+y]==0;
}

int getConnections(WorldData *world, int x, int y) {
	if(x>(MAPSIZE*TILESIZE)||y>(MAPSIZE*TILESIZE)||x<0||y<0) return 0;
	int n=0,e=0,w=0,s=0;
	if(y-1>=0) n = isWall(world,x,y-1);
	if(x+1<=(MAPSIZE*TILESIZE)) e = isWall(world,x+1,y);
	if(x-1>=0) w = isWall(world,x-1,y);
	if(y+1<=(MAPSIZE*TILESIZE)) s = isWall(world,x,y+1);
	return n+e+w+s;
}

char *getWallModel(WorldData *world, int x, int y) {
	int con = getConnections(world, x, y);
	if(con<=2) return "Wall";
	if(con==3) return "Wall_3";
	if(con==4) return "Wall_4";
	return "Wall";
}

float getWallRot(WorldData *world, int x, int y) {
	int n = isWall(world,x,y-1);
	int e = isWall(world,x+1,y);
	int w = isWall(world,x-1,y);
	int s = isWall(world,x,y+1);
	int connections = n+e+w+s;
	switch(connections) {
		case 0: return 0.0f; break;
		case 1: if(n) return 0.0f;
				if(s) return 180.0f;
				if(w) return 270.0f;
				if(e) return 90.0f;
				break;
		case 2: if(n&&s||(w&&n)) return 0.0f;
				if((e&&w)||(s&&w)) return 90.0f;
				if(n&&e) return 270.0f;
				if(e&&s) return 180.0f;
				break;
		case 3: if(!n) return 0.0f;
				if(!s) return 180.0f;
				if(!w) return 270.0f;
				if(!e) return 90.0f;
				break;
		case 4: return 0.0f; break;
	};
	return 0;
}

float getBorderWallRot(int x, int y, int maxX, int maxY) {
	if(x==maxX&&y>=0) return 180.0f;
	if(x>=0&&y==maxY) return 270.0f;
	if(x>=0&&y==0) return 90.0f;
	if(x==0&&y>=0) return 0.0f;
	return 0.0f;
}

int getChunkDistanceFromPlayer(int x, int y) {
	static Entity *e = NULL;
	if(_EXPZ(e==NULL)) e = getEntity("Player");
	int px = (int)e->pos[0];
	int py = (int)e->pos[2];
	long dx = (long)x-px;
	long dy = (long)y-py;
	unsigned long res = (unsigned long)((dx*dx)+(dy*dy));
	return QSQRT((float)res);
}

void updateDungeonLights(WorldData *world) {
	int i;
	for(i=0;i<MAXLIGHTS;i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "wlight_%d", i);
		vec3 lPos; vec3_dup(lPos, (vec3){world->mapData->lightX[i], 0.5f, world->mapData->lightY[i]});
		int d = getChunkDistanceFromPlayer(lPos[0], lPos[2]);
		int exists = OEDoesLightExist(buf);
		if((d>RDISTANCE||d<-RDISTANCE)&&exists) {OERemoveLight(buf);continue;}
		if(!exists&&!(d>RDISTANCE||d<-RDISTANCE)) {
			vec3_dup(lPos, PPOSTOSPACE(lPos));
			Color l = ORANGE; l.a = 255.0f;
			OEAddLight(buf, lPos, RGBA255TORGBA1(l));
		}
	}
}

void reinitWorld(WorldData *world) {
	int i,j;
	for(i=0;i<MAXLIGHTS-1;i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "wlight_%d", i);
		OERemoveLight(buf);
	}

	free(world->levelData);
	loadWorldData(world);
	world->backup = world->currentLevel;
	Entity *player = getEntity("Player");
	for(i=0,j=0;i<MAPSIZE*TILESIZE&&j<MAPSIZE*TILESIZE;i++,j++) {
		int current = world->levelData[i*(MAPSIZE*TILESIZE)+j];
		if(current==1) {
			vec3_dup(player->pos, (vec3){i,0.0f,j});
			break;
		}
	}
	vec3 pos;
	vec3_dup(pos, PPOSTOSPACEPY(player->pos));
	OESetLightPosition("Player", pos);
	OECamSet((vec3){pos[0]+15.0f,15.0f,pos[2]+15.0f});
	for(i=0;i<MAXLIGHTS-1;i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "wlight_%d", i);
		vec3 lPos; vec3_dup(lPos, (vec3){world->mapData->lightX[i], 1.25f, world->mapData->lightY[i]});
		vec3_dup(lPos, PPOSTOSPACE(lPos));
		Color l = ORANGE; l.a = 255.0f;
		OEAddLight(buf, lPos, RGBA255TORGBA1(l));
	}
}

void renderCurrentMap(WorldData *world) {
	int i,j;
	int maxcoord = (MAPSIZE*TILESIZE)-1;
	/*Check the state*/
	if(world->currentLevel!=world->backup) reinitWorld(world); 

	/*Render the world*/
	updateDungeonLights(world);
	for(i=0;i<MAPSIZE*TILESIZE;i++) {
		for(j=0;j<MAPSIZE*TILESIZE;j++) {
			Vec3 camPos = OEGetCamPos();
			Vec3 SCamPos = {camPos.x-15.0f,camPos.y-15.0f,camPos.z-15.0f};

			/*Render distance checking*/
			int distance = getChunkDistanceFromPlayer(i,j);
			if(distance>RDISTANCE||distance<-RDISTANCE) continue;

			/*Draw data*/
			OESetObjectPosition("Plane", (vec3){i*2,0.0f,j*2});
			OEDrawObjectTex(OEGetObjectFromName("Plane"), OE_TEXPOS, getTexture("floor"));
			int current = world->levelData[i*(MAPSIZE*TILESIZE)+j];
			if(current==0) {
				char *wall = getWallModel(world, i, j);
				OESetObjectPosition(wall, (vec3){i*2,0.0f,j*2});
				float rot = getWallRot(world,i,j);
				OERotateObject(wall, rot);
				OEDrawObjectTex(OEGetObjectFromName(wall), OE_TEXPOS, getTexture("wallSkin"));
				/*OESetObjectPosition("OECube", (vec3){i*2, 0.0f, j*2});
				OEDrawObject(OEGetObjectFromName("OECube"));*/
			}
			/*Draw outer wall*/
			if(j==maxcoord&&i==0) {
					OESetObjectPosition("Wall_Corner", (vec3){(i*2)-2,0.0f,(j*2)+2});
					OERotateObject("Wall_Corner", 270.0f);
					OEDrawObjectTex(OEGetObjectFromName("Wall_Corner"), OE_TEXPOS, getTexture("wallSkin"));
					OESetObjectPosition("Plane", (vec3){(i*2)-2,0.0f,(j*2)+2});
					OEDrawObjectTex(OEGetObjectFromName("Plane"), OE_TEXPOS, getTexture("floor"));
			} else if(i==maxcoord&&j==0) {
					OESetObjectPosition("Wall_Corner", (vec3){(i*2)+2,0.0f,(j*2)-2});
					OERotateObject("Wall_Corner", 90.0f);
					OEDrawObjectTex(OEGetObjectFromName("Wall_Corner"), OE_TEXPOS, getTexture("wallSkin"));
					OESetObjectPosition("Plane", (vec3){(i*2)+2,0.0f,(j*2)-2});
					OEDrawObjectTex(OEGetObjectFromName("Plane"), OE_TEXPOS, getTexture("floor"));
			}
			if(j==0||i==0) {
				if(j==0&&i==0) {
					OESetObjectPosition("Wall_Corner", (vec3){(i*2)-2,0.0f,(j*2)-2});
					OEDrawObjectTex(OEGetObjectFromName("Wall_Corner"), OE_TEXPOS, getTexture("wallSkin"));
					OESetObjectPosition("Plane", (vec3){(i*2)-2,0.0f,(j*2)-2});
					OEDrawObjectTex(OEGetObjectFromName("Plane"), OE_TEXPOS, getTexture("floor"));
				} else {
					OESetObjectPosition("Wall", (vec3){(i*2)-2,0.0f,(j*2)-2});
					float rot = getBorderWallRot(i,j,maxcoord,maxcoord);
					//float rot = getWallRot(world,i-1,j-1);
					OERotateObject("Wall", rot);
					OEDrawObjectTex(OEGetObjectFromName("Wall"), OE_TEXPOS, getTexture("wallSkin"));
					OESetObjectPosition("Plane", (vec3){(i*2)-2,0.0f,(j*2)-2});
					OEDrawObjectTex(OEGetObjectFromName("Plane"), OE_TEXPOS, getTexture("floor"));
				}
			} 
			if(j==maxcoord||i==maxcoord) {
				if(j==maxcoord&&i==maxcoord) {
					OESetObjectPosition("Wall_Corner", (vec3){(i*2)+2,0.0f,(j*2)+2});
					OERotateObject("Wall_Corner", 180.0f);
					OEDrawObjectTex(OEGetObjectFromName("Wall_Corner"), OE_TEXPOS, getTexture("wallSkin"));
					OESetObjectPosition("Plane", (vec3){(i*2)+2,0.0f,(j*2)+2});
					OEDrawObjectTex(OEGetObjectFromName("Plane"), OE_TEXPOS, getTexture("floor"));
				} else {
					OESetObjectPosition("Wall", (vec3){(i*2)+2,0.0f,(j*2)+2});
					float rot = getBorderWallRot(i,j,maxcoord,maxcoord);
					//float rot = getWallRot(world,i+1,j+1);
					OERotateObject("Wall", rot);
					OEDrawObjectTex(OEGetObjectFromName("Wall"), OE_TEXPOS, getTexture("wallSkin"));
					OESetObjectPosition("Plane", (vec3){(i*2)+2,0.0f,(j*2)+2});
					OEDrawObjectTex(OEGetObjectFromName("Plane"), OE_TEXPOS, getTexture("floor"));
				}
			} 

		}
	}

	OEResetRotation("Wall");
	OEResetRotation("Wall_Corner");
	OEResetRotation("Wall_3");
	OEResetRotation("Wall_4");
}

void renderSurface(WorldData *world) {
	/*Render Surface*/

}
