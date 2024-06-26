#include "../../../utility/DxLib_Engine.h"
#include "SoundManager.h"
#include "SoundType_BGM.h"


void SoundManager::LoadBGM(const char* pathName) {

	_BGM = std::make_unique<SoundType_BGM>(pathName);
}


void SoundManager::PlayBGM() {

	if (_BGM) _BGM->Play();
}


void SoundManager::StopBGM() {

	if (_BGM) _BGM->Stop();
}