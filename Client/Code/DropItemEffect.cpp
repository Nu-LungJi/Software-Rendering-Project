#include "../Include/PCH.h"

DropItemEffect::DropItemEffect(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
DropItemEffect::DropItemEffect(const GameObject& _RHS) : GameObject(_RHS) {}
DropItemEffect::~DropItemEffect() {}

HRESULT DropItemEffect::Ready_GameObject(ItemINFO* _ItemInfo) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_pInfo = _ItemInfo;

	return S_OK;
}
INT	DropItemEffect::Update_GameObject(const _float& _DT)
{
	if (ObjectDead)
		return -1;

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	GameObject::Update_GameObject(_DT);


	return 0;
}
VOID DropItemEffect::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	Monster::BillBoard(Component_Transform, GRPDEV);
}
VOID DropItemEffect::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	//GRPDEV->SetTexture(0, );
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT DropItemEffect::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(10.f, 0.112f, 10.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(0.5f, 0.5f, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(0.5f, 1.f, 0.5f);

	return S_OK;
}
DropItemEffect* DropItemEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, ItemINFO* _ItemInfo) {
	DropItemEffect* MST = new DropItemEffect(_GRPDEV);
	if (FAILED(MST->Ready_GameObject(_ItemInfo))) {
		MSG_BOX("Cannot Create DropItemEffect.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}

VOID DropItemEffect::Free() {

	GameObject::Free();
}

