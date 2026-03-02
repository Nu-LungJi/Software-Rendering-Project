#include "../Include/PCH.h"
#include "Artifact.h"

Artifact::Artifact(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Artifact::Artifact(const GameObject& _RHS) : GameObject(_RHS) {}
Artifact::~Artifact() {}

HRESULT Artifact::Ready_GameObject()
{
	
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
		PlayerObject->Set_DefaultSpeed(2.f);
		break;
	}

	return VOID();
}

VOID Artifact::Free()
{
	return VOID();
}
