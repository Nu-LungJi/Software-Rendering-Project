#include "EffectManager.h"


IMPLEMENT_SINGLETON(EffectManager)

EffectManager::EffectManager()	{			}
EffectManager::~EffectManager() {	Free();	}

HRESULT EffectManager::Ready_EffectManager() {
	return S_OK;
}
INT EffectManager::Update_EffectManager(CONST FLOAT& _DT) {
	for (auto& PE : Container_PlayerEffect) {
		PE->Update_GameObject(_DT);
	}for (auto& ME : Container_MonsterEffect) {
		ME->Update_GameObject(_DT);
	}for (auto& EE : Container_BossEffect) {
		EE->Update_GameObject(_DT);
	}for (auto& UE : Container_UIEffect) {
		UE->Update_GameObject(_DT);
	}
	if(nullptr != Scene_Effect)	Scene_Effect->Update_GameObject(_DT);
	return 0;
}
VOID EffectManager::LateUpdate_EffectManager(CONST FLOAT& _DT) {

	for (auto iter = Container_PlayerEffect.begin(); iter != Container_PlayerEffect.end();) {
		(*iter)->LateUpdate_GameObject(_DT);
		if ((*iter)->Get_ObjectDead() == TRUE) { 
			Safe_Release((*iter));
			iter = Container_PlayerEffect.erase(iter); 
			continue; 
		}
		else { ++iter; }
	}
	for (auto iter = Container_MonsterEffect.begin(); iter != Container_MonsterEffect.end();) {
		(*iter)->LateUpdate_GameObject(_DT);
		if ((*iter)->Get_ObjectDead() == TRUE) {
			Safe_Release((*iter));
			iter = Container_MonsterEffect.erase(iter);
			continue;
		}
		else { ++iter; }
	}
	for (auto iter = Container_BossEffect.begin(); iter != Container_BossEffect.end();) {
		(*iter)->LateUpdate_GameObject(_DT);
		if ((*iter)->Get_ObjectDead() == TRUE) {
			Safe_Release((*iter));
			iter = Container_BossEffect.erase(iter);
			continue;
		}
		else { ++iter; }
	}
	for (auto iter = Container_UIEffect.begin(); iter != Container_UIEffect.end();) {
		(*iter)->LateUpdate_GameObject(_DT);
		if ((*iter)->Get_ObjectDead() == TRUE) {
			Safe_Release(*iter);
			iter = Container_UIEffect.erase(iter);
			continue;
		}
		else { ++iter; }
	}
	if (nullptr != Scene_Effect)	Scene_Effect->LateUpdate_GameObject(_DT);
}
VOID EffectManager::Render_EffectManager(LPDIRECT3DDEVICE9 _GRPDEV, EFFECT_RENDER _RENDER) {
	_GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	_GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	_GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	_GRPDEV->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_GRPDEV->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	if (_RENDER == EFFECT_RENDER::PLAYER_EFFECT) {
		for (auto& PE : Container_PlayerEffect)
			PE->Render_GameObject();
	}
	if (_RENDER == EFFECT_RENDER::MONSTER_EFFECT) {
		for (auto& ME : Container_MonsterEffect)
			ME->Render_GameObject();
	}
	if (_RENDER == EFFECT_RENDER::BOSS_EFFECT) {
		for (auto& EE : Container_BossEffect)
			EE->Render_GameObject();
	}
	if (_RENDER == EFFECT_RENDER::UI_EFFECT) {
		for (auto& UE : Container_UIEffect)
			UE->Render_GameObject();
	}
	_GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	_GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

HRESULT EffectManager::Append_Effect(EFFECT_OWNER _Owner, GameObject* _Effect) {

	// 실패구문 추가
	if (!_Effect)
		return E_POINTER;

	if (_Owner == EFFECT_OWNER::PLAYER)
		Container_PlayerEffect.push_back(_Effect);
	else if (_Owner == EFFECT_OWNER::MONSTER)
		Container_MonsterEffect.push_back(_Effect);
	else if (_Owner == EFFECT_OWNER::BOSS)
		Container_BossEffect.push_back(_Effect);
	else if (_Owner == EFFECT_OWNER::UI)
		Container_UIEffect.push_back(_Effect);
	else if (_Owner == EFFECT_OWNER::SCENE)
		Scene_Effect = _Effect;
	return S_OK;
}
GameObject* EffectManager::Get_Effect(EFFECT_OWNER _Owner, wstring _TAG) {
	if (_Owner == EFFECT_OWNER::UI) {
		for (auto& UE : Container_UIEffect) {
			if (UE->Get_ObjectTag() == _TAG) {
				return UE;
			}
		}
	}
	else if (_Owner == EFFECT_OWNER::BOSS) {
		for (auto& BE : Container_BossEffect) {
			if (BE->Get_ObjectTag() == _TAG) {
				return BE;
			}
		}
	}
	return nullptr;
}
list<GameObject*>* EffectManager::Get_EffectLst(EFFECT_OWNER _Owner) {
	if (_Owner == EFFECT_OWNER::PLAYER)
		return &Container_PlayerEffect;
	else if (_Owner == EFFECT_OWNER::MONSTER)
		return &Container_MonsterEffect;
	else if (_Owner == EFFECT_OWNER::BOSS)
		return &Container_BossEffect;
	else if (_Owner == EFFECT_OWNER::UI)
		return &Container_UIEffect;

	return nullptr;
}
VOID	EffectManager::Free() {
	for (auto& EFF : Container_PlayerEffect)
		Safe_Release(EFF);
	for (auto& EFF : Container_MonsterEffect)
		Safe_Release(EFF);
	for (auto& EFF : Container_BossEffect)
		Safe_Release(EFF);
	for (auto& EFF : Container_UIEffect)
		Safe_Release(EFF);
	if (nullptr != Scene_Effect)	Safe_Release(Scene_Effect);
}