
/*
 * @file	mixmworld_music.c
 * @author  MapleLeaf
 * @date	2016.4.21
 * @brief
 * 
 */

#include "mixmworld_music.h"

Mixm_Music_Data **_mixm_Music_PointerList;
const int MPLen=USHRT_MAX;
int MainVolume=128;
char *ConstString(const char *in){
	char *out=(char *)malloc(strlen(in)+1);
	return strcpy(out,in);
}


int mixm_Music_Init(int chunkSize){
	return mixm_Music_InitEx(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, chunkSize);
}

int mixm_Music_InitEx(int frequency, Uint16 format, int channels, int chunksize){
	if (!_mixm_Music_PointerList &&\
	Mix_Init(MIX_INIT_MP3|MIX_INIT_FLAC|MIX_INIT_OGG|MIX_INIT_MOD) &&\
	!Mix_OpenAudio(frequency, format, channels, chunksize)){
		_mixm_Music_PointerList = (Mixm_Music_Data **)calloc(sizeof (Mixm_Music_Data *), MPLen);
		atexit(mixm_Music_Quit);
		return 0;
	}
	return -1;
}

void mixm_Music_Quit(void){
	for (int i=0;i<MPLen;i++){
		if (!_mixm_Music_PointerList[i]){
			mixm_Music_Del(i);
		}
	}
	free(_mixm_Music_PointerList);
	Mix_Quit();
}

int mixm_Music_Load(ID id,const char *fname){
	if (_mixm_Music_PointerList[id])
		return 1;
		
	_mixm_Music_PointerList[id] = (Mixm_Music_Data *)calloc(sizeof (Mixm_Music_Data),1);
	
	if ((_mixm_Music_PointerList[id]->dataPointer = Mix_LoadWAV(fname))==NULL){
		free(_mixm_Music_PointerList[id]);
		return -1;
	}
	
	_mixm_Music_PointerList[id]->filePath = ConstString(fname);
	return 0;
}

int mixm_Music_Set(ID id,const char *fname){
	if (_mixm_Music_PointerList[id])
		mixm_Music_Del(id);

	return mixm_Music_Load(id,fname);
}

int mixm_Music_Del(ID id){
	if (!_mixm_Music_PointerList[id])
		return 1;
	
	Mix_FreeChunk(_mixm_Music_PointerList[id]->dataPointer);
	
	free(_mixm_Music_PointerList[id]->filePath);
	free(_mixm_Music_PointerList[id]);
	_mixm_Music_PointerList[id]=NULL;
	return 0;
}

const char* mixm_Music_GetPath(ID id){
	return _mixm_Music_PointerList[id]->filePath;
}

int mixm_Music_Play(ID id,int loop,int volume){
	if (!_mixm_Music_PointerList[id])
		return -1;
	if (mixm_Music_Playing(id))
		return -1;
	_mixm_Music_PointerList[id]->dataPointer->volume=volume;
	Mix_PlayChannel(id,_mixm_Music_PointerList[id]->dataPointer,loop);
	Mix_Volume(id,MainVolume);
	return 0;
}

int mixm_Music_Pause(ID id){
	return _mixm_Music_PointerList[id] && (Mix_Pause(id),1);
}

int mixm_Music_Resume(ID id){
	return _mixm_Music_PointerList[id] && (Mix_Resume(id),1);
}

int mixm_Music_Playing(ID id){
	return _mixm_Music_PointerList[id] && Mix_Playing(id);
}

int mixm_Music_Stop(ID id){
	if ((!_mixm_Music_PointerList[id])&&(mixm_Music_Playing(id)))
		return -1;
	Mix_HaltChannel(id);
}

int mixm_Music_Paused(ID id){
	return _mixm_Music_PointerList[id] && Mix_Paused(id);
}

void mixm_Music_SetVolume(int v){
	if (v>128){
		MainVolume = 128;
		return ;
	}
	if (v<0){
		MainVolume = 0;
		return ;
	}
	MainVolume = v;
	Mix_Volume(-1,v);
}
int mixm_Music_ChangeVolume(ID id,int v){
	if (v>128){
		v = 128;
	}
	if (v<0){
		v = 0;
	}
	if (!mixm_Music_Playing(id)){
		return -1;
	}
	_mixm_Music_PointerList[id]->dataPointer->volume=v;
	return 0;
}
