/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef SHADERS_H
#define SHADERS_H

#include <OE/OE.h>

#define STARTINGCAP 10

typedef struct {
	char *id;
	sg_pipeline_desc pipe;
	sg_shader_desc *shaderDesc;
	sg_shader shader;
} Shader;

typedef struct {
	int size, cap;
	Shader *shaders;
} Shaders;

void initShaders(Shaders *shaders);
void addShader(Shaders *shaders, char *name, const sg_shader_desc *shd);
sg_shader getShader(Shaders *shaders, char *name);

#endif
