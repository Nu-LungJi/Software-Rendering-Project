#include "../Include/PCH.h"

CLEffect::CLEffect(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_iBulletCnt(0),m_fRotY(0.f), m_fAngle(0.f), m_TextureIndex(0), m_iCnt(0), m_FrameTick(0.f), m_bDead(false){}
CLEffect::CLEffect(const GameObject& _RHS) : GameObject(_RHS), m_TextureIndex(0), m_FrameTick(0.f) {}
CLEffect::~CLEffect() {}

HRESULT CLEffect::Ready_Effect(CL_EFFECT eEffect, _vec3 vPos, _bool bDead, _vec3 vScale, _vec3 vRot, FLOAT fFrame, _vec3 vLook) {
	if (FAILED(Component_Initialize(eEffect))) return E_FAIL;

	_bool AngleChase = true;

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
	case CL_EFFECT::LEAF_EXPLOSION:
		Make_TextureList(L"Spr_Effect_Cheonlog_BigExplosione_Birth");
		break;
	case CL_EFFECT::LEAF_EXPLOSION_CIRCLE:
		Make_TextureList(L"Spr_Ui_Effect_ChaosGazeCircleEffect01_0");
		break;
	case CL_EFFECT::LEAF_CHARGING:
		Make_TextureList(L"Spr_Effect_Cheonlog_ChargeAccelReturn_Birth01_0");
		break;
	case CL_EFFECT::LEAF_SPIN:
		Make_TextureList(L"Spr_Effect_Cheonlog_RadialCrossSplit_Loop_0");
		break;
	case CL_EFFECT::LEAF_SPIN_DEATH:
		Make_TextureList(L"Spr_Bullet_Cheonlog_DivideFlower_Death_0");
		break;
	case CL_EFFECT::SPAWN_BOOM:
		Make_TextureList(L"Spr_Effect_Cheonlog_Appear_Electric01_0");
		break;
	case CL_EFFECT::SPAWN_THUNDER:
		Make_TextureList(L"Spr_Effect_Cheonlog_RadialCrossSplit_Birth01_0");
		break;
	}

	m_vScale = vScale;
	Component_Transform->Set_Pos(vPos);
	Component_Transform->Set_Rotation(vRot);
	Component_Transform->Set_Scale(vScale);
	m_vPos = vPos;
	m_vLook = vLook;
	m_eEffect = eEffect;
	m_bDead = bDead;
	m_fFrame = fFrame;
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
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, TextureList[m_TextureIndex]);

	Component_Buffer->Render_Buffer();
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GRPDEV->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CLEffect::Move_Normal(const _float& _DT)
{
	if (m_FrameTick > m_fFrame)
	{
		++m_TextureIndex;
		m_FrameTick = 0.f;
		if (m_eEffect == CL_EFFECT::LEAF_CHARGING)
		{
			m_vScale -= {0.2f, 0.2f, 0.2f};
			Component_Transform->Set_Scale(m_vScale);
		}
		if (m_TextureIndex > TextureList.size() - 1)
		{
			m_TextureIndex = 0;

			if (m_bDead)
			{
				Set_ObjectDead(m_bDead);
			}
		}
	}
}
void CLEffect::Move_Frame(const _float& _DT)
{
	m_FrameTick += _DT;
	_vec3 vRot, vPos, vScale;
	_matrix matRotZ, matWorld,matRotY, matScale,matInverse,matBill;
	vRot = *Component_Transform->Get_Rotation();
	switch (m_eEffect)
	{
	case CL_EFFECT::LEAF_FIRST:
		Move_Normal(_DT);
		break;
	case CL_EFFECT::CL_BODY:
		Move_Normal(_DT);
		break;
	case CL_EFFECT::LEFT_HORN:
		Move_Normal(_DT);
		break;
	case CL_EFFECT::RIGHT_HORN:
		Move_Normal(_DT);
		break;
	case CL_EFFECT::LEAF_EXPLOSION:
		Move_Normal(_DT);
		Component_Transform->Get_Info(INFO_POS, &vPos);
		if (m_TextureIndex == 16)
			EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEAF_EXPLOSION_CIRCLE, { vPos.x,-0.2f,vPos.z - 3 }, TRUE, { 6,0,5 }, { 20,0,0 }, 0.1f));
		if (m_TextureIndex == 25)
			Component_Transform->Set_Scale(5.f, 5.f, 5.f);
		break;
	case CL_EFFECT::LEAF_EXPLOSION_CIRCLE:
		if (m_bDead == FALSE)
		{
			if (m_FrameTick > m_fFrame)
			{
				++m_TextureIndex;
				m_FrameTick = 0.f;
				++m_iCnt;
			}
			if (m_TextureIndex > TextureList.size() - 1)
			{
				m_TextureIndex = TextureList.size() - 1;
			}
			if (m_iCnt > 17)
				Set_ObjectDead(TRUE);
		}
		else if (m_bDead == TRUE)
		{
			++m_TextureIndex;
			m_FrameTick = 0.f;

			if (m_TextureIndex > TextureList.size() - 1)
			{
				m_TextureIndex = 0;

				if (m_bDead)
				{
					Set_ObjectDead(m_bDead);
				}
			}
		}
		break;
	case CL_EFFECT::LEAF_SPIN:
		if (m_FrameTick > m_fFrame && CL_EFFECT::LEAF_SPIN == m_eEffect && m_TextureIndex != 5)
		{
			m_FrameTick = 0.f;
			m_vScale += {0.3f, 0.3f, 0.3f};
			Component_Transform->Set_Scale(m_vScale);
			++m_TextureIndex;
		}
		else
		{
			m_fAngle += 3.f;
			vScale = *Component_Transform->Get_Scale();
			D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);
			D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fAngle));

			matWorld = *Component_Transform->Get_World();
			GRPDEV->GetTransform(D3DTS_VIEW, &matInverse);
			D3DXMatrixInverse(&matBill, nullptr, &matInverse);

			matWorld = matScale * matRotZ * matBill;
			memcpy(matWorld.m[3], m_vPos, sizeof(_vec3));

			Component_Transform->Set_World(&matWorld);
			Component_Transform->Set_Pos(matWorld._41, matWorld._42, matWorld._43);
		}
		break;
	case CL_EFFECT::LEAF_SPIN_DEATH:
		Move_Normal(_DT);
		Effect_Bill();
		Effect_Dead_After(LEAF_ATTACK::LEAF_SECOND, m_vLook,true);
		Effect_Dead_After(LEAF_ATTACK::LEAF_THIRD,  *D3DXVec3TransformNormal(&vRot, &m_vLook,D3DXMatrixRotationY(&matRotY,D3DXToRadian(90))), true);
		break;
	case CL_EFFECT::SPAWN_BOOM:
		Move_Normal(_DT);
		break;
	case CL_EFFECT::SPAWN_THUNDER:
		Move_Normal(_DT);
		break;
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
void CLEffect::Effect_Bill()
{
	_matrix matScale, RotZ, matWorld, matBill, matView;

	matWorld = *Component_Transform->Get_World();
	GRPDEV->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matBill, nullptr, &matView);

	D3DXMatrixScaling(&matScale, 0.4f, 0.4f, 0.4f);

	matWorld = matScale * matBill;

	memcpy(matWorld.m[3], m_vPos, sizeof(_vec3));
	Component_Transform->Set_World(&matWorld);
	Component_Transform->Set_Pos({ matWorld._41 , matWorld._42, matWorld._43 });
}
void CLEffect::Effect_Dead_After(LEAF_ATTACK eid, _vec3 vLook, _bool bSpin)
{
	if (Get_ObjectDead() == TRUE)
	{
		CLAttack* pAttack = nullptr;
		TCHAR tChar[128] = {};
		wsprintf(tChar, L"CL_Leaf%d", m_iBulletCnt);
		pAttack = CLAttack::Create(GRPDEV, eid, m_vPos, vLook, bSpin);
		pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		pAttack->Set_ObjectTag(tChar);
		SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pAttack);
	}
}
void CLEffect::Pos_Check(_float x, _float y, _float z)
{
	_vec3 pPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();

	pPos += {x, y, z};
	Component_Transform->Set_Pos(pPos);
}


HRESULT	CLEffect::Component_Initialize(CL_EFFECT eEffect) {
	
	if (CL_EFFECT::LEAF_EXPLOSION_CIRCLE == eEffect)
	{
		Component_Buffer = ADD_COMPONENT_TILE;
	}else
		Component_Buffer = ADD_COMPONENT_RECTTEX;

	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);			// 충돌체가 오브젝트를 따라 다니도록
	Component_Collider->Set_Scale(0.1f, 0.1f, 0.1f);				// 충돌체의 범위 조절

	Component_Texture = ADD_COMPONENT_TEXTURE;

	return S_OK;
}
CLEffect* CLEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, CL_EFFECT eEffect, _vec3 vPos, _bool bDead, _vec3 vScale , _vec3 vRot, FLOAT fFrame, _vec3 vLook) {
	CLEffect* EFT = new CLEffect(_GRPDEV);
	if (FAILED(EFT->Ready_Effect(eEffect, vPos, bDead, vScale, vRot, fFrame, vLook))) {
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