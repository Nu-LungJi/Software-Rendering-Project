#include "../Include/PCH.h"

CLEffect::CLEffect(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_bNextEffect(false), m_fSpeed(0.f), m_fAlpha(0.f), m_iBulletCnt(0),m_fRotY(0.f), m_fAngle(0.f), m_TextureIndex(0), m_iCnt(0), m_FrameTick(0.f), m_bDead(false){}
CLEffect::CLEffect(const GameObject& _RHS) : GameObject(_RHS), m_TextureIndex(0), m_FrameTick(0.f) {}
CLEffect::~CLEffect() {}

HRESULT CLEffect::Ready_Effect(CL_EFFECT eEffect, _vec3 vPos, _bool bDead, _vec3 vScale, _vec3 vRot, FLOAT fFrame, _vec3 vLook, _bool bNext) {
	if (FAILED(Component_Initialize(eEffect))) return E_FAIL;

	_bool AngleChase = true;
	Set_ObjectTag(L"CheonLog_Leaf");
	switch (eEffect)
	{
	case CL_EFFECT::LEFT_HORN:
		break;
	case CL_EFFECT::RIGHT_HORN:
		break; 
	case CL_EFFECT::CL_BODY:
		break;
	case CL_EFFECT::LEAF_FIRST:
		SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/No.033_Cheonlog'sHorn_Charge1.wav", CHANNELID::SOUND_EFFECT02, 0.3f);
		break;
	case CL_EFFECT::LEAF_EXPLOSION_CIRCLE:
		break;
	case CL_EFFECT::LEAF_CHARGING:
		break;
	case CL_EFFECT::LEAF_SPIN:
		break;
	case CL_EFFECT::LEAF_SPIN_DEATH:
		break;
	case CL_EFFECT::SPAWN_BOOM:
		break;
	case CL_EFFECT::SPAWN_THUNDER:
		break;
	case CL_EFFECT::SPAWN_BOOM_CIRCLE:
		break;
	case CL_EFFECT::SPAWN_L:
		break;
	case CL_EFFECT::SPAWN_R:
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
	m_fSpeed = 0.1f;
	m_fFrame = fFrame;
	m_fAlpha = 1.f;
	m_bNextEffect = bNext;

	return S_OK;
}

INT  CLEffect::Update_GameObject(const _float& _DT) {
	GameObject::Update_GameObject(_DT);
	Move_Frame(_DT);
	Move_Pos();

	if (m_fAlpha > 0.f)
		m_fAlpha -= _DT * m_fSpeed;
	if (m_fAlpha < 0.f)
		m_fAlpha = 0.f;
	return 0;
}
void CLEffect::LateUpdate_GameObject(const _float& _DT) {
	
	if (dynamic_cast<Cheonlog*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog"))->Get_Statu() == CL_DEAD)
		Set_ObjectDead(TRUE);

}
void CLEffect::Render_GameObject() {
	
	if (!AlphaStart(m_eEffect))
	{
		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		GRPDEV->SetTexture(0, CHEONLOG->Get_EffectTexture(m_eEffect)[m_TextureIndex]);
	}
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	Component_Buffer->Render_Buffer();

	if (!AlphaEnd(m_eEffect))
	{
		GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}
_bool CLEffect::AlphaStart(CL_EFFECT eid)
{
	if (eid == CL_EFFECT::SPAWN_BOOM || eid == CL_EFFECT::SPAWN_BOOM_CIRCLE || eid == CL_EFFECT::SPAWN_THUNDER)
	{
		DWORD tfactor = D3DCOLOR_ARGB(
			(BYTE)(m_fAlpha * 255.f),
			255, 255, 255
		);

		GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, tfactor);

		GRPDEV->SetTexture(0, CHEONLOG->Get_EffectTexture(m_eEffect)[m_TextureIndex]);

		// COLOR = Texture * TFACTOR
		GRPDEV->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

		// ALPHA = TextureAlpha * TFACTORAlpha
		GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		return true;
	}

	return false;
}
_bool CLEffect::AlphaEnd(CL_EFFECT eid)
{
	if (eid == CL_EFFECT::SPAWN_BOOM || eid == CL_EFFECT::SPAWN_BOOM_CIRCLE || eid == CL_EFFECT::SPAWN_THUNDER)
	{
		GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
		GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		return true;
	}
	return false;
}
void CLEffect::Move_Normal(const _float& _DT)
{
	_vec3 vScale, vPos;
	vScale = *Component_Transform->Get_Scale();
	vPos = *Component_Transform->Get_Position();

	if (m_FrameTick > m_fFrame)
	{
		++m_TextureIndex;
		m_FrameTick = 0.f;
		if (m_eEffect == CL_EFFECT::LEAF_CHARGING)
		{
			m_vScale -= {0.2f, 0.2f, 0.2f};
			Component_Transform->Set_Scale(m_vScale);
		}
		
		if (m_eEffect == CL_EFFECT::SPAWN_BOOM_CIRCLE)
		{
			vScale += {0.2f, 0.2f, 0.2f};
			Component_Transform->Set_Scale(vScale);
			if (m_TextureIndex == 7)
				EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_BOOM, vPos, TRUE,{1.5f,1.5f,1.5f},{65,0,0},0.1f));
				
		}
		if (m_eEffect  == CL_EFFECT::SPAWN_BOOM)
		{
			vScale += {0.3f, 0.3f, 0.3f};
			Component_Transform->Set_Scale(vScale);
		}

		if (m_TextureIndex > CHEONLOG->Get_EffectTexture(m_eEffect).size() - 1)
		{
			m_TextureIndex = 0;

			if (m_bDead)
			{
				if (m_eEffect == CL_EFFECT::SPAWN_THUNDER && m_bNextEffect)
					EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_BOOM_CIRCLE, {vPos.x, 1.f, vPos.z}, TRUE, {1.5f,1.5f,1.5f}, {65,0,0}, 0.1f));

				if (m_eEffect == CL_EFFECT::SPAWN_BOOM)
					dynamic_cast<Cheonlog*>(SceneManager::GetInstance()->Get_GameObject(L"CheonLog"))->Set_Statu(vPos);
			
				Set_ObjectDead(m_bDead);
			}
		}
	}
}

