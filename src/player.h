/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef PLAYER_H
#define PLAYER_H
#include <OE/OE.h>

/*convert player pos to world space*/
#define PPOSTOSPACE(_x) \
	((vec3){(_x)[0]*2.0f,(_x)[1]*2.0f,(_x)[2]*2.0f})

/*convert player pos to world space, and add 2 to y*/
#define PPOSTOSPACEPY(_x) \
	((vec3){((_x)[0]*2.0f)+0.3f,((_x)[1]*2.0f)+2.5f, ((_x)[2]*2.0f)+0.3f})

/*convert player pos to world space, and add _y to y*/
#define PPOSTOSPACEVY(_x, _y) \
	((vec3){(_x)[0]*2.0f,((_x)[1]*2.0f)+(_y), (_x)[2]*2.0f})

void *runPlayerEntity();
void playerUse();
void playerQuickHeal();
void playerBuff();

#endif
