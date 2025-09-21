/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef RENDER_H
#define RENDER_H

#include "shaders.h"
#include "world.h"

typedef struct {
	bool ssao;
	bool ssgi;
	bool fxaa;
	bool bloom;
	bool ca; /*Chromatic Aberration*/
	bool pixelize;
} GraphicsSettings;

typedef struct {
	sg_shader ca, pa;
	sg_pipeline cap, pap;
} PPShaders;

typedef struct {
	EVENTFUNC keyEvent; /*This is so we can switch controls in testing*/
	Shaders shaders;
	GraphicsSettings graphicsSettings;
	PPShaders ppshaders;
} Renderer;

void renderObjects();
void renderDebugGui();
WorldData *getWorldData();
void initRender(Renderer *renderer, WorldData *worldData);

#endif
