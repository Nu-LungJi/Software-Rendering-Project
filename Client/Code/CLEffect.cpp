#include "../Include/PCH.h"

CLEffect::CLEffect(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_iPotalCnt(0), m_bNextEffect(false), m_fSpeed(0.f), m_fAlpha(0.f), m_iBulletCnt(0),m_fRotY(0.f), m_fAngle(0.f), m_TextureIndex(0), m_iCnt(0), m_FrameTick(0.f), m_bDead(false){}
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
		SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Chunlog_pattern_rage1.wav", CHANNELID::SOUND_EFFECT02, 0.4f);
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
	case CL_EFFECT::SPAWN_POTAL:
		CollisionManager::GetInstance()->Add_ColliderObject(this);
		Make_EffectTextureList(L"BlackHole_Open");
		break;
	case CL_EFFECT::SPAWN_ITEM:
		INT FRAME = 0;
		while (++FRAME) {
			wstring FileName = L"../../Resource/Weapon/DropEffect/Spr_Ui_Effect_DeathResult_0" + to_wstring(FRAME) + L".png";
			IDirect3DBaseTexture9* TEX = nullptr;
			D3DXCreateTextureFromFileExW(GRPDEV, FileName.c_str(), 200, 300,
				1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&TEX);

			if (TEX == nullptr) break;
			else { m_vecTextureList.push_back(TEX); }
		}
	
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
HRESULT CLEffect::Make_EffectTextureList(wstring _FileName)
{
	INT FRAME = 0;
	while (++FRAME) {
		wstring FileName = _FileName + to_wstring(FRAME) + L".dds";
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { m_vecTextureList.push_back(TEX); }
	}

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
	GameObject::LateUpdate_GameObject(_DT);
	if (m_eEffect != CL_EFFECT::SPAWN_ITEM && m_eEffect != CL_EFFECT::SPAWN_POTAL && dynamic_cast<Cheonlog*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog"))->Get_Statu() == CL_DEAD)
	{
		Set_ObjectDead(TRUE);
	}
}
void CLEffect::Render_GameObject() {
	_vec3 vPos{ 0,0,0 };
	if (!AlphaStart(m_eEffect))
	{
		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		switch (m_eEffect)
		{
		case CL_EFFECT::SPAWN_POTAL:
			GRPDEV->SetTexture(0, m_vecTextureList[m_TextureIndex]);
			break;
		case CL_EFFECT::SPAWN_ITEM:

			m_pSprite->SetTransform(&m_matWorld);
			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
			m_pSprite->Draw((LPDIRECT3DTEXTURE9)m_vecTextureList[m_TextureIndex], NULL, NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
			m_pSprite->End();
			D3DXMatrixIdentity(&m_matWorld);
			m_pSprite->SetTransform(&m_matWorld);
			break;
		default:
			if (CHEONLOG == nullptr)
				return;
			GRPDEV->SetTexture(0, CHEONLOG->Get_EffectTexture(m_eEffect)[m_TextureIndex]);
			break;
		}
	}
	if (m_eEffect != CL_EFFECT::SPAWN_ITEM)
	{
		GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
		Component_Buffer->Render_Buffer();
	}
	
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
void CLEffect::Move_Potal(const _float& _DT)
{
	_vec3 vScale, vPos;
	vPos = *Component_Transform->Get_Position();

	Component_Transform->Set_Scale(8.f, 5.f, 5.f);
	if (m_FrameTick > 0.1f)
	{
		++m_TextureIndex;
		m_FrameTick = 0.f;

		if (m_TextureIndex > m_vecTextureList.size() - 1)
		{
			m_TextureIndex = m_vecTextureList.size() - 5;
			++m_iPotalCnt;
			if (m_bDead)
			{
				CollisionManager::GetInstance()->Delete_ColliderObject(this);
				SoundManager::GetInstance()->Stop_AllSound();
				dynamic_cast<StageBlackOut*>(EffectManager::GetInstance()->Get_Scene())->Set_Pos({57.726f,0.5f,16.321f}, false, 0, false,SCENE_EFFECT::SCENE_BOSS);
				TileManager::GetInstance()->Set_CurStage(TILE_STAGE::TILE_DOCHER1);
				Set_ObjectDead(m_bDead);
			}
		}
	}
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
				
				if(m_eEffect == CL_EFFECT::LEAF_SPIN_DEATH)
					SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_SeedlingDeath.wav", CHANNELID::SOUND_EFFECT02, 0.2f);

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
	case CL_EFFECT::SPAWN_POTAL:
		Move_Potal(_DT);
		break;
	case CL_EFFECT::SPAWN_ITEM:
		Move_ItemEffect(_DT);
		Drop_Item(_DT);
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
void CLEffect::Move_ItemEffect(const _float& _DT)
{
	_vec3 vScale, vPos;
	vScale = *Component_Transform->Get_Scale();
	vPos = *Component_Transform->Get_Position();

	if (m_FrameTick > 0.1)
	{
		++m_TextureIndex;
		m_FrameTick = 0.f;
		
		if(m_TextureIndex > m_vecTextureList.size() -1 )
		{
			_vec3 vPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"SpawnITem")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();

			Set_ObjectDead(TRUE);
			vPos.y += 3.f;
			vPos.z += 12.f;
			EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::UI, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_POTAL, vPos, FALSE));

		}
		
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
		SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_SmallLeaf_Shot_01.wav", CHANNELID::SOUND_EFFECT03, 0.05f);

		dynamic_cast<CLAttack*>(CHEONLOG->Get_PollMainBullet(eid).back())->Set_Look(vLook, m_vPos, bSpin);
		CollisionManager::GetInstance()->Add_ColliderObject(CHEONLOG->Get_PollMainBullet(eid).back());
		CHEONLOG->Get_OriginBullet(eid).push_back(CHEONLOG->Get_PollMainBullet(eid).back());
		
		CHEONLOG->Get_PollMainBullet(eid).pop_back();
	}
}
void CLEffect::Pos_Check(_float x, _float y, _float z)
{
	_vec3 pPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();

	pPos += {x, y, z};
	Component_Transform->Set_Pos(pPos);
}

void CLEffect::Drop_Item(const _float _DT)
{
	_vec3 Screen, vScale;
	_matrix matWorld, matScale, matRotZ, matTrans, matView, matProj;
	_vec3 vPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"SpawnITem")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
	vScale = *Component_Transform->Get_Scale();
	_D3DVIEWPORT9 viewPort;
	GRPDEV->GetTransform(D3DTS_VIEW, &matView);
	GRPDEV->GetTransform(D3DTS_PROJECTION, &matProj);
	GRPDEV->GetViewport(&viewPort);
	m_fAngle += 3.f * 7.f, _DT;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXVec3Project(&Screen, &vPos, &viewPort, &matProj, &matView, &m_matWorld);
	
	
	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixTranslation(&matTrans, Screen.x-100, Screen.y-160, 0);
	
	m_matWorld = matScale * matTrans;
}

BOOL	CLEffect::OnCollisionStay(GameObject* _Other)
{
	if (_Other->Get_ObjectTag() == L"Player" && KeyManager::GetInstance()->Get_KeyState(DIK_E))
	{
		m_bDead = true;
		return true;
	}

	return false;
}
HRESULT	CLEffect::Component_Initialize(CL_EFFECT eEffect) {
	
	if (CL_EFFECT::LEAF_EXPLOSION_CIRCLE == eEffect)
	{
		Component_Buffer = ADD_COMPONENT_TILE;
	}else
		Component_Buffer = ADD_COMPONENT_RECTTEX;

	Component_Transform = ADD_COMPONENT_TRANSFORM;

	
	Component_Texture = ADD_COMPONENT_TEXTURE;

	if (CL_EFFECT::SPAWN_POTAL == eEffect)
	{
		Component_Colider = ADD_COMPONENT_COLLIDER;
		Component_Colider->Set_CenterPos(Component_Transform);
		Component_Colider->Set_Scale(8.f, 5.f, 2.f);
	}
	
	D3DXCreateSprite(GRPDEV, &m_pSprite);
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
	for (auto& iter : m_vecTextureList)
		Safe_Release(iter);

	Safe_Release(m_pSprite);
	GameObject::Free();
}