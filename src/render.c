/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include "render.h"
#include "ca.glsl.h"
#include "pixel.glsl.h"
#include "world.h"
#include "game.h"
#include "wfc.h"
#include "testing.h"
#include "color.h"
#include "entity.h"

WorldData *wDataPtr;
Renderer *rptr;

void renderObjects() {
	/*OESetObjectPosition("Cube", (vec3){sin(OEGetTick())-4,cos(OEGetTick())+3,0.0f});
	OEDrawObject(OEGetObjectFromName("Cube"));
	OEDrawObject(OEGetObjectFromName("Plane"));
	OESetObjectPosition("Cube", (vec3){2.0f,0.0f,0.0f});
	OEDrawObject(OEGetObjectFromName("Cube"));*/
	//startThread("Entities", (BRIDEFUNC)runEntities);
	runEntities();
	switch(wDataPtr->state) {
		case TITLE: break;
		case MENU: break;
		case SURFACE: renderSurface(wDataPtr);
					  drawEntities();
					  break;
		case DUNGEON: renderCurrentMap(wDataPtr);
					  drawEntities();
					  break;
		case TESTING: renderTest();
	};
	//while(!getEntityThreadState());
}


void *chromaticAberrationUniform() {
	ca_params_t params = {
		.startDist = 0.5f,
		.endDist = 1.0f,
		.strength = 0.02f
	};
	sg_apply_uniforms(UB_ca_params, &SG_RANGE(params));
	return NULL;
}

void enableChromaticAberration() {
	OEAddPostPass("ca", rptr->ppshaders.cap, (UNILOADER)chromaticAberrationUniform);
}

void enablePixelArtShader() {
	OEAddPostPass("pas", rptr->ppshaders.pap, NULL);
}


void renderDebugGui() {
	static bool prevSSGI = true;
	static bool prevFXAA = true;
	static bool prevBLOOM = true;
	static bool prevCA = false;
	static bool prevP = true;
	static int ssgiRays = 32;
	static int ssgiSteps = 8;
	const char *gameStates[] = {"Title","Menu","Surface","Dungeon","Testing"};
	static Entity *player = NULL;
	if(__builtin_expect(player==NULL,0)) player = getEntity("Player");


	/** CIMGUI **/
	igBegin("Cowabunga!",NULL,0);
	igText("Shader Settings");
	igCheckbox("SSGI", &rptr->graphicsSettings.ssgi);
	igSliderInt("SSGI Rays", &ssgiRays, 0, 256, NULL, 0);
	igSliderInt("SSGI Steps", &ssgiSteps, 0, 256, NULL, 0);
	igCheckbox("FXAA", &rptr->graphicsSettings.fxaa);
	igCheckbox("Bloom", &rptr->graphicsSettings.bloom);
	igCheckbox("Chromatic Aberration", &rptr->graphicsSettings.ca);
	igCheckbox("Palettize", &rptr->graphicsSettings.pixelize);
	igText("Game Settings");
	igCombo_Str_arr("Game State", &wDataPtr->state, gameStates, ARRLEN(gameStates), -1);
	if(wDataPtr->state==DUNGEON) {
		igInputInt("Level (0-99)", &wDataPtr->currentLevel, 1,5,0);
		if(wDataPtr->currentLevel>GENWORLDS) wDataPtr->currentLevel = GENWORLDS;
		if(wDataPtr->currentLevel<0) wDataPtr->currentLevel = 0;
	}
	igText("Game Info");
	char pposbuf[512];
	snprintf(pposbuf, sizeof(pposbuf), "Player Pos: %fx, %fy, %fz",
			player->pos[0], player->renderPos[1]/2.0f, player->pos[2]);
	igText(pposbuf);
	igEnd();

	/*Update the shader options*/
	if(rptr->graphicsSettings.fxaa!=prevFXAA) {
		switch(rptr->graphicsSettings.fxaa) {
			case true: OEEnableFXAA();break;
			case false: OEDisableFXAA();break;
		};
	} 
	if(rptr->graphicsSettings.ssgi!=prevSSGI) {
		switch(rptr->graphicsSettings.ssgi) {
			case true: OEEnableSSGI(16, 64);break;
			case false: OEDisableSSGI();break;
		}
	}
	if(rptr->graphicsSettings.bloom!=prevBLOOM) {
		switch(rptr->graphicsSettings.bloom) {
			case true: OEEnableBloom(0.8, 0.5);break;
			case false: OEDisableBloom();break;
		};
	} 
	if(rptr->graphicsSettings.ca!=prevCA) {
		switch(rptr->graphicsSettings.ca) {
			case true: enableChromaticAberration();break;
			case false: OERemovePostPass("ca");break;
		};
	} 
	if(rptr->graphicsSettings.pixelize!=prevP) {
		switch(rptr->graphicsSettings.pixelize) {
			case true: enablePixelArtShader();break;
			case false: OERemovePostPass("pas");break;
		};
	} 

	/*update ssgi params*/
	OEUpdateSSGIParams(ssgiRays, ssgiSteps);

	prevSSGI = rptr->graphicsSettings.ssgi;
	prevFXAA = rptr->graphicsSettings.fxaa;
	prevBLOOM = rptr->graphicsSettings.bloom;
	prevCA = rptr->graphicsSettings.ca;
	prevP = rptr->graphicsSettings.pixelize;
}

