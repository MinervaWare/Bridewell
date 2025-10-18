/* 
 * copyright (c) 2025 minervaware llc
 * licensed under the apache license, version 2.0. see license.
 * */

/*
 * All audio processing is done with the Soloud library
 * so I can do more complex live mixing and surround sound.
 * */


#ifndef AUDIO_H
#define AUDIO_H
#include <soloud_c.h>

#define ASIZESTEP 10

/*Freeverb tuning*/
#ifndef AFV_DEFS
#define AVF_DEFS
#define AVF_TUNE_RSIZE 0.3f
#define AVF_TUNE_DAMP 1.0f
#define AVF_TUNE_WIDTH 0.5f
#endif

typedef unsigned int soloud_id;

typedef enum {
	A_MUSIC,
	A_EFFECT
} ASourceType;

#define A_TYPESTR(_type, _str) ({ \
		if(_type==A_MUSIC) _str="A_MUSIC"; \
		if(_type==A_EFFECT) _str="A_EFFECT";})

typedef struct {
	soloud_id id;
	ASourceType type;
	vec3 pos;
	int isLoop, isStopped; 
	float volume, speed;
	float length; /*Length in secconds*/
	char *name;
} PlayingInfo;

typedef struct {
	PlayingInfo *infos;
	int size, cap;
} PlayingAudio;

typedef struct {
	char **devices;
	int *deviceIDs;
	int size, selectedDevice;
} AudioDeviceList;

typedef struct {
	/*All these soloud defs are just 'typedef void *' */
	Soloud *sl;
	soloud_id mainBusID, musicBusID, effectBusID;
	Bus *mainBus, *musicBus, *effectBus;
	FreeverbFilter *verb;

	float volume; /*This is the standard speaker volume, seperate from PlayingInfo volume.*/
	vec3 listenPos;
	AudioDeviceList deviceList;
} AudioHandle;

void initAudio(AudioHandle *handle);
void destroyAudio(AudioHandle *handle);
/*If type is A_MUSIC pos is discarded*/
PlayingInfo *playAudioSource(AudioHandle *handle, ASourceType type,
		vec3 pos, char *filePath, int loop);

void updateListenPosition(AudioHandle *handle, vec3 pos);
void updateAudioPosition(AudioHandle *handle, PlayingInfo *audio, vec3 pos);
void pauseAudio(AudioHandle *handle, PlayingInfo *audio);
void playAudio(AudioHandle *handle, PlayingInfo *audio);
void setAudioLoop(AudioHandle *handle, PlayingInfo *audio, int loop);
void updateAudioVolume(AudioHandle *handle, PlayingInfo *audio, float vol);
void updateAudioSpeed(AudioHandle *handle, PlayingInfo *audio, float speed);
void updateFreeVerbParams(AudioHandle *handle,
		float mode, float roomSize, float damp, float width);

float getAudioLeakDeduction(int *map, PlayingInfo *audio, vec2 src, vec2 dest);

void addAudioData(PlayingAudio *arr, PlayingInfo *data);
PlayingInfo *getAudioSourceInfo(PlayingAudio *arr, char *name);

#endif