void CLEffect::Move_Frame(const _float& _DT)
{
	m_FrameTick += _DT;
	_vec3 vRot, vPos, vScale;
	_matrix matRotZ, matWorld, matRotY, matScale, matInverse, matBill;
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
	case CL_EFFECT::LEAF_EXPLOSION_CIRCLE:
		if (m_bDead == FALSE)
		{
			if (m_FrameTick > m_fFrame)
			{
				++m_TextureIndex;
				m_FrameTick = 0.f;
				++m_iCnt;
			}
			if (m_TextureIndex > CHEONLOG->Get_EffectTexture(m_eEffect).size() - 1)
			{
				m_TextureIndex = CHEONLOG->Get_EffectTexture(m_eEffect).size() - 1;
			}
			if (m_iCnt > 17)
				Set_ObjectDead(TRUE);
		}
		else if (m_bDead == TRUE)
		{
			++m_TextureIndex;
			m_FrameTick = 0.f;

			if (m_TextureIndex > CHEONLOG->Get_EffectTexture(m_eEffect).size() - 1)
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
		if (dynamic_cast<Cheonlog*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog"))->Get_Check() == IDEL)
			Set_ObjectDead(TRUE);
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
	case CL_EFFECT::SPAWN_BOOM_CIRCLE:
		Move_Normal(_DT);
		break;
	case CL_EFFECT::SPAWN_L:
		Move_Normal(_DT);
		break;
	case CL_EFFECT::SPAWN_R:
		Move_Normal(_DT);
		break;
	case CL_EFFECT::LEAF_CHARGING:
		Move_Normal(_DT);
		break;

	}
}

void CLEffect::Move_Pos()
{
	switch (m_eEffect)
	{
	case CL_EFFECT::LEFT_HORN:
		if(dynamic_cast<Cheonlog*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog"))->Get_Statu() == CL_LJUMP)
			Pos_Check(-2.0f, 0.8f, 1.1f);
		else Pos_Check(0.0f, 0.8f, 1.1f);
		break;
	case CL_EFFECT::RIGHT_HORN:
		if (dynamic_cast<Cheonlog*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog"))->Get_Statu() == CL_LJUMP)
			Pos_Check(0.3f, 0.8f, 1.1f);
		else 	Pos_Check(2.3f, 0.8f, 1.1f);
		break;
	case CL_EFFECT::CL_BODY:
		Pos_Check(-1.2f, 0.f, -0.5f);
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
		pAttack = CLAttack::Create(GRPDEV, eid, m_vPos, vLook, bSpin);
		pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
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

	
	Component_Texture = ADD_COMPONENT_TEXTURE;

	return S_OK;
}
CLEffect* CLEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, CL_EFFECT eEffect, _vec3 vPos, _bool bDead, _vec3 vScale , _vec3 vRot, FLOAT fFrame, _vec3 vLook, _bool bNext) {
	CLEffect* EFT = new CLEffect(_GRPDEV);
	if (FAILED(EFT->Ready_Effect(eEffect, vPos, bDead, vScale, vRot, fFrame, vLook, bNext))) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(EFT);
		return nullptr;
	}
	return EFT;
}
void CLEffect::Free() {
	GameObject::Free();
}