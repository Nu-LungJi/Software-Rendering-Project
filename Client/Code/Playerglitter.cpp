#include "../Include/PCH.h"
#include "Playerglitter.h"

Playerglitter::Playerglitter(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Playerglitter::Playerglitter(const GameObject& _RHS) : GameObject(_RHS) {}
Playerglitter::~Playerglitter() {}

HRESULT Playerglitter::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_pCurr = pSee::End;
	m_pOthe = pSee::End;
	return S_OK;
}
INT	Playerglitter::Update_GameObject(const _float& _DT) {
	if (Get_ObjectDead())
		return -1;

	GameObject::Update_GameObject(_DT);

	Component_Buffer->Update_Particle(_DT);
	
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_TILE, this);
	return 0;
}
VOID Playerglitter::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
	
	//_float m_fAngle = atan2f(vLook.y, vLook.x);
	_vec3		vPos, vPlayerLook, vOrigin, myPos, vScale, vLook(0,0,1), vLookPaticle;
	_matrix		matWorld, matScale, matPlayerWorld, RotZ, matBill;
	_float		yOffset(-0.1f), LookOffset(0.5f),Angle(0);
	Player* vPlayer = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->
		Get_GameObject(L"Player"));
	D3DXMatrixIdentity(&matWorld);
	matPlayerWorld = *dynamic_cast<Transform*>(vPlayer->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_World();
	GRPDEV->GetTransform(D3DTS_VIEW, &matBill);
	switch (vPlayer->Get_See())
	{
	case pSee::SEE_UP:
		vLook = { 0,yOffset,-1 };
		break;
	case pSee::SEE_RU:
		vLook = { -1,yOffset,-1 };
		break;
	case pSee::SEE_RIGHT:
		vLook = { -1,yOffset,0 };
		break;
	case pSee::SEE_RD:
		vLook = { -1,yOffset,1 };
		break;
	case pSee::SEE_DOWN:
		vLook = { 0,yOffset,1 };
		break;
	case pSee::SEE_LD:
		vLook = { 1,yOffset,1 };
		break;
	case pSee::SEE_LEFT:
		vLook = { 1,yOffset,0 };
		break;
	case pSee::SEE_LU:
		vLook = { 1,yOffset,-1 };
		break;

	}

	if (m_pCurr == m_pOthe)
	{
		m_vOriginLook = vLook;
		m_pCurr = vPlayer->Get_See();
	}
	else
	{
		Angle   = atan2f(vLook.y - m_vOriginLook.y, vLook.x - m_vOriginLook.x);
		Component_Buffer->Set_OiriginLook(m_vOriginLook);
		Component_Buffer->Set_OiriginPos(vPos);
		m_pOthe = m_pCurr;
	}

	m_pCurr = vPlayer->Get_See();
	m_vOriginLook = vLook;

	memcpy(vPlayerLook, matPlayerWorld.m[2], sizeof(_vec3));
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

	D3DXVec3TransformNormal(&vPlayerLook, &vLook, &matPlayerWorld);
	memcpy(vPos,		 matPlayerWorld.m[3], sizeof(_vec3));
	D3DXMatrixRotationZ(&RotZ, Angle);
	D3DXVec3TransformNormal(&vPlayerLook, &vPlayerLook, &RotZ);
	vPlayerLook *= LookOffset;

	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
	vPos += vPlayerLook;
	vPos.y = -0.5f;
	vPos.z -= 0.8f;
	Component_Buffer->Set_Look(vPlayerLook,1);
	Component_Buffer->Set_Pos(vPos);

	Component_Transform->Set_Pos(vPos);


	m_fSpeed = vPlayer->Get_Speed();
	if (m_fSpeed != 0)
		Component_Buffer->Reset();
	
}
VOID Playerglitter::Render_GameObject() {

	Component_Buffer->PreRedner_Particle();
	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	GRPDEV->SetTexture(0, StaticTexture);
	
	Component_Buffer->Render_Buffer();
	GRPDEV->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	Component_Buffer->PostRender_Particle();

}
HRESULT Playerglitter::Component_Initialize() {

	Component_Buffer = ParticleEffect::Create(GRPDEV, 16);
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Scale(1.f, 1.f, 1.f);

	IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(L"blue_particle_64.png");

	if (TEX == nullptr) return E_FAIL;
	else { TEX->AddRef();  StaticTexture = TEX; }

	return S_OK;
}
Playerglitter* Playerglitter::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Playerglitter* pPlayerglitter = new Playerglitter(_GRPDEV);
	if (FAILED(pPlayerglitter->Ready_GameObject())) {
		MSG_BOX("Cannot Create Playerglitter.");
		Safe_Release(pPlayerglitter);
		return nullptr;
	}
	return pPlayerglitter;
}
VOID Playerglitter::Free() {

	Safe_Release(Component_Buffer);
	Safe_Release(StaticTexture);
	GameObject::Free();
}