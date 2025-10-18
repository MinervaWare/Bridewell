/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

/*All soloud API functions are in src/soloud_c.cpp*/

#include <OE/OE.h>
#include "path.h"
#include "audio.h"
#include "game.h"

void initAudioDeviceList(AudioHandle *handle) {
	if(_EXPZ(!handle)) return;
	handle->deviceList.size = SDL_GetNumAudioDevices(0);
	handle->deviceList.selectedDevice = 0;
	int size = handle->deviceList.size;
	handle->deviceList.devices = calloc(size, sizeof(char *));
	handle->deviceList.deviceIDs = calloc(size, sizeof(int));
	int i;
	for(i=0;i<size;i++) {
		handle->deviceList.deviceIDs[i] = i;
		handle->deviceList.devices[i] = (char *)SDL_GetAudioDeviceName(i, 0);
	}
}

void initAudioBuses(AudioHandle *handle) {
	if(_EXPZ(!handle||!handle->verb)) return;
	handle->mainBus = Bus_create();
	if(!handle->mainBus)
		WLOG(AUDIO_ERROR, "Failed to create main mixing bus!");
	handle->musicBus = Bus_create();
	if(!handle->musicBus)
		WLOG(AUDIO_ERROR, "Failed to create main mixing bus!");
	handle->effectBus = Bus_create();
	if(!handle->effectBus)
		WLOG(AUDIO_ERROR, "Failed to create main mixing bus!");
}

void initAudio(AudioHandle *handle) {
	if(handle==NULL) handle = calloc(1, sizeof(AudioHandle));
	handle->sl = Soloud_create();
	if(!handle->sl)
		WLOG(AUDIO_ERROR, "Failed to create Soloud instance!");

	if(Soloud_init(handle->sl))
		WLOG(AUDIO_ERROR, "Failed to initialize Soloud!");
	
	initAudioDeviceList(handle);

	handle->verb = FreeverbFilter_create();
	if(!handle->verb) 
		WLOG(AUDIO_ERROR, "Failed to create FreeVerb filter!");
	FreeverbFilter_setParams(handle->verb, 0.0f, 
			AVF_TUNE_RSIZE, AVF_TUNE_DAMP, AVF_TUNE_WIDTH);

	initAudioBuses(handle);

	handle->volume = 1.0f;
	Soloud_setGlobalVolume(handle->sl, handle->volume);
	Bus_setVolume(handle->mainBus, handle->volume);
	Bus_setVolume(handle->musicBus, handle->volume);
	Bus_setVolume(handle->effectBus, handle->volume);

	Bus_setFilter(handle->effectBus, 0, handle->verb);

	handle->mainBusID = Soloud_play(handle->sl, handle->mainBus);
	handle->musicBusID = Bus_play(handle->mainBus, handle->musicBus);
	handle->effectBusID = Bus_play(handle->mainBus, handle->effectBus);

	WLOG(AUDIO_INFO, "Successfully initialized Soloud");
}

void destroyAudio(AudioHandle *handle) {
	if(_EXPZ(!handle)) return;
	Soloud_deinit(handle->sl);
	Soloud_destroy(handle->sl);
}

Wav *loadWav(char *filePath) {
	if(!filePath) return NULL;
	Wav *res = Wav_create();
	if(!res) WLOG(AUDIO_ERROR, "Failed to create wav");

	char *infoLog = "Loading wav file: ";
	int size = strlen(filePath)+strlen(infoLog)+16;
	char *buffer; buffer = calloc(size, sizeof(char));
	snprintf(buffer, sizeof(char)*size, "%s %s", infoLog, filePath);
	WLOG(AUDIO_INFO, buffer);
	free(buffer);

	if(Wav_load(res, filePath)) WLOG(AUDIO_ERROR, "Failed to load .wav file!");
	return res;
}

PlayingInfo *playAudioSource(AudioHandle *handle, ASourceType type,
		vec3 pos, char *filePath, int loop) {
	if(_EXPZ(!handle||!filePath)) return NULL;
	Wav *wav = loadWav(filePath);
	if(!wav) return NULL;

	PlayingInfo *res = calloc(1, sizeof(PlayingInfo));
	vec3_dup(res->pos, pos);
	res->volume = 1.0f;
	res->speed = 1.0f;
	res->isLoop = loop;
	res->isStopped = 0;
	res->name = calloc(strlen(filePath)+1, sizeof(char));
	strcpy(res->name, filePath);
	if(type==A_MUSIC) {
		res->type = A_MUSIC;
		res->id = Bus_play(handle->musicBus, wav);
		Soloud_setVolume(handle->sl, res->id, res->volume);
	} else if(type==A_EFFECT) {
		res->type = A_EFFECT;
		res->id = Bus_play3d(handle->effectBus, wav, 
				pos[0], pos[1], pos[2]);
		Soloud_setVolume(handle->sl, res->id, res->volume);
	}

	if(res->isLoop) Soloud_setLooping(handle->sl, res->id, res->isLoop); 

	return res;
}

void updateListenPosition(AudioHandle *handle, vec3 pos) {
	if(_EXPZ(!handle)) return;
	vec3_dup(handle->listenPos, pos);
	Soloud_set3dListenerPosition(handle->sl, pos[0], pos[1], pos[2]);
	Soloud_update3dAudio(handle->sl);
}

void updateAudioPosition(AudioHandle *handle, PlayingInfo *audio, vec3 pos) {
	if(_EXPZ(!handle||!audio)) return;
	vec3_dup(audio->pos, pos);
	Soloud_set3dSourcePosition(handle->sl, audio->id, pos[0], pos[1], pos[2]);
	Soloud_update3dAudio(handle->sl);
}

void pauseAudio(AudioHandle *handle, PlayingInfo *audio) {
	if(_EXPZ(!handle||!audio)) return;
	Soloud_setPause(handle->sl, audio->id, 1);
	audio->isStopped = 1;
}

void playAudio(AudioHandle *handle, PlayingInfo *audio) {
	if(_EXPZ(!handle||!audio)) return;
	Soloud_setPause(handle->sl, audio->id, 0);
	audio->isStopped = 0;
}

void setAudioLoop(AudioHandle *handle, PlayingInfo *audio, int loop) {
	if(_EXPZ(!handle||!audio)) return;
	Soloud_setLooping(handle->sl, audio->id, loop);
	audio->isLoop = loop;
}

void updateAudioVolume(AudioHandle *handle, PlayingInfo *audio, float vol) {
	if(_EXPZ(!handle||!audio)) return;
	Soloud_setVolume(handle->sl, audio->id, vol);	
}

void updateMasterVolume(AudioHandle *handle, float vol) {
	if(_EXPZ(!handle)) return;
	Soloud_setGlobalVolume(handle->sl, vol);
	handle->volume = vol;
}

void updateAudioSpeed(AudioHandle *handle, PlayingInfo *audio, float speed) {
	if(_EXPZ(!handle||!audio)) return;
	Soloud_setRelativePlaySpeed(handle->sl, audio->id, speed);
}

void updateFreeVerbParams(AudioHandle *handle,
		float mode, float roomSize, float damp, float width) {
	if(_EXPZ(!handle)) return;
	Soloud_setFilterParameter(handle->sl, handle->effectBusID, 0, 1, mode);
	Soloud_setFilterParameter(handle->sl, handle->effectBusID, 0, 2, roomSize);
	Soloud_setFilterParameter(handle->sl, handle->effectBusID, 0, 3, damp);
	Soloud_setFilterParameter(handle->sl, handle->effectBusID, 0, 4, width);
}

float getAudioLeakDeduction(int *map, PlayingInfo *audio, vec2 src, vec2 dest) {
	if(_EXPZ(!audio)) return 0.0f;
	float ret = audio->volume/(float)SourceToEntityWallHitCount(map, src, dest);
	if(ret<0.1f) ret = 0.1f;
	else if(ret>1.0f) ret = 1.0f;
	return ret;	
}

void addAudioData(PlayingAudio *arr, PlayingInfo *data) {
	if(!arr) {
		arr = calloc(1, sizeof(PlayingAudio));
		arr->cap = ASIZESTEP;
		arr->size = 0;
		arr->infos = calloc(arr->cap, sizeof(PlayingInfo));
	}
	if(arr->size>=arr->cap) {
		arr->cap += ASIZESTEP;
		arr->infos = (PlayingInfo *)realloc(arr->infos, sizeof(PlayingInfo)*arr->cap);
	}
	if(!data) return;
	if(!data->name) return;
	arr->infos[arr->size].id = data->id;
	arr->infos[arr->size].type = data->type;
	vec3_dup(arr->infos[arr->size].pos, data->pos);
	arr->infos[arr->size].isLoop = data->isLoop;
	arr->infos[arr->size].isStopped = data->isStopped;
	arr->infos[arr->size].volume = data->volume;
	arr->infos[arr->size].speed = data->speed;
	arr->infos[arr->size].length = data->length;
	arr->infos[arr->size].name = calloc(strlen(data->name)+1, sizeof(char));
	strcpy(arr->infos[arr->size].name, data->name);
	arr->size++;
	return;
}

PlayingInfo *getAudioSourceInfo(PlayingAudio *arr, char *name) {
	if(_EXPZ(!arr||!name)) return NULL;
	int i;
	for(i=0;i<arr->size&&arr->infos[i].name!=NULL;i++)
		if(!strcmp(arr->infos[i].name,name)) return &arr->infos[i];
	return NULL;
}
