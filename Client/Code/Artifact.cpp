#include "../Include/PCH.h"
#include "Artifact.h"

Artifact::Artifact(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Artifact::Artifact(const GameObject& _RHS) : GameObject(_RHS) {}
Artifact::~Artifact() {}

HRESULT Artifact::Ready_GameObject()
{
	itemIdx = -1;
	return S_OK;
}

INT Artifact::Update_GameObject(const _float& _DT)
{
	if (ObjectDead == true)
		return -1;

	return S_OK;
}

VOID Artifact::LateUpdate_GameObject(const _float& _DT)
{
	return VOID();
}

VOID Artifact::Render_GameObject()
{
	return VOID();
}

Artifact* Artifact::Create(LPDIRECT3DDEVICE9 _GRPDEV)
{
	Artifact* artifact = new Artifact(_GRPDEV);
	if (FAILED(artifact->Ready_GameObject())) {
		MSG_BOX("Cannot Create BOW.");
		Safe_Release(artifact);
		return nullptr;
	}
	return artifact;
}

VOID Artifact::Artifact_Effect()
{
	Player* PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));
	switch (itemIdx) {
	case 0 :
		PlayerObject->Set_DefaultSpeed(1.2f); // 스피드
		break;
	case 1:
		PlayerObject->Set_Atk(11); // 공격력
		break;
	case 2:
		PlayerObject->Set_MaxArrow(2.f); // 활살 최대
		break;
	case 3:
		PlayerObject->Set_AttackSpeed(1.f); // 공격속도
		break;
	}

	return VOID();
}

VOID Artifact::Free()
{
	return VOID();
}
