#include "TileDestoryEffect.h"
#include "../Include/PCH.h"

TileDestoryEffect::TileDestoryEffect(LPDIRECT3DDEVICE9 _GRPDEV) :GameObject(_GRPDEV), m_bEffect(true), m_fTime(0), m_fFrame(0), m_pTileEffectBuff(nullptr), m_pTransform(nullptr){}
TileDestoryEffect::TileDestoryEffect(const GameObject& _RHS) : GameObject(_RHS) {}
TileDestoryEffect::~TileDestoryEffect() {  }

HRESULT TileDestoryEffect::Ready_GameObject(OBJECT_DESTORY eid, _int iCnt, _vec3 vPos, _vec3 vScale, _vec3 vRot) {


	if (FAILED(Component_Initialize())) return E_FAIL;

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
	}
		
	return S_OK;
}
INT	TileDestoryEffect::Update_GameObject(const _float& _DT) {

	GameObject::Update_GameObject(_DT);

	Frame_Move(_DT);
	
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_TILE, this);
		return 0;

}
VOID TileDestoryEffect::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

}

VOID TileDestoryEffect::Render_GameObject()
{
	
	if (m_bEffect)
	{
		GRPDEV->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());
		GRPDEV->SetTexture(0, m_vecTileEffectList[static_cast<int>(m_eDestory)][m_fFrame]);

		m_pTileEffectBuff->Render_Buffer();
	}

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
					EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::ENVIROMENT, TileDestoryEffect::Create(GRPDEV, OBJECT_DESTORY::BOOM_S, 0, Pos, {5.f,5.f,5.f}, Rot));

					Set_ObjectDead(TRUE);
				}
			}
			break;
		case OBJECT_DESTORY::BOOM_S:
			if (m_fTime > 0.1f) 
			{
				++m_fFrame; 
				Scale += {0.2f, 0.2f, 0.2f};
				m_pTransform->Set_Scale(Scale);
				m_fTime = 0.f;	

				if (m_fFrame > m_vecTileEffectList[static_cast<int>(m_eDestory)].size() - 1)
				{
					//m_fFrame = 1.f;
					m_bEffect = false;
					Set_ObjectDead(TRUE);
				}
			}
			break;
		
		}
	
	
	}
	
	
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
		wstring FileName = pName + to_wstring(FRAME) + L".png";
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { TEX->AddRef();  m_vecTileEffectList[static_cast<int>(eid)].push_back(TEX); }
	}

}


HRESULT TileDestoryEffect::Component_Initialize() {

	m_pTileEffectBuff = ADD_COMPONENT_TILEFRONT;
	m_pTransform = ADD_COMPONENT_TRANSFORM;
	
	return S_OK;
}

TileDestoryEffect* TileDestoryEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, OBJECT_DESTORY eid, _int iCnt, _vec3 vPos, _vec3 vScale, _vec3 vRot) {
	
	TileDestoryEffect* pTileDestoryEffect = new TileDestoryEffect(_GRPDEV);
	
	if (FAILED(pTileDestoryEffect->Ready_GameObject(eid, iCnt, vPos, vScale, vRot))) {
		MSG_BOX("Cannot Create TileDestoryEffect.");
		Safe_Release(pTileDestoryEffect);
		return nullptr;
	}
	
	return pTileDestoryEffect;
}
VOID TileDestoryEffect::Free() {

	for(auto& iter : m_vecTileEffectList[static_cast<int>(m_eDestory)])
			Safe_Release(iter);

	GameObject::Free();
}