//
//  sound_empty.cpp
//  Sludge Engine
//
//  Created by Matt Tuttle on 10/22/15.
//  Copyright © 2015 SLUDGE. All rights reserved.
//

#include <stdio.h>
#include "sound.h"

soundList *deleteSoundFromList (soundList *s) { return 0; }
void playSoundList(soundList *s) {}

// GENERAL...
bool initSoundStuff (HWND) { return true; }
void killSoundStuff () {}

// MUSIC...
bool playMOD (int, int, int) { return false; }
void stopMOD (int) {}
void setMusicVolume (int a, int v) {}
void setDefaultMusicVolume (int v) {}

// SAMPLES...
int cacheSound (int f) { return 0; }
bool startSound (int, bool) { return false; }
void huntKillSound (int a) {}
void huntKillFreeSound (int filenum) {}
void setSoundVolume (int a, int v) {}
void setDefaultSoundVolume (int v) {}
void setSoundLoop (int a, int s, int e) {}
bool stillPlayingSound (int ch) { return false; }
bool getSoundCacheStack (stackHandler * sH) { return false; }
int findInSoundCache (int a) { return 0; }

void debugSounds () {}
void loadSounds (FILE * fp) {}
void saveSounds (FILE * fp) {}

unsigned int getSoundSource(int index) { return 0; }