void loadObjects() {
	/*Player*/
	OEMesh cube, plane, wall;
	OEParseObj("Cube", "assets/models/cube.obj", &cube);
	OECreateMeshFromAssimp("Player", "assets/models/player.fbx", (vec3){0.0f,0.0f,0.0f});
	OECreateObjectFromMesh(&cube, (vec3){0.0f,0.0f,0.0f});
	/*Dungeon*/
	OECreateMeshFromAssimp("Wall", "assets/models/wall_test.fbx", (vec3){0.0f,0.0f,0.0f});
	OECreateMeshFromAssimp("Wall_3", "assets/models/wall_3_way.fbx", (vec3){0.0f,0.0f,0.0f});
	OECreateMeshFromAssimp("Wall_4", "assets/models/wall_4_way.fbx", (vec3){0.0f,0.0f,0.0f});
	OECreateMeshFromAssimp("Wall_Corner", "assets/models/wall_2_corner.fbx", (vec3){0.0f,0.0f,0.0f});
	OECreateMeshFromAssimp("Plane", "assets/models/floor.fbx", (vec3){0.0f,0.0f,0.0f});
	//OECreateMeshFromAssimp("Wall_Corner", "assets/models/wall_corner.fbx", (vec3){0.0f,0.0f,0.0f});
	/*Surface*/
	OECreateMeshFromAssimp("Surface_Floor", "assets/models/surface_floor.fbx", (vec3){0.0f,0.0f,0.0f});
}

void loadObjectScripts() {
}

void loadTextures() {
	addTexture("playerSkin", "assets/textures/Player_Texture.png");
	addTexture("wallSkin", "assets/textures/wall.png");
	addTexture("floor", "assets/textures/floor.png");
	addTexture("surface_floor", "assets/textures/surface_floor.png");
}

void initPostPassShaders(Renderer *renderer) {
	renderer->ppshaders.ca = sg_make_shader(ca_shader_desc(sg_query_backend()));
	sg_pipeline_desc capd = OEGetQuadPipeline(renderer->ppshaders.ca, "chromatic_aberration");	
	renderer->ppshaders.cap = sg_make_pipeline(&capd);
	renderer->ppshaders.pa = sg_make_shader(pixelArt_shader_desc(sg_query_backend()));
	sg_pipeline_desc ppd = OEGetQuadPipeline(renderer->ppshaders.pa, "pixel_art");	
	renderer->ppshaders.pap = sg_make_pipeline(&ppd);

	// TODO: Edge-Detection, subtle hatching, SSR
	OEEnableFXAA();
	enablePixelArtShader();
	OEEnableSSGI(32, 8);
	OEEnableBloom(0.8, 0.5);
}

WorldData *getWorldData() {
	return wDataPtr;
}

void initRender(Renderer *renderer, WorldData *worldData) {
	wDataPtr = worldData;
	renderer->graphicsSettings.ssgi = true;
	renderer->graphicsSettings.fxaa = true;
	renderer->graphicsSettings.bloom = true;
	renderer->graphicsSettings.ca = false;
	renderer->graphicsSettings.pixelize = true;
	rptr = renderer;


	OEInitRenderer(1280, 720, "yerb", ISOMETRIC);
    //OEInitRenderer(1280, 720, "yerb", PERSPECTIVE);

	/*OEAddLight("Test", (vec3){5.0f, 2.0f, 2.0f}, RGBA255TORGBA1(RED));
	OEAddLight("Test1", (vec3){15.0f, 2.0f, 15.0f}, RGBA255TORGBA1(ORANGE));
	OEAddLight("Test2", (vec3){5.0f, 2.0f, 42.0f}, RGBA255TORGBA1(ORANGE));
	OEAddLight("Test3", (vec3){30.0f, 2.0f, 5.0f}, RGBA255TORGBA1(PURPLE));	
	OEAddLight("Test4", (vec3){0.0f, 2.0f, 0.0f}, RGBA255TORGBA1(GREEN));*/

	loadObjects();
	loadTextures();

	/*TEST SCENE*/
	initWorld(wDataPtr);
	if(wDataPtr->debugLevel>0) {
		OEEnableDebugInfo();
		initTestScene();
	}

	initPostPassShaders(renderer);
}
