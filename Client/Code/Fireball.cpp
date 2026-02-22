#include "../Include/PCH.h"

Fireball::Fireball(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Fireball::Fireball(const GameObject& _RHS) : GameObject(_RHS) {}
Fireball::~Fireball() {}

HRESULT Fireball::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	m_tInfo.fTimer[1] = FIREBALL_SPEED;
	return S_OK;
}
INT	Fireball::Update_GameObject(const _float& _DT)
{
	GameObject::Update_GameObject(_DT);

	if (FAILED(Monster::Set_TextureList(L"Fireball", &m_tInfo)))
	{
		ObjectDead = true;
		return 0;
	}

	if(m_tInfo.bTrigger[0])
	{
		_vec3 vPos = *MYPOS;
		vPos += m_tInfo.vDirection * MYSCALE->x * 0.5f;

		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::BULLET_STANDARD_DEATH, vPos, MYSCALE->x * 0.5f, 1.2f, false, m_tInfo.vDirection);

		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);

		m_tInfo.bTrigger[0] = false;
		ObjectDead = true;
		return 0;
	}
	
	MYPOS->y = MYSCALE->y * 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->y * 0.5f);

	m_tInfo.Textureinfo._frameTick += _DT;
	if (m_tInfo.Textureinfo._frameTick >= FRAMETICK)
		++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;

	if (ObjectDead)
		return -1;

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	return 0;
}
VOID Fireball::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	//m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);

AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV, m_tInfo.vDirection, false);

}
VOID Fireball::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT Fireball::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(FIREBALL_WIDTH, FIREBALL_HEIGHT, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	return S_OK;
}

Fireball* Fireball::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Fireball* MST = new Fireball(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create Fireball.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL Fireball::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();

	if (Tag == L"PlayerArrow") {
		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
		return TRUE;
	}
	else if (Tag == L"Player")
	{
		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
		return TRUE;
	}
	return FALSE;
}
VOID Fireball::Free()
{
	GameObject::Free();
}