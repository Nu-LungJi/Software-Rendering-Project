#include "../Include/PCH.h"
#include "Rain.h"

Rain::Rain(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Rain::Rain(const GameObject& _RHS) : GameObject(_RHS) {}
Rain::~Rain() {}

HRESULT Rain::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	return S_OK;
}
INT	Rain::Update_GameObject(const _float& _DT) {
	if (Get_ObjectDead())
		return -1;
	
	GameObject::Update_GameObject(_DT);
	_vec3		vPos;

	dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->
		Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Info(INFO_POS, &vPos);
	_vec3 vRight{ -1,0,0 };
	Component_Transform->Set_Rotation(80.f, 0.f, 60.f);
	Component_Transform->Move_Pos(&vRight,1.f,_DT);
	Component_Transform->Set_Pos(vPos);
	Component_Buffer->Update_Particle(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_TILE, this);

	return 0;
}
VOID Rain::LateUpdate_GameObject(const _float& _DT) {
	//GameObject::LateUpdate_GameObject(_DT);
	if (TileManager::GetInstance()->Get_Stage() == TILE_STAGE::TILE_DOCHER1)
		Set_ObjectDead(TRUE);
}
VOID Rain::Render_GameObject() {
	if (TileManager::GetInstance()->Get_Stage() == TILE_STAGE::TILE_DEFENSE)
		return;
	Component_Buffer->PreRedner_Particle();
	GRPDEV->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	GRPDEV->SetTexture(0, StaticTexture);

	Component_Buffer->Render_Buffer();
	GRPDEV->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	Component_Buffer->PostRender_Particle();

}
HRESULT Rain::Component_Initialize() {

	BoundingBox bound;
	bound.vMin = { -15,-15, -15 };
	bound.vMax = { 15,10, 15 };
	Component_Buffer = ParticleRain::Create(GRPDEV,&bound,2500);
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Scale(1.f, 1.f, 1.f);

	IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(L"Spr_Effect_WaterWave02_12.dds");

	if (TEX == nullptr) return E_FAIL;
	else { TEX->AddRef();  StaticTexture = TEX; }

	return S_OK;
}
Rain* Rain::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Rain* pRain = new Rain(_GRPDEV);
	if (FAILED(pRain->Ready_GameObject())) {
		MSG_BOX("Cannot Create Rain.");
		Safe_Release(pRain);
		return nullptr;
	}
	return pRain;
}
VOID Rain::Free() {

	Safe_Release(Component_Buffer);
	Safe_Release(StaticTexture);
	GameObject::Free();
}