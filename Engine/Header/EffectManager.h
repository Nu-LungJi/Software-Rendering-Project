#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include "GameObject.h"

class UIEffect;


BEGIN(Engine)
class ENGINE_DLL EffectManager : public Base {
	DECLARE_SINGLETON(EffectManager)

private:
	explicit EffectManager();
	virtual ~EffectManager();
public:
	HRESULT		Ready_EffectManager();
	INT			Update_EffectManager(CONST FLOAT& _DT);
	VOID		LateUpdate_EffectManager(CONST FLOAT& _DT);
	VOID		Render_EffectManager(LPDIRECT3DDEVICE9 _GRPDEV, EFFECT_RENDER _RENDER);

public:
	HRESULT				Append_Effect(EFFECT_OWNER _Owner, GameObject* _Effect);
	GameObject*			Get_Effect(EFFECT_OWNER _Owner, wstring	_TAG);
	list<GameObject*>*	Get_EffectLst(EFFECT_OWNER _Owner);
	GameObject*			Get_Scene() { return Scene_Effect; }
private:
	list<GameObject*>	Container_PlayerEffect;
	list<GameObject*>	Container_MonsterEffect;
	list<GameObject*>	Container_BossEffect;
	list<GameObject*>	Container_UIEffect;
	GameObject*			Scene_Effect;
public:
	virtual			VOID	Free();
};

END