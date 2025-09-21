/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include "shaders.h"

void initShaders(Shaders *shaders) {
	shaders = calloc(1, sizeof(Shaders));
	shaders->cap = STARTINGCAP;
	shaders->size = 0;
	shaders->shaders = calloc(shaders->cap, sizeof(Shader));
}

void addShader(Shaders *shaders, char *name, const sg_shader_desc *shd) {
	int i;
	if(name==NULL) return;
	if(shaders==NULL) initShaders(shaders);
	if(shaders->size >= shaders->cap) {
		shaders->cap += STARTINGCAP;
		shaders->shaders = (Shader *)realloc(shaders->shaders,
				sizeof(Shader)*shaders->cap);
	}
	for(i=0;i<shaders->cap;i++) {

		if(shaders->shaders[i].id==NULL) {
			shaders->shaders[i].shader = sg_make_shader(shd);
			shaders->shaders[i].pipe = OEGetDefaultPipe(shaders->shaders[i].shader, name);
			shaders->shaders[i].id = calloc(strlen(name)+1, sizeof(char)); 
			strcpy(shaders->shaders[i].id, name);
		}
	}
}

sg_shader getShader(Shaders *shaders, char *name) {
	if(name!=NULL) {
		int i;
		for(i=0;i<shaders->cap;i++) {
			if(shaders->shaders[i].id!=NULL&&
					!strcmp(shaders->shaders[i].id, name))
				return shaders->shaders[i].shader;
		}
	}
	return (sg_shader){0};
}

