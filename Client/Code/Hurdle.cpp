#include "../Include/PCH.h"
#include "Hurdle.h"

Hurdle::Hurdle(LPDIRECT3DDEVICE9 _GRPDEV): GameObject(_GRPDEV) {}
Hurdle::Hurdle(const GameObject& _RHS):GameObject(_RHS){}
Hurdle::~Hurdle() {}

Hurdle* Hurdle::Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 _vSrc, _vec3 _vDst, _float _fSpeed, _float _fScale, uint8_t _Type)
{
	Hurdle* pHurdle = DBG_NEWW Hurdle(_GRPDEV);
	if (FAILED(pHurdle->Ready_Hurdle(_vSrc, _vDst, _fSpeed, _fScale, _Type))) {
		MSG_BOX("Cannot Create Hurdle\nFrom\t: %.2f, %.2f, %.2f\nTo\t: %.2f, %.2f, %.2f",
			_vSrc.x, _vSrc.y, _vSrc.z, _vDst.x, _vDst.y, _vDst.z);
		Safe_Release(pHurdle);
		return nullptr;
	}
	return pHurdle;
}

HRESULT Hurdle::Ready_Hurdle(_vec3 __vSrc, _vec3 __vDst, _float __fSpeed, _float __fScale, uint8_t __Type)
{
	uint16_t Count = 0;

	m_vDir = __vDst - __vSrc;

	if (m_vDir.x != 0.f)	++Count;
	if (m_vDir.y != 0.f)	++Count;
	if (m_vDir.z != 0.f)	++Count;
	if (Count > 1)			return E_FAIL;

	m_fDis = D3DXVec3Length(&m_vDir);

	if (__fScale == 0 || m_fDis < fabsf(__fSpeed) || m_fDis < __fScale)
		return E_FAIL;

	Count = (uint16_t)fabsf(m_fDis / __fSpeed);

	if (Count > 0xff)
		return E_FAIL;

	m_vSrc		= __vSrc;
	m_vDst		= __vDst;
	m_fSpeed	= __fSpeed;
	m_fScale	= __fScale;
	m_ubType	= __Type;

	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Pos(m_vDir * 0.5f);

	Component_Collider	= ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(m_vDir.x + m_fScale, m_vDir.y + m_fScale, m_vDir.z + m_fScale);

	D3DXVec3Normalize(&m_vDir, &m_vDir);

	HRESULT		Result = S_OK;

	for (uint8_t i = 0; i< Count; ++i)
	{
		uint8_t		ubType = __Type;
		HURDLECOM*	pHurdle = DBG_NEWW HURDLECOM;
		
		pHurdle->Component_Buffer		= static_cast<Buffer*>(ProtoManager::GetInstance()->Clone_Prototype(COMPONENT_TYPE::COMPONENT_RECTTEX));
		pHurdle->Component_Transform	= static_cast<Transform*>(ProtoManager::GetInstance()->Clone_Prototype(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		pHurdle->Component_Collider		= static_cast<Collider*>(ProtoManager::GetInstance()->Clone_Prototype(COMPONENT_TYPE::COMPONENT_COLLIDER));
		
		if (ubType ==	(uint8_t)eHurdleType::Random)
			ubType =	(uint8_t)RANDOM::Get_int((uint8_t)eHurdleType::Bat, (uint8_t)((uint8_t)eHurdleType::Random - 1));

		pHurdle->ID = (uint16_t)ubType << 8 | (uint16_t)i;

		_vec3 vScale = { 0.f,0.f,0.f };

		switch (ubType)
		{
		case (uint8_t)eHurdleType::Bat:
			Result = Monster::Set_TextureList(L"Spr_Monster_BlueEvilBat", &pHurdle->Textures);
			pHurdle->Textures._Endframe = 6;
			vScale.x = BATIMGX; vScale.y = BATIMGY;
			D3DXVec3Normalize(&vScale, &vScale);
			vScale.z = 1.f;
			break;
		case (uint8_t)eHurdleType::Scorpion:
			Result = Monster::Set_TextureList(L"Spr_Monster_BlueScorpionEvilSoul_Stand", &pHurdle->Textures);
			pHurdle->Textures._Endframe = 8;
			vScale.x = SCORPIONEVILSOULIMGX; vScale.y = SCORPIONEVILSOULIMGY;
			D3DXVec3Normalize(&vScale, &vScale);
			vScale.z = 1.f;
			break;
		case (uint8_t)eHurdleType::Shotgun:
			Result = Monster::Set_TextureList(L"Spr_Monster_BlueEliteShotGunEvilSoul_Stand", &pHurdle->Textures);
			pHurdle->Textures._Endframe = 8;
			vScale.x = SHOTGUNEVILSOULIMGX; vScale.y = SHOTGUNEVILSOULIMGY;
			D3DXVec3Normalize(&vScale, &vScale);
			vScale.z = 1.f;
			break;
		case (uint8_t)eHurdleType::Slime:;
			Result = Monster::Set_TextureList(L"Spr_Monster_BlueEvilSlime_Move", &pHurdle->Textures);
			vScale.x = EVILSLIMEIMGX; vScale.y = EVILSLIMEIMGY;
			D3DXVec3Normalize(&vScale, &vScale);
			vScale.z = 1.f;
			break;
		default:
			return E_FAIL;
		}

		if (FAILED(Result)) return Result;

		pHurdle->Component_Transform->Set_Pos(__vSrc + m_vDir * m_fSpeed);
		pHurdle->Component_Transform->Set_Scale(vScale * m_fScale);
		pHurdle->Component_Transform->Set_Rotation(0.f, 0.f, 0.f);

		pHurdle->Component_Collider->Set_CenterPos(pHurdle->Component_Transform);
		pHurdle->Component_Collider->Set_Scale(vScale.x * 0.5f, vScale.y * 0.5f, vScale.x * 0.5f);

		m_vecComponent.push_back(pHurdle);
	}
	return S_OK;
}

INT Hurdle::Update_GameObject(const _float& _DT)
{
	GameObject::Update_GameObject(_DT);

	for (auto iter : m_vecComponent) {
		iter->Component_Buffer->Update_Component(_DT);
		iter->Component_Transform->Update_Component(_DT);
		iter->Component_Collider->Update_Component(_DT);
	}

	return 0;
}

VOID Hurdle::LateUpdate_GameObject(const _float& _DT)
{
	GameObject::LateUpdate_GameObject(_DT);

	for (auto iter : m_vecComponent)	{
		iter->Component_Transform->Move_Pos(&m_vDir, m_fSpeed, _DT);

		iter->Textures._frameTick += _DT;
		if (iter->Textures._frameTick > FRAMETICK)
			++iter->Textures._frame %= iter->Textures._Endframe;

		iter->Component_Buffer->LateUpdate_Component(_DT);
		iter->Component_Transform->LateUpdate_Component(_DT);
		iter->Component_Collider->LateUpdate_Component(_DT);
	}
}


VOID Hurdle::Render_GameObject()
{
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (auto iter : m_vecComponent) {
		GRPDEV->SetTransform(D3DTS_WORLD, iter->Component_Transform->Get_World());
		GRPDEV->SetTexture(0, iter->Textures._vecTexture[iter->Textures._frame]);
		iter->Component_Buffer->Render_Buffer();
	}
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

BOOL Hurdle::OnCollisionEnter(GameObject* _Other)
{
	wstring	Tag = _Other->Get_ObjectTag();

	if (Tag != L"Player") return false;

	Collider* pPlayer = COLLIDER(_Other);
	for (auto iter : m_vecComponent) {
		if ((iter->Component_Collider->Get_MaxPoint().x >= pPlayer->Get_MinPoint().x) && (pPlayer->Get_MaxPoint().x >= iter->Component_Collider->Get_MinPoint().x) &&
			(iter->Component_Collider->Get_MaxPoint().y >= pPlayer->Get_MinPoint().y) && (pPlayer->Get_MaxPoint().y >= iter->Component_Collider->Get_MinPoint().y) &&
			(iter->Component_Collider->Get_MaxPoint().z >= pPlayer->Get_MinPoint().z) && (pPlayer->Get_MaxPoint().z >= iter->Component_Collider->Get_MinPoint().z)) 
		{
			// юс╫ц
			POS(m_pPlayer)->z -= m_fScale;
		}
	}

	return true;
}

VOID Hurdle::Free() {
	for (auto& iter : m_vecComponent)	{
		Safe_Delete(iter);
	}
	m_vecComponent.clear();
}