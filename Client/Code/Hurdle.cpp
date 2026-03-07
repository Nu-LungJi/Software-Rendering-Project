#include "../Include/PCH.h"
#include "Hurdle.h"

Hurdle::Hurdle(LPDIRECT3DDEVICE9 _GRPDEV): GameObject(_GRPDEV) {}
Hurdle::Hurdle(const GameObject& _RHS):GameObject(_RHS){}
Hurdle::~Hurdle() {}

Hurdle* Hurdle::Create(LPDIRECT3DDEVICE9 _GRPDEV, HurdleType eType, _vec3 vPos, _vec3 vScale){
	Hurdle* pHurdle = DBG_NEWW Hurdle(_GRPDEV);
	if (FAILED(pHurdle->Ready_GameObject(eType, vPos, vScale))) {
		Safe_Release(pHurdle);
		return nullptr;
	}
	return pHurdle;
}

HRESULT Hurdle::Ready_GameObject(HurdleType eType, _vec3 vPos, _vec3 vScale)
{
	ObjectTAG = L"Wall";
	Hurdle::Component_Initiaize();
	Component_Transform->Set_Pos(vPos);
	Component_Transform->Set_Scale(vScale);

	_vec3 ColScale = vScale * 0.5f;
	Component_Collider->Set_Scale(ColScale.x, ColScale.y, ColScale.z);
	Monster::Staic_Obj(GRPDEV, Component_Transform);

	m_pTexture = ResourceManager::GetInstance()->Find_Texture(L"CubeFloorTileDark.dds");

	return S_OK;

	//m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Tile, eType, 0);

	//return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}

INT Hurdle::Update_GameObject(const _float& _DT)
{
	if (m_bMoved) {
		Monster::Staic_Obj(GRPDEV, Component_Transform);
		_vec3 vColScale = *Component_Transform->Get_Scale() * 0.5f;
		Component_Collider->Set_Scale(vColScale.x, vColScale.y, vColScale.z);
	}
	return 0;
}

VOID Hurdle::LateUpdate_GameObject(const _float& _DT)
{
	if (m_bMoved) {
		GameObject::LateUpdate_GameObject(_DT);
		m_bMoved = false;
	}
	//GameObject::LateUpdate_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
}


VOID Hurdle::Render_GameObject()
{
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	GRPDEV->SetTexture(0, m_pTexture);
	//GRPDEV->SetTexture(0, (*m_tInfo.Textureinfo.pTexture)[0]);
	Component_Buffer->Render_Buffer();
}

HRESULT Hurdle::Component_Initiaize()
{
	Component_Buffer	= ADD_COMPONENT_CUBE;

	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Collider	= ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	return S_OK;
}

BOOL Hurdle::OnCollisionStay(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();
	if (Tag == L"Player") {
		Monster::Hurdle_CollisionStay(this, _Other);
		return true;
	}
	return 0;
}

VOID Hurdle::Free() {
	m_pCam = nullptr;
	GameObject::Free();
}