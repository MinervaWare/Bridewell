/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef ENTITY_H
#define ENTITY_H

#include <OE/OE.h>

#define MAXENTITY 100000
#define ENTITYSTEPSIZE 10

#define DEF_HEALTH 100
#define MAX_EX_HEALTH 1000 /*max extended health for the player (gains more over time)*/
#define MAX_HEALTH 100000

#define DEF_IMMUNE 1 

typedef unsigned int EID;
typedef void (*ENTITYFUNC)();

typedef struct {
	EID ID :18; /*cannot go over 100k*/ 
	unsigned int renderEnabled :1;
	char *name; /*to be matched with id*/
	char *modelName;
	char *texName;
	vec3 pos, renderPos;
	float rotation, nextRotation; /*Next is for LERP*/
	ENTITYFUNC render; /*Rendering the entity extras (particles)*/
	ENTITYFUNC generalPerFrame; /*Things like movement, collision, etc.*/

	/*
	 * Special game-specific things like health, path, inChest, etc.
	 * */
	unsigned int targeting :1; /*Is the entity currently targeting another Entity*/	
	void *currentPath, *prevPath;
	unsigned int health :8;
	unsigned int hit :1; /*are they currently taking a hit*/
	unsigned int hitDamage :10; /*should be set when hit is 1*/
	unsigned int immunity :8; /*Immunity time in OETick*/
} Entity;

typedef struct {
	Entity *entities;
	int cap :18;
	int size :18;
	unsigned int threadState :1;
} EntityList; /*Not a linked-list anymore*/

void createEntity(char *name, char *textureName, vec3 pos,
		ENTITYFUNC renderer, ENTITYFUNC general);
void removeEntity(char *name);
void dumpEntityNames();
EntityList *getEntityList();
/*Binary search based on alphebetical position of first char in the name*/
Entity *getEntity(char *name);
void disableEntityRender(char *name);
void enableEntityRender(char *name);
void updateEntityRenderPos(char *name);
void setEntityModel(char *name, char *modelName);
void *runEntities();
void drawEntities();
int getEntityThreadState();

#endif
