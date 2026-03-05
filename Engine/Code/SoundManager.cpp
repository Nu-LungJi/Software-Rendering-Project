#include "SoundManager.h"

IMPLEMENT_SINGLETON(SoundManager)

SoundManager::SoundManager() {}
SoundManager::~SoundManager() { Free(); }

HRESULT SoundManager::Ready_SoundManager() {
	System_Create(&SoundSystem);
	SoundSystem->init(64, FMOD_INIT_NORMAL, NULL);

	SoundSystem->createChannelGroup("BGM",		&ChannelGroup[(int)CHANNELID::SOUND_BGM01]);
	SoundSystem->createChannelGroup("BGM",		&ChannelGroup[(int)CHANNELID::SOUND_BGM02]);
	SoundSystem->createChannelGroup("BGM",		&ChannelGroup[(int)CHANNELID::SOUND_BGM03]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT01]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT02]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT03]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT04]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT05]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT06]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT07]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT08]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT09]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT10]);
	SoundSystem->createChannelGroup("Effect",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT11]);

	return S_OK;
}
VOID SoundManager::Update_SoundManager() {
  SoundSystem->update();

 
}

HRESULT SoundManager::Play_Sound_Once(CONST TCHAR* _FilePath, CHANNELID _SoundChannel, _float Volume)
{
	Sound* SoundObject = nullptr;

	string RootPath = "../../Sound/";
	wstring strUni = _FilePath;

	int len = WideCharToMultiByte(CP_ACP, 0, &strUni[0], -1, NULL, 0, NULL, NULL);
	string strMulti(len, 0);
	WideCharToMultiByte(CP_ACP, 0, &strUni[0], -1, &strMulti[0], len, NULL, NULL);
	RootPath += strMulti;

	auto iter = find_if(SoundMap.begin(), SoundMap.end(), CTag_Finder(_FilePath));

	Channel* pSound = nullptr;

	if (iter == SoundMap.end()) {
		SoundSystem->createSound(RootPath.c_str(), FMOD_LOOP_OFF | FMOD_2D | FMOD_IGNORETAGS, nullptr, &SoundObject);
		SoundMap.insert({ _FilePath, SoundObject });
		SoundSystem->playSound(SoundObject, ChannelGroup[(int)_SoundChannel], FALSE, &SoundChannel[(LONG)_SoundChannel]);

	}
	else {
		SoundSystem->playSound(iter->second, ChannelGroup[(int)_SoundChannel], FALSE, &pSound);
	}
	ChannelGroup[(LONG)_SoundChannel]->setVolume(Volume);
	return S_OK;


}
HRESULT SoundManager::Play_Sound(CONST TCHAR* _FilePath, CHANNELID _SoundChannel, _float Volume) {
  Sound* SoundObject = nullptr;

  string RootPath = "../../Sound/";
  wstring strUni = _FilePath;

  int len = WideCharToMultiByte(CP_ACP, 0, &strUni[0], -1, NULL, 0, NULL, NULL);
  string strMulti(len, 0);
  WideCharToMultiByte(CP_ACP, 0, &strUni[0], -1, &strMulti[0], len, NULL, NULL);
  RootPath += strMulti;

  auto iter = find_if(SoundMap.begin(), SoundMap.end(), CTag_Finder(_FilePath));

  IsPlaying(_SoundChannel);
  Channel* pSound = nullptr;

  if (iter == SoundMap.end()) {
    SoundSystem->createStream(RootPath.c_str(), FMOD_LOOP_NORMAL | FMOD_2D | FMOD_IGNORETAGS, nullptr, &SoundObject);
    SoundMap.insert({ _FilePath, SoundObject });
    SoundSystem->playSound(SoundObject, ChannelGroup[(int)_SoundChannel], FALSE, &SoundChannel[(LONG)_SoundChannel]);
  }
  else {
    SoundSystem->playSound(iter->second, ChannelGroup[(int)_SoundChannel], FALSE, &pSound);
  }

  SoundChannel[(LONG)_SoundChannel]->setVolume(Volume);
  return S_OK;
}
HRESULT SoundManager::Stop_Sound(CHANNELID _SoundChannel) {
  SoundChannel[(LONG)_SoundChannel]->stop();
  return S_OK;
}
HRESULT SoundManager::Stop_AllSound() {
  for (INT ID = 0; ID < (LONG)CHANNELID::SOUND_END; ++ID) {
    SoundChannel[(LONG)ID]->stop();
  }

  for (INT ID = 0; ID < (LONG)CHANNELID::SOUND_END; ++ID)
  {
	  ChannelGroup[(LONG)ID]->stop();
  }
  return S_OK;
}
BOOL SoundManager::IsPlaying(CHANNELID _SoundChannel) {
  bool PlayingCheck = FALSE;

  SoundChannel[(LONG)_SoundChannel]->isPlaying(&PlayingCheck);
  return PlayingCheck;
}

VOID SoundManager::Free() {
  for (auto& Sound : SoundMap) {
    Sound.second->release();
  }
  SoundMap.clear();
  SoundSystem->release();
  SoundSystem->close();
} 