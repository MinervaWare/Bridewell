/* 
 * copyright (c) 2025 minervaware llc
 * licensed under the apache license, version 2.0. see license.
 * */

/*
 * SOLOUD C API - Only the C functions needed for Bridewell.
 * Implemented based on:
 * https://github.com/jarikomppa/soloud/blob/master/src/c_api/soloud_c.cpp
 * */

#include <soloud.h>
#include <soloud_bus.h>
#include <soloud_wav.h>
#include <soloud_filter.h>
#include <soloud_freeverbfilter.h>
using namespace SoLoud;

extern "C" {

/*
 * Generic Soloud API
 * Some of these functions may or may not be used in Bridewell because of Bus usage.
 * */

void Soloud_destroy(void *aClassPtr) {
	delete (Soloud *)aClassPtr;
}

void *Soloud_create() {
	return (void *)new Soloud;
}

int Soloud_init(void *aClassPtr) {
	Soloud *cl = (Soloud *)aClassPtr;
	return cl->init();
}

void Soloud_deinit(void *aClassPtr) {
	Soloud * cl = (Soloud *)aClassPtr;
	cl->deinit();
}

unsigned int Soloud_play(void *aClassPtr, AudioSource *aSound) {
	Soloud *cl = (Soloud *)aClassPtr;
	return cl->play(*aSound);
}

unsigned int Soloud_playEx(void *aClassPtr, AudioSource *aSound,
		float aVolume, float aPan, int aPaused, unsigned int aBus) {
	Soloud *cl = (Soloud *)aClassPtr;
	return cl->play(*aSound, aVolume, aPan, !!aPaused, aBus);
}


unsigned int Soloud_play3d(void *aClassPtr, AudioSource *aSound, 
		float aPosX, float aPosY, float aPosZ) {
	Soloud *cl = (Soloud *)aClassPtr;
	return cl->play3d(*aSound, aPosX, aPosY, aPosZ);
}

unsigned int Soloud_play3dEx(void *aClassPtr, AudioSource *aSound, 
		float aPosX, float aPosY, float aPosZ, 
		float aVelX, float aVelY, float aVelZ, 
		float aVolume, int aPaused, unsigned int aBus) {
	Soloud *cl = (Soloud *)aClassPtr;
	return cl->play3d(*aSound, aPosX, aPosY, aPosZ, 
			aVelX, aVelY, aVelZ, aVolume, !!aPaused, aBus);
}

unsigned int Soloud_play3dClocked(void *aClassPtr, double aSoundTime, AudioSource *aSound, 
		float aPosX, float aPosY, float aPosZ) {
	Soloud *cl = (Soloud *)aClassPtr;
	return cl->play3dClocked(aSoundTime, *aSound, aPosX, aPosY, aPosZ);
}

unsigned int Soloud_play3dClockedEx(void *aClassPtr, double aSoundTime, AudioSource *aSound, 
		float aPosX, float aPosY, float aPosZ,
		float aVelX, float aVelY, float aVelZ,
		float aVolume, unsigned int aBus) {
	Soloud * cl = (Soloud *)aClassPtr;
	return cl->play3dClocked(aSoundTime, *aSound, aPosX, aPosY, aPosZ,
			aVelX, aVelY, aVelZ, aVolume, aBus);
}

unsigned int Soloud_playBackgroundEx(void *aClassPtr, AudioSource *aSound,
		float aVolume, int aPaused, unsigned int aBus) {
	Soloud *cl = (Soloud *)aClassPtr;
	return cl->playBackground(*aSound, aVolume, !!aPaused, aBus);
}

int Soloud_seek(void *aClassPtr, unsigned int aVoiceHandle, double aSeconds) {
	Soloud * cl = (Soloud *)aClassPtr;
	return cl->seek(aVoiceHandle, aSeconds);
}

void Soloud_stop(void *aClassPtr, unsigned int aVoiceHandle) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->stop(aVoiceHandle);
}

void Soloud_stopAll(void * aClassPtr) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->stopAll();
}

void Soloud_setGlobalVolume(void *aClassPtr, float aVolume) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->setGlobalVolume(aVolume);
}

