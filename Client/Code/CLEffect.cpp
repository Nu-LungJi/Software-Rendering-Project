#include "../Include/PCH.h"

CLEffect::CLEffect(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_TextureIndex(0), m_FrameTick(0.f), m_bDead(false){}
CLEffect::CLEffect(const GameObject& _RHS) : GameObject(_RHS), m_TextureIndex(0), m_FrameTick(0.f) {}
CLEffect::~CLEffect() {}

HRESULT CLEffect::Ready_Effect(CL_EFFECT eEffect, _vec3 vPos, _bool bDead) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	_bool AngleChase = true;

	m_eEffect = eEffect;
	m_bDead = bDead;
	switch (eEffect)
	{
	case CL_EFFECT::LEFT_HORN:
		Make_TextureList(L"Spr_Effect_Cheonlog_AttackMode_Rage_0");
		break;
	case CL_EFFECT::RIGHT_HORN:
		Make_TextureList(L"Spr_Effect_Cheonlog_AttackMode_Rage_0");
		break; 
	case CL_EFFECT::CL_BODY:
		Make_TextureList(L"Spr_Effect_Cheonlog_AttackModeEffect_0");
		break;
	case CL_EFFECT::LEAF_FIRST:
		Make_TextureList(L"Spr_Effect_Cheonlog_BaseBullet_Birth01_0");
		break;
	case CL_EFFECT::LEAF_SECOND:
		Make_TextureList(L"Spr_Effect_Cheonlog_BigExplosione_Birth0");
		break;
	case CL_EFFECT::LEAF_SECOND_E:
		Make_TextureList(L"Spr_Cheonlog_BigExplosion_0");
		break;
	case CL_EFFECT::LEAF_SECOND_B:
		Make_TextureList(L"Spr_Effect_Cheonlog_BigExplosionToPlayer_0");
		break;
	case CL_EFFECT::LEAF_SECOND_C:
		Make_TextureList(L"Spr_Ui_Effect_ChaosGazeCircleEffect01_0");
		break;
	}
	
	Component_Transform->Set_Pos(vPos);
	Component_Transform->Set_Rotation(45.f, 0.f, 0.f);
	Component_Transform->Set_Scale(1.5f, 1.5f, 1.5f);
	return S_OK;
}

HRESULT CLEffect::Make_TextureList(wstring _FileName) {
	INT FRAME = 0;

	while (++FRAME) {
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { TEX->AddRef();  TextureList.push_back(TEX); }
	}

	return S_OK;
}

INT  CLEffect::Update_GameObject(const _float& _DT) {
	GameObject::Update_GameObject(_DT);
	Move_Frame(_DT);
	Move_Pos();
	return 0;
}
void CLEffect::LateUpdate_GameObject(const _float& _DT) {

}
void CLEffect::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, TextureList[m_TextureIndex]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
void CLEffect::Move_Frame(const _float& _DT)
{
	m_FrameTick += _DT;

	if (m_FrameTick > 0.1f)
	{
		++m_TextureIndex;
		m_FrameTick = 0.f;
	}
	if (m_TextureIndex > TextureList.size() - 1)
	{
		m_TextureIndex = 0;

		if (m_bDead)
		{
			Set_ObjectDead(m_bDead);

			Cheonlog* pCL = dynamic_cast<Cheonlog*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog"));

		}
	}
		
}

void CLEffect::Move_Pos()
{
	switch (m_eEffect)
	{
	case CL_EFFECT::LEFT_HORN:
		Pos_Check(0.0f, 0.5f, 2.7f);
		break;
	case CL_EFFECT::RIGHT_HORN:
		Pos_Check(2.3f, 0.0f, 3.2f);
		break;
	case CL_EFFECT::CL_BODY:
		Pos_Check(-1.2f, 0.2F, 0.8f);
		break;
	case CL_EFFECT::LEAF_FIRST:
		break;
	}

}

void CLEffect::Pos_Check(_float x, _float y, _float z)
{
	_vec3 pPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();

	pPos += {x, y, z};
	Component_Transform->Set_Pos(pPos);
}


HRESULT	CLEffect::Component_Initialize() {
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);			// 충돌체가 오브젝트를 따라 다니도록
	Component_Collider->Set_Scale(0.1f, 0.1f, 0.1f);				// 충돌체의 범위 조절

	Component_Texture = ADD_COMPONENT_TEXTURE;

	return S_OK;
}
CLEffect* CLEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, CL_EFFECT eEffect, _vec3 vPos, _bool bDead) {
	CLEffect* EFT = new CLEffect(_GRPDEV);
	if (FAILED(EFT->Ready_Effect(eEffect, vPos, bDead))) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(EFT);
		return nullptr;
	}
	return EFT;
}
void CLEffect::Free() {
	for (auto& iter : TextureList)
		Safe_Release(iter);

	TextureList.clear();
	GameObject::Free();
}