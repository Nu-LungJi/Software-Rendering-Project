#include "TileDestoryEffect.h"
#include "../Include/PCH.h"

TileDestoryEffect::TileDestoryEffect(LPDIRECT3DDEVICE9 _GRPDEV) :GameObject(_GRPDEV), m_bEffect(true), m_fTime(0), m_fFrame(0), m_pTileEffectBuff(nullptr), m_pTransform(nullptr){}
TileDestoryEffect::TileDestoryEffect(const GameObject& _RHS) : GameObject(_RHS) {}
TileDestoryEffect::~TileDestoryEffect() {  }

HRESULT TileDestoryEffect::Ready_GameObject(OBJECT_DESTORY eid, _int iCnt, _vec3 vPos, _vec3 vScale, _vec3 vRot, _bool bOther) {


	if (FAILED(Component_Initialize(bOther, eid))) return E_FAIL;

	m_pTransform->Set_Pos(vPos);
	m_pTransform->Set_Scale(vScale);
	m_pTransform->Set_Rotation(vRot);
	m_eDestory = eid;
	switch (m_eDestory)
	{
		case OBJECT_DESTORY::STONE:
		Add_Effect(OBJECT_DESTORY::STONE, L"StoneBox_Destruction_");
		break;
		case OBJECT_DESTORY::POTALEFFECT:
		Add_Effect(OBJECT_DESTORY::POTALEFFECT, L"Spr_InfectionThorns_DestructionEffect_0");
		break;
		case OBJECT_DESTORY::BOOM_F:
		Add_Effect(OBJECT_DESTORY::BOOM_F, L"DangerArea0");
		break;
		case OBJECT_DESTORY::BOOM_S:
		Add_Effect(OBJECT_DESTORY::BOOM_S, L"Spr_Effect_No027_GunpowderBowPulse_0");
		break;
		case OBJECT_DESTORY::BOOM_T:
		Add_Effect(OBJECT_DESTORY::BOOM_T, L"Spr_Effect_FireBird_FireBallPulse_0");
		break;
	}
		
	return S_OK;
}
INT	TileDestoryEffect::Update_GameObject(const _float& _DT) {

	GameObject::Update_GameObject(_DT);

	Frame_Move(_DT);
	
	{
		if (m_eDestory != OBJECT_DESTORY::BOOM_F)
		{

			_vec3 vScale, vPos;
			_matrix matScale, RotZ, matWorld, matBill, matView;
			vScale = *m_pTransform->Get_Scale();
			matWorld = *m_pTransform->Get_World();
			vPos = *m_pTransform->Get_Position();
			GRPDEV->GetTransform(D3DTS_VIEW, &matView);
			D3DXMatrixInverse(&matBill, nullptr, &matView);

			D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);

			matWorld = matScale * matBill;
			memcpy(matWorld.m[3], vPos, sizeof(_vec3));
			m_pTransform->Set_World(&matWorld);
			m_pTransform->Set_Pos({ matWorld._41 , matWorld._42 , matWorld._43 });

		}	
	}

		return 0;
}
VOID TileDestoryEffect::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

}

VOID TileDestoryEffect::Render_GameObject()
{
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	if (m_bEffect)
	{
		GRPDEV->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());
		GRPDEV->SetTexture(0, m_vecTileEffectList[static_cast<int>(m_eDestory)][m_fFrame]);

		m_pTileEffectBuff->Render_Buffer();

	}

	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


void TileDestoryEffect::Frame_Move(const FLOAT& _DT)
{ 
	_vec3 Pos, Scale, Rot;
	m_pTransform->Get_Info(INFO_POS, &Pos);
	Scale = *m_pTransform->Get_Scale();
	Rot = *m_pTransform->Get_Rotation();
	m_fTime += _DT;
	if (m_bEffect)
	{
		switch (m_eDestory)
		{
		case OBJECT_DESTORY::STONE:
			Frame_Normal(_DT);
			break;
		case OBJECT_DESTORY::POTALEFFECT:
			Frame_Normal(_DT);
			break;
		case OBJECT_DESTORY::BOOM_F:
		 //지난 시간
			if (m_fTime > 0.1f) //0.1초가 지나면
			{
				++m_fFrame;     //프레임 증가
				m_fTime = 0.f;	//시간 초기화

				if (m_fFrame > m_vecTileEffectList[static_cast<int>(m_eDestory)].size() - 1)
				{
					//m_fFrame = 1.f;
					m_bEffect = false;
					Pos.y += 2.f;
					Pos.z -= 0.5f;

					EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::UI, TileDestoryEffect::Create(GRPDEV, OBJECT_DESTORY::BOOM_S, 0, Pos, {4.f,4.f,4.f}, Rot));

					Set_ObjectDead(TRUE);
				}
			}
			break;
		case OBJECT_DESTORY::BOOM_S:
			if (m_fTime > 0.1f) 
			{
				++m_fFrame; 
				// Scale += {0.1f, 0.1f, 0.1f};
				m_pTransform->Set_Scale(Scale);
				m_fTime = 0.f;	
				if(m_fFrame == 5)
					EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::UI, TileDestoryEffect::Create(GRPDEV, OBJECT_DESTORY::BOOM_T, 0, Pos, { 2.f,2.f,2.f }, Rot));

				if (m_fFrame > m_vecTileEffectList[static_cast<int>(m_eDestory)].size() - 1)
				{
					m_bEffect = false;
					Set_ObjectDead(TRUE);
				}
			}
			break;
		case OBJECT_DESTORY::BOOM_T:
			if (m_fTime > 0.1f)
			{
				++m_fFrame;
				Scale += {0.2f,0.2f,0.2f};
				m_pTransform->Set_Scale(Scale);
				m_fTime = 0.f;

				if (m_fFrame > m_vecTileEffectList[static_cast<int>(m_eDestory)].size() - 1)
				{
					m_bEffect = false;
					Set_ObjectDead(TRUE);
				}
			}
			break;
		}
	}
}

BOOL TileDestoryEffect::OnCollisionStay(GameObject* _Other)
{

	wstring Tag = _Other->Get_ObjectTag();
	if (Tag == L"Monster")
	{
		_float hp(0);
		hp = dynamic_cast<Collider*>(_Other->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER))->Get_Hp();
		dynamic_cast<Collider*>(_Other->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER))->Set_Hp(hp - m_pCollider->Get_Att());
	}
	else
	{
		return false;
	}

	return TRUE;
}

void TileDestoryEffect::Frame_Normal(const FLOAT& _DT)
{	 //지난 시간
	if (m_fTime > 0.1f) //0.1초가 지나면
	{
		++m_fFrame;     //프레임 증가
		m_fTime = 0.f;	//시간 초기화

		if (m_fFrame > m_vecTileEffectList[static_cast<int>(m_eDestory)].size() - 1)
		{
			m_fFrame = 1.f;
			m_bEffect = false;
			Set_ObjectDead(TRUE);
		}
	}
}

void TileDestoryEffect::Add_Effect(OBJECT_DESTORY eid, const _tchar* pName)
{
	INT FRAME = 0;

	while (++FRAME) {
		wstring FileName = pName + to_wstring(FRAME) + L".dds";
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { TEX->AddRef();  m_vecTileEffectList[static_cast<int>(eid)].push_back(TEX); }
	}

}


HRESULT TileDestoryEffect::Component_Initialize(_bool bOther, OBJECT_DESTORY eid) {

	
	if (bOther)
	{
		m_pTileEffectBuff = ADD_COMPONENT_TILE;
	}
	else
		m_pTileEffectBuff = ADD_COMPONENT_TILEFRONT;

	m_pTransform = ADD_COMPONENT_TRANSFORM;
	
	if (eid == OBJECT_DESTORY::BOOM_S)
	{
		m_pCollider = ADD_COMPONENT_COLLIDER;
		m_pCollider->Set_CenterPos(m_pTransform);
		m_pCollider->Set_Scale(1.f, 1.f, 1.f);
		m_pCollider->Set_Att(50.f);
	}
	return S_OK;
}

TileDestoryEffect* TileDestoryEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, OBJECT_DESTORY eid, _int iCnt, _vec3 vPos, _vec3 vScale, _vec3 vRot, _bool bOther) {
	
	TileDestoryEffect* pTileDestoryEffect = new TileDestoryEffect(_GRPDEV);
	
	if (FAILED(pTileDestoryEffect->Ready_GameObject(eid, iCnt, vPos, vScale, vRot, bOther))) {
		MSG_BOX("Cannot Create TileDestoryEffect.");
		Safe_Release(pTileDestoryEffect);
		return nullptr;
	}
	
	return pTileDestoryEffect;
}
VOID TileDestoryEffect::Free() {

	for (auto& iter : m_vecTileEffectList[static_cast<int>(m_eDestory)])
			Safe_Release(iter);
	
	m_vecTileEffectList->clear();

	GameObject::Free();
}