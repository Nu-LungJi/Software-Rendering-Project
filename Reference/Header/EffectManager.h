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

	VOID		Add_GlobalEffect(GameObject* _Effect);
	VOID		Render_GlobalEffect(LPDIRECT3DDEVICE9 _GRPDEV);
	GameObject*	Find_GlobalEffect(wstring _Tag);

public:
	HRESULT				Append_Effect(EFFECT_OWNER _Owner, GameObject* _Effect, INT _FB = (INT)EFFECT_RENDER::BOSS_FRONT_EFFECT);
	GameObject*			Get_Effect(EFFECT_OWNER _Owner, wstring	_TAG);
	GameObject*			Get_Effect(EFFECT_OWNER _Owner, GameObject* _GOBJ);
	GameObject*			Get_Scene() { return Scene_Effect; }
private:
	list<GameObject*>	Container_PlayerEffect;
	list<GameObject*>	Container_MonsterEffect;
	list<GameObject*>	Container_BossFrontEffect;
	list<GameObject*>	Container_BossBackEffect;
	list<GameObject*>	Container_UIEffect;

	GameObject*			Scene_Effect;

	list<GameObject*>	Global_Effect;
public:
	virtual			VOID	Free();
};

END