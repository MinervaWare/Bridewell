/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef ENTITY_H
#define ENTITY_H

#include <OE/OE.h>
#include "path.h"

#define MAXENTITY 100000
#define ENTITYSTEPSIZE 10

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

	/*Special game-specific things like isInChestRoom*/
	unsigned int targeting :1; /*Is the entity currently targeting another Entity*/	
	Path *currentPath;
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
