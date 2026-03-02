#include "../Include/PCH.h"

Supply::Supply(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Supply::Supply(const GameObject& _RHS) : GameObject(_RHS) {}
Supply::~Supply() {}

HRESULT Supply::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;


	return S_OK;
}
INT	Supply::Update_GameObject(const _float& _DT)
{
	if (ObjectDead)
		return -1;

	GameObject::Update_GameObject(_DT);

	for (int i = 0; i < (long)SUPPLY_OBJECT::End; ++i)
		m_pObj[i]->Update_GameObject(_DT);

	return 0;
}
VOID Supply::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
}

HRESULT Supply::Component_Initialize() {

	Component_Transform->Set_Pos(1.f, 1.f, 1.f);

	return S_OK;
}
Supply* Supply::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Supply* MST = new Supply(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create Supply.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}

VOID Supply::Free() {

	GameObject::Free();
}

