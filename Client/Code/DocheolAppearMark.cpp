#include "../Include/PCH.h"

DocheolAppearMark::DocheolAppearMark(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
DocheolAppearMark::DocheolAppearMark(const GameObject& _RHS) : GameObject(_RHS) {}
DocheolAppearMark::~DocheolAppearMark() {}

HRESULT DocheolAppearMark::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.Change_State(MONSTER_STATE_SUMMON);

	return S_OK;
}
INT	DocheolAppearMark::Update_GameObject(const _float& _DT)
{
	// <플레이어 업데이트 시점>
	GameObject::Update_GameObject(_DT);

	m_tInfo.fTimer[0] += _DT;

	if (FAILED(Monster::Set_TextureList(L"Spr_Effect_DocheolAppearMarkFire", &m_tInfo)))
	{
		ObjectDead = true;
		return 0;
	}

	if (m_tInfo.fTimer[0] >= DOCHEOLAPPEARMARK_FLOATTIME)
	{
		m_tInfo.Change_State(MONSTER_STATE_APPEAR);
	}

	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_SUMMON:
		m_tInfo.fSpeed = 0.1f;
		m_tInfo.vDirection = { 0.f,1.f,0.f };
		break;
	case MONSTER_STATE_APPEAR:
		m_tInfo.fSpeed = 0.f;
		m_tInfo.Textureinfo._frameTick += _DT;
		if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
		{
			m_tInfo.Textureinfo._frameTick = 0.f;
			++m_tInfo.Textureinfo._frame;
		}
		if (m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe)
		{
			m_tInfo.Change_State(MONSTER_STATE_DEAD);
		}
		break;
	case MONSTER_STATE_DEAD:
		ObjectDead = true;
		return 0;
	}

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}
VOID DocheolAppearMark::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);


	Monster::BillBoard(Component_Transform, GRPDEV);
}
VOID DocheolAppearMark::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	switch (m_tInfo.eState[0])
	{
	default:
		GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
		Component_Buffer->Render_Buffer();
		break;
	case MONSTER_STATE_DEAD:
		break;
	}

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT DocheolAppearMark::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(10.f, 0.5f, 0.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(0.513f, 0.416f, 1.f);

	return S_OK;
}
DocheolAppearMark* DocheolAppearMark::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	DocheolAppearMark* MST = new DocheolAppearMark(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create DocheolAppearMark.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}

VOID DocheolAppearMark::Free() {

	GameObject::Free();
}
