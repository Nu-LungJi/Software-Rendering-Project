#include "../Include/PCH.h"

Frog_Bullet::Frog_Bullet(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Frog_Bullet::Frog_Bullet(const GameObject& _RHS) : GameObject(_RHS) {}
Frog_Bullet::~Frog_Bullet() {}

HRESULT Frog_Bullet::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	return S_OK;
}
INT	Frog_Bullet::Update_GameObject(const _float& _DT)
{
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->y * 0.5f);

	if (m_tInfo.bTrigger[0])
	{
		m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Bullet, (uint8_t)BULLET_TYPE::Chain01, 0);

		if (FAILED(Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo))) ObjectDead = true;
	}
	else
	{
		m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Bullet, (uint8_t)BULLET_TYPE::Chain02, 0);

		if (FAILED(Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo))) ObjectDead = true;
	}

	m_tInfo.fTimer[0] += _DT;
	if (m_tInfo.fTimer[0] >= 2.f)
	{
		Component_Collider->Set_Hp(-1.f);
	}

	if (Component_Collider->Get_Hp() < 0.f)
	{
		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::BULLET_STANDARD_DEATH, *MYPOS, FALSE, 1.2f);

		_vec3 vEffectScale = { MYSCALE->x, MYSCALE->x, MYSCALE->x };
		*static_cast<Transform*>(pEffect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = vEffectScale;
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);

		ObjectDead = true;
	}

	//GameObject::Update_GameObject(_DT);
	Component_Buffer->Update_Component(_DT);
	Component_Collider->Update_Component(_DT);

	if (ObjectDead)
		return -1;

	return 0;
}

VOID Frog_Bullet::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);

	m_tInfo.Textureinfo._frameTick += _DT;

	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		if (m_tInfo.Textureinfo._Endframe > 0)
			++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
	}

	if (static_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->IsIn_Frustum(*MYPOS, MYSCALE->x)) {
		AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV, m_tInfo.vDirection, false);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}

}
VOID Frog_Bullet::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, (*m_tInfo.Textureinfo.pTexture)[m_tInfo.Textureinfo._frame]);
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT Frog_Bullet::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(EVILFROG_BULLET_WIDTH, EVILFROG_BULLET_HEIGHT, 1.f);
	Component_Transform->Set_Pos(0.f, EVILFROG_BULLET_HEIGHT * 0.5f, 0.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(EVILFROG_BULLET_WIDTH * 0.5f, EVILFROG_BULLET_HEIGHT, EVILFROG_BULLET_WIDTH * 0.f);
	Component_Collider->Set_Hp(1.f);
	Component_Collider->Set_Att(1.f);

	m_tInfo.fSpeed = EVILFROG_BULLET_SPEED;

	m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Bullet, (uint8_t)BULLET_TYPE::Chain01, 0);

	return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}

Frog_Bullet* Frog_Bullet::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Frog_Bullet* FB = new Frog_Bullet(_GRPDEV);
	if (FAILED(FB->Ready_GameObject())) {
		MSG_BOX("Cannot Create Frog_Bullet.");
		Safe_Release(FB);
		return nullptr;
	}
	return FB;
}
BOOL Frog_Bullet::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();

	return FALSE;
}
BOOL Frog_Bullet::OnCollisionStay(GameObject* _Other)
{
	return 0;
}
BOOL Frog_Bullet::OnCollisionExit(GameObject* _Other)
{
	return 0;
}
VOID Frog_Bullet::Free()
{
	GameObject::Free();
}