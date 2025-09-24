/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef PHYSICS_H
#define PHYSICS_H

#define GRAV -9.806

/* Get downforce
 * _m=mass _af=applied force (upward)*/
#define GETDFORCE(_m, _af) \
	((GRAV*(_m)+(_af)))

void incEulerHeight(float *y, float *v, float downForce, float mass);

#endif
