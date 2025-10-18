/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

/*
 * This is for some game definitions and utility macros.
 * */

#ifndef GAME_H
#define GAME_H
#include <OE/OE.h>
#include "world.h"
#include "render.h"

enum GameState {
	TITLE=0, /*Main menu*/
	MENU, /*When you open a menu (settings, chest, inventory, etc)*/
	SURFACE, /*The surface world area*/
	DUNGEON, /*The dungeon area*/
	TESTING /**/
};

#if defined(__GNUC__) || defined(__clang__)
#	define _GAME_PRIVATE __attribute__((unused)) static
#else
#	define _GAME_PRIVATE static
#endif

/*set's the state enum to it's correct string*/
#define STATEFSTR(_str, _enum) \
	if(_str!=NULL) { \
		if(!strcmp(_str,"\"TITLE\""))_enum=TITLE; \
		if(!strcmp(_str,"\"MENU\""))_enum=MENU; \
		if(!strcmp(_str,"\"SURFACE\""))_enum=SURFACE; \
		if(!strcmp(_str,"\"DUNGEON\""))_enum=DUNGEON; \
		if(!strcmp(_str,"\"TESTING\""))_enum=TESTING;}

/*Set the string to it's corresponding enum*/
#define STATEFEN(_str, _enum) \
	if(_enum==TITLE)_str="TITLE"; \
	if(_enum==MENU)_str="MENU"; \
	if(_enum==SURFACE)_str="SURFACE"; \
	if(_enum==DUNGEON)_str="DUNGEON"; \
	if(_enum==TESTING)_str="TESTING";

#define _EXP(_cond, _exp) __builtin_expect((_cond), (_exp))
#define _EXPZ(_cond) __builtin_expect((_cond), 0)

#if OSCLASS==OS_UNIX
#include <unistd.h>
#elif OSCLASS==OS_WINDOWS
#define PATH_MAX 1024
#include <direct.h>
#define getcwd _getcwd
#endif

_GAME_PRIVATE char *getWorkingDir(char *path) {
	char *res = calloc(PATH_MAX, sizeof(char));
	getcwd(res, PATH_MAX*sizeof(char));
	strcat(res, "/");
	strcat(res, path);
	return res;
}

_GAME_PRIVATE char *getExecDir(char *path) {
	WorldData *wDataPtr = getWorldData();
	int len = strlen(path)+strlen(wDataPtr->execDir)+128;
	char *res = calloc(len, sizeof(char));
	snprintf(res, sizeof(char)*len, "%s/%s", wDataPtr->execDir, path);
	return res;
}

_GAME_PRIVATE void initExecDir(char *arg, WorldData *worldData) {
	if(_EXPZ(!arg||!worldData)) return;
	worldData->execDir = calloc(strlen(arg)+1, sizeof(char));
	strcpy(worldData->execDir, arg);
#if OSCLASS==OS_UNIX
	char delim = '/';
#elif OSCLASS==OS_WINDOWS
	char delim = '\\';
#endif
	char *cut = strrchr(worldData->execDir, delim);
	if(cut) *cut = '\0';
}


#endif
