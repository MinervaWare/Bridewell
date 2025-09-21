/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#include "path.h"
#include "chunk.h"

int canEntitySeeEntity(int *map, vec2 source, vec2 dest) {
	if(!map) return 0;
	int mapX = (int)source[0];
	int mapY = (int)source[1];
	if(mapX==(int)dest[0]&&mapY==(int)dest[1]) return 1;
	float dx = dest[0]-source[0];
	float dy = dest[1]-source[1];
	float p = (dx*dx)+(dy*dy);
	float len = QSQRT(p);
	if(len==0.0f) return 1;
	float rayDirX = dx/len;
	float rayDirY = dy/len;

	float sideDistX, sideDistY;
	float deltaDX = (rayDirX==0) ? 1e30 : fabsf(1.0f/rayDirX);
	float deltaDY = (rayDirY==0) ? 1e30 : fabsf(1.0f/rayDirY);

	int wallHit = 1, side; /*size is just if it hit on N/S or E/W*/
	int stepX, stepY;
	if(rayDirX<0) {
		stepX = -1;
		sideDistX = (source[0]-mapX)*deltaDX;
	} else {
		stepX = 1;
		sideDistX = (mapX+1.0f-source[0])*deltaDX;
	}
	if(rayDirY<0) {
		stepY = -1;
		sideDistY = (source[1]-mapY)*deltaDY;
	} else {
		stepY = 1;
		sideDistY = (mapY+1.0f-source[1])*deltaDY;
	}
	while(wallHit==1) {
		if(sideDistX<sideDistY) {
			sideDistX += deltaDX;
			mapX += stepX;
			side = 0;
		} else {
			sideDistY += deltaDY;
			mapY += stepY;
			side = 1;
		}
		if(mapX<0||mapX>=(MAPSIZE*TILESIZE)||mapY<0||mapY>=(MAPSIZE*TILESIZE)) return 0;
		if(mapX==(int)dest[0]&&mapY==(int)dest[1]) return 1;
		if(map[mapX*(MAPSIZE*TILESIZE)+mapY]==0) {wallHit=0;break;}
	}

	return wallHit;
}

Path *findEntityPathTo(int *map, vec2 source, vec2 dest) {
	Path *res = calloc(1, sizeof(Path));
	vec2_dup(res->target, dest);
	vec2_dup(res->source, source);
	res->x = source[0];
	res->y = source[1];


	if(!map) {freeEntityPath(res);return NULL;}
	int mapX = (int)source[0];
	int mapY = (int)source[1];
	if(mapX==(int)dest[0]&&mapY==(int)dest[1]) {freeEntityPath(res);return NULL;}
	float dx = dest[0]-source[0];
	float dy = dest[1]-source[1];
	float p = (dx*dx)+(dy*dy);
	float len = QSQRT(p);
	if(len==0.0f) {freeEntityPath(res);return NULL;}
	float rayDirX = dx/len;
	float rayDirY = dy/len;

	float sideDistX, sideDistY;
	float deltaDX = (rayDirX==0) ? 1e30 : fabsf(1.0f/rayDirX);
	float deltaDY = (rayDirY==0) ? 1e30 : fabsf(1.0f/rayDirY);

	int wallHit = 1, side; /*size is just if it hit on N/S or E/W*/
	int stepX, stepY;
	if(rayDirX<0) {
		stepX = -1;
		sideDistX = (source[0]-mapX)*deltaDX;
	} else {
		stepX = 1;
		sideDistX = (mapX+1.0f-source[0])*deltaDX;
	}
	if(rayDirY<0) {
		stepY = -1;
		sideDistY = (source[1]-mapY)*deltaDY;
	} else {
		stepY = 1;
		sideDistY = (mapY+1.0f-source[1])*deltaDY;
	}

	if(sideDistX<sideDistY) {
		sideDistX += deltaDX;
		if(mapX+stepX<0||mapX+stepX>=(MAPSIZE*TILESIZE)||mapY<0||mapY>=(MAPSIZE*TILESIZE)) return res;
		if(map[(mapX+stepX)*(MAPSIZE*TILESIZE)+mapY]==0) return res;
		if(mapX+stepX==(int)dest[0]&&mapY==(int)dest[1]) {
			mapX += stepX; 
			res->x = mapX; 
			res->y = mapY; 
			return res;
		}
		mapX += stepX;
		side = 0;
	} else {
		sideDistY += deltaDY;
		if(mapX<0||mapX>=(MAPSIZE*TILESIZE)||mapY+stepY<0||mapY+stepY>=(MAPSIZE*TILESIZE)) return res;
		if(map[mapX*(MAPSIZE*TILESIZE)+mapY+stepY]==0) return res;
		if(mapX==(int)dest[0]&&mapY+stepY==(int)dest[1]) {
			mapY += stepY; 
			res->x = mapX; 
			res->y = mapY; 
			return res;
		}
		mapY += stepY;
		side = 1;
	}

	res->x = mapX;
	res->y = mapY;

	return res;
}

void freeEntityPath(Path *path) {
	if(!path) return;
	free(path);
}
