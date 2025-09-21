/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

#ifndef GAME_H
#define GAME_H
#include <OE/OE.h>

enum GameState {
	TITLE=0, /*Main menu*/
	MENU, /*When you open a menu (settings, chest, inventory, etc)*/
	SURFACE, /*The surface world area*/
	DUNGEON, /*The dungeon area*/
	TESTING /**/
};

typedef struct {
	
} Game;

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


#endif
