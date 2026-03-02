#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include "GameObject.h"
#include "GraphicDevice.h"

class  DamageFontManager : public Base
{
	DECLARE_SINGLETON(DamageFontManager)

private:
	explicit DamageFontManager();
	virtual ~DamageFontManager();

private:
	queue<DamageFont>
};