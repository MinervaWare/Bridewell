/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include <OE/OE.h>
#include "render.h"
#include "shaders.h"
#include "wfc.h"
#include "world.h"
#include "controls.h"
#include "game.h"
//Renderer *renderer;

void draw() {
	renderObjects();	
}

void event() {
	keyEvent();	
}

int main(int argc, char **argv) {
	Renderer renderer;
	WorldData worldData;

	initExecDir(argv[0], &worldData);

	initRender(&renderer, &worldData);
	initWFC();
	initWorld(&worldData);
	WorldData *wDataPtr = getWorldData();
	RENDFUNC cimgui = NULL;

	OESetCursor(getExecDir("assets/textures/cursor.png"));

	while(OERendererIsRunning()) {
		if(wDataPtr->debugLevel>0) cimgui = (RENDFUNC)renderDebugGui;
		else cimgui = NULL;
		OEPollEvents((EVENTFUNC)event);
		OERenderFrame((RENDFUNC)draw, cimgui);
	}

	destroyAudio(&worldData.audioHandle);
	OECleanup();
	
	return 0;
}
