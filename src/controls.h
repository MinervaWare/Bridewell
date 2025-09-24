/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef CONTROLS_H
#define CONTROLS_H
#include <OE/OE.h>
#include "entity.h"

void smoothCamUpdate();
void smoothPlayerRotUpdate();
void smoothEntityRotate(char *ID);
void moveEntityToPos(Entity *e, vec2 dest);
void keyEvent();

#endif
