#pragma once

#include "Base.h"
#include "Engine_Define.h"
#include <fmod.hpp>
#include <fmod.h>
#include <fmod_errors.h>

using namespace FMOD;

BEGIN(Engine)
class ENGINE_DLL SoundManager : public Base {
	DECLARE_SINGLETON(SoundManager)
private:
	explicit SoundManager();
	virtual ~SoundManager();

public:
	HRESULT		Ready_SoundManager();
	VOID		Update_SoundManager();

	HRESULT	    Play_Sound_Once(CONST TCHAR* _FilePath, CHANNELID _SoundChannel, _float Volume = 0.5f);
	HRESULT		Play_Sound(CONST TCHAR* _FilePath, CHANNELID _SoundChannel,_float Volume = 0.5f);
	HRESULT		Stop_Sound(CHANNELID _SoundChannel);
	HRESULT		Stop_AllSound();
	HRESULT		IsPlaying(CHANNELID _SoundChannel);
	VOID		Set_ChannelVolume(CHANNELID _CID, FLOAT Volume) { SoundChannel[(LONG)_CID]->setVolume(Volume); }

private:
	System*	SoundSystem; 

	map<CONST TCHAR*, Sound*>	SoundMap;
	
	ChannelGroup* ChannelGroup[(LONG)CHANNELID::SOUND_END];
	Channel* SoundChannel[(LONG)CHANNELID::SOUND_END];

private:
	virtual VOID	Free();
};

END