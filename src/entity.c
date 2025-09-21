/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include "entity.h"

EntityList *globalList = NULL;

int EntityCmpName(const void *a, const void *b) {
	const Entity *ea = (const Entity *)a;
	const Entity *eb = (const Entity *)b;
	return strcmp(ea->name, eb->name);
}

int EntityCmpID(const void *a, const void *b) {
	const Entity *ea = (const Entity *)a;
	const Entity *eb = (const Entity *)b;
	return (ea->ID==eb->ID);
}

/*Alphebetically quick sorts*/
void sortEntitiesAlphebet() {
	qsort(&globalList->entities[0], globalList->size, sizeof(Entity), EntityCmpName);
}

/*Quick sorts based on the original EID*/
void sortEntitiesID() {
	qsort(&globalList->entities[0], globalList->size, sizeof(Entity), EntityCmpID);
}

void dumpEntityNames() {
	int i;
	WLOG(INFO_ENTITY, "- - - Entity Dump - - -");
	for(i=0;i<globalList->size;i++) {
		int len = strlen(globalList->entities[i].name)+1;
		char *buf = calloc(len, sizeof(char));
		snprintf(buf, len*sizeof(char), "%s\n", globalList->entities[i].name);
		WLOG(INFO_ENTITY, buf);
	}
	WLOG(INFO_ENTITY, "- - - - - - - - - - - -");
}

Entity *getEntity(char *name) {
	Entity key = {.name = calloc(strlen(name)+1,sizeof(char))};
	strcpy(key.name, name);
	Entity *res = (Entity *)bsearch(&key, globalList->entities,
			globalList->size, sizeof(Entity), EntityCmpName);	
	free(key.name);
	return res;
}

void createEntity(char *name, char *textureName, vec3 pos, 
		ENTITYFUNC renderer, ENTITYFUNC general) {
	if(globalList==NULL) {
		globalList = calloc(1, sizeof(EntityList));
		globalList->size = 0;
		globalList->cap = ENTITYSTEPSIZE;
		globalList->threadState = 0;
		globalList->entities = calloc(globalList->cap, sizeof(Entity));
	}
	if(globalList->size>=globalList->cap) {
		globalList->cap += ENTITYSTEPSIZE;
		globalList->entities = (Entity *)realloc(globalList->entities,
				sizeof(Entity)*globalList->cap);
	}
	Entity *entity = &globalList->entities[globalList->size];
	entity->ID = (EID)globalList->size;
	entity->name = calloc(strlen(name)+1, sizeof(char));
	strcpy(entity->name, name);
	entity->modelName = calloc(strlen(name)+1, sizeof(char));
	strcpy(entity->modelName, name);
	entity->texName = calloc(strlen(textureName)+1, sizeof(char));
	strcpy(entity->texName, textureName);
	entity->render = renderer;
	entity->generalPerFrame = general;
	vec3_dup(entity->pos, pos);
	vec3_dup(entity->renderPos, entity->pos);
	entity->rotation = 0.0f;
	entity->nextRotation = 0.0f;
	entity->renderEnabled = 1;
	entity->targeting = 0;
	entity->currentPath = NULL;
	globalList->size++;
	sortEntitiesAlphebet();
}

void disableEntityRender(char *name) {
	Entity *e = getEntity(name);
	e->renderEnabled = 0;
}

void enableEntityRender(char *name) {
	Entity *e = getEntity(name);
	e->renderEnabled = 1;
}

void updateEntityRenderPos(char *name) {
	Entity *e = getEntity(name);
	vec3_dup(e->renderPos, e->pos);
}

void setEntityModel(char *name, char *modelName) {
	Entity *e = getEntity(name);
	e->modelName = calloc(strlen(modelName)+1, sizeof(char));
	strcpy(e->modelName, modelName);
}

void *runEntities() {
	if(globalList==NULL) return NULL;
	globalList->threadState = 0;
	int i;
	for(i=0;i<globalList->size;i++) {
		Entity *entity = &globalList->entities[i];
		if(entity->generalPerFrame!=NULL) entity->generalPerFrame();
	}
	globalList->threadState = 1;
	return NULL;
}

void drawEntities() {
	if(globalList==NULL) return;
	int i;
	for(i=0;i<globalList->size;i++) {
		Entity *entity = &globalList->entities[i];
		if(entity->renderEnabled) {
			Object *obj = OEGetObjectFromName(entity->modelName);
			OESetObjectPosition(entity->modelName, entity->renderPos);
			OERotateObject(entity->modelName, entity->rotation);	
			OEDrawObjectTex(obj, OE_TEXPOS, getTexture(entity->texName));
		}
		if(entity->render!=NULL) entity->render();
	}
}

int getEntityThreadState() {
	return globalList->threadState;
}

void testEntityTime() {
	/*Initialize 10000 entities and query 10000 entities.*/
	clock_t start, end;
	start = clock();
	int i;
	for(i=0;i<10000;i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "test_%d", i);
		createEntity(buf, buf, (vec3){0.0f,0.0f,0.0f}, NULL, NULL);
	}
	for(i=0;i<10000;i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "test_%d", i);
		Entity *testEntity = getEntity(buf);
	}
	end = clock();
	dumpEntityNames();
	char timeBuf[100];
	snprintf(timeBuf, sizeof(timeBuf), "Entity query time of 10k entity queries: %fs, %fms", 
			((double)(end-start) / CLOCKS_PER_SEC), 
			(((double)(end-start) / CLOCKS_PER_SEC) * 1000));
	WLOG(INFO_ENTITY, timeBuf);
}

