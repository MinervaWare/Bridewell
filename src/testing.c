/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include "testing.h"
#include "render.h"

void renderTest() {
	OEDrawObject(OEGetObjectFromName("Monkey"));
	OEDrawObjectTex(OEGetObjectFromName("Player"), OE_TEXPOS, getTexture("playerSkin"));
}

void initTestScene() {
	OEMesh monkey;
	OEParseObj("Monkey", "assets/models/monkey.obj", &monkey);
	OECreateObjectFromMesh(&monkey, (vec3){2.0f,0.0f,0.0f});
	//OEAttachScript("Monkey", "scripts/test.lua");
}
