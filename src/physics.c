/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include <OE/OE.h>
#include "physics.h"

/*This is just for calculating things like jumps. You can also use x/z I guess.*/
void incEulerHeight(float *y, float *v, float downForce, float mass) {
	float dt = OEGetFrameTime();
	float a = downForce*(1.0f/mass);
	*(v) += a*dt;
	*(y) += *(v)*dt;
}