void Soloud_setVolume(void * aClassPtr, unsigned int aVoiceHandle, float aVolume) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->setVolume(aVoiceHandle, aVolume);
}

void Soloud_set3dListenerPosition(void *aClassPtr, 
		float aPosX, float aPosY, float aPosZ) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->set3dListenerPosition(aPosX, aPosY, aPosZ);
}

void Soloud_set3dSourcePosition(void *aClassPtr, unsigned int aVoiceHandle, 
		float aPosX, float aPosY, float aPosZ) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->set3dSourcePosition(aVoiceHandle, aPosX, aPosY, aPosZ);
}

void Soloud_setLooping(void *aClassPtr, unsigned int aVoiceHandle, int aLooping) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->setLooping(aVoiceHandle, !!aLooping);
}

void Soloud_setPause(void *aClassPtr, unsigned int aVoiceHandle, int aPause) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->setPause(aVoiceHandle, !!aPause);
}

void Soloud_update3dAudio(void *aClassPtr) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->update3dAudio();
}

int Soloud_setRelativePlaySpeed(void *aClassPtr, unsigned int aVoiceHandle, float aSpeed) {
	Soloud *cl = (Soloud *)aClassPtr;
	return cl->setRelativePlaySpeed(aVoiceHandle, aSpeed);
}

void Soloud_setFilterParameter(void *aClassPtr, unsigned int aVoiceHandle,
		unsigned int aFilterId, unsigned int aAttributeId, float aValue) {
	Soloud *cl = (Soloud *)aClassPtr;
	cl->setFilterParameter(aVoiceHandle, aFilterId, aAttributeId, aValue);
}

/*
 * Soloud Bus API
 * */

void Bus_destroy(void *aClassPtr) {
	delete (Bus *)aClassPtr;
}

void *Bus_create() {
	return (void *)new Bus;
}

void Bus_setVolume(void * aClassPtr, float aVolume) {
	Bus *cl = (Bus *)aClassPtr;
	cl->setVolume(aVolume);
}

unsigned int Bus_play(void *aClassPtr, AudioSource *aSound) {
	Bus *cl = (Bus *)aClassPtr;
	return cl->play(*aSound);
}

unsigned int Bus_play3d(void *aClassPtr, AudioSource *aSound, 
		float aPosX, float aPosY, float aPosZ) {
	Bus *cl = (Bus *)aClassPtr;
	return cl->play3d(*aSound, aPosX, aPosY, aPosZ);
}

unsigned int Bus_play3dClocked(void *aClassPtr, double aSoundTime, AudioSource *aSound, 
		float aPosX, float aPosY, float aPosZ) {
	Bus *cl = (Bus *)aClassPtr;
	return cl->play3dClocked(aSoundTime, *aSound, aPosX, aPosY, aPosZ);
}

void Bus_stop(void * aClassPtr) {
	Bus *cl = (Bus *)aClassPtr;
	cl->stop();
}

void Bus_setFilter(void *aClassPtr, unsigned int aFilterId, Filter *aFilter) {
	Bus *cl = (Bus *)aClassPtr;
	cl->setFilter(aFilterId, aFilter);
}

/*
 * Soloud Wav API
 * */

void Wav_destroy(void * aClassPtr) {
	delete (Wav *)aClassPtr;
}

void *Wav_create() {
	return (void *)new Wav;
}

int Wav_load(void *aClassPtr, const char *aFilename) {
	Wav *cl = (Wav *)aClassPtr;
	return cl->load(aFilename);
}

/*
 * FreeVerb Filter
 * */

void FreeverbFilter_destroy(void *aClassPtr) {
	delete (FreeverbFilter *)aClassPtr;
}

void *FreeverbFilter_create() {
	return (void *)new FreeverbFilter;
}

int FreeverbFilter_setParams(void * aClassPtr, float aMode, float aRoomSize, 
		float aDamp, float aWidth) {
	FreeverbFilter *cl = (FreeverbFilter *)aClassPtr;
	return cl->setParams(aMode, aRoomSize, aDamp, aWidth);
}

}
