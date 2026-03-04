#include "../Include/PCH.h"

Bullet_Chain::Bullet_Chain(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Bullet_Chain::Bullet_Chain(const GameObject& _RHS)		: GameObject(_RHS)		{}
Bullet_Chain::~Bullet_Chain() {}

HRESULT Bullet_Chain::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	return S_OK;
}
INT	Bullet_Chain::Update_GameObject(const _float& _DT)
{

	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->x * 0.5f);

	if (m_tInfo.bTrigger[0])
	{
		m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Bullet, (uint8_t)BULLET_TYPE::Chain, (uint8_t)BULLET_CHAIN_TYPE::Rect);


		if(FAILED( Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo))) ObjectDead = true;
	}
	else
	{
		m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Bullet, (uint8_t)BULLET_TYPE::Chain, (uint8_t)BULLET_CHAIN_TYPE::Line);


		if (FAILED(Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo))) ObjectDead = true;
	}

	m_tInfo.fTimer[0] += _DT;


	//Kill Timer
	if (m_tInfo.fTimer[0] >= 3.f)
	{
		Component_Collider->Set_Hp(-1.f);
	}

	if (Component_Collider->Get_Hp() <= 0.f)
	{
		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::BULLET_STANDARD_DEATH, *MYPOS, FALSE, 1.2f);
		//SoundManager::GetInstance()->Play_Sound_Once(L"Monster/ChainAttack.wav", CHANNELID::SOUND_EFFECT08, 0.1f);
		//SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_EFFECT08, 0.01f);

		_vec3 vEffectScale = { MYSCALE->x, MYSCALE->x, MYSCALE->x };
		*static_cast<Transform*>(pEffect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = vEffectScale;
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);
		SoundManager::GetInstance()->Play_Sound(L"Monster/Scorpion_Chain.mp3", CHANNELID::SOUND_EFFECT08, 0.3f);

		ObjectDead = true;
	}

	Component_Collider->Update_Component(_DT);

	if (ObjectDead)
		return -1;

	return 0;
}

VOID Bullet_Chain::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);

	m_tInfo.Textureinfo._frameTick += _DT;

	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		if(m_tInfo.Textureinfo._Endframe > 0)
			++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
	}

	if (static_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->IsIn_Frustum(*MYPOS, MYSCALE->x)) {
		AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV, m_tInfo.vDirection, false);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}

}
VOID Bullet_Chain::Render_GameObject() {

	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, (*m_tInfo.Textureinfo.pTexture)[m_tInfo.Textureinfo._frame]);
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT Bullet_Chain::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(BULLET_CHAIN_WIDTH, BULLET_CHAIN_HEIGHT, 1.f);
	Component_Transform->Set_Pos(0.f, BULLET_CHAIN_HEIGHT * 0.5f, 0.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(BULLET_CHAIN_WIDTH * 0.5f, BULLET_CHAIN_HEIGHT, BULLET_CHAIN_WIDTH * 0.f);
	Component_Collider->Set_Hp(1.f);
	Component_Collider->Set_Att(1.f);

	m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Bullet, (uint8_t)BULLET_TYPE::Chain, (uint8_t)BULLET_CHAIN_TYPE::Head);

	return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}

Bullet_Chain* Bullet_Chain::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Bullet_Chain* MST = new Bullet_Chain(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create Bullet_Chain.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL Bullet_Chain::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();
	if (Tag == L"PlayerArrow") {

		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
		return TRUE;
	}
	else if (Tag == L"Player") {
		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - 1.f);
		return true;
	}
	return FALSE;
}
BOOL Bullet_Chain::OnCollisionStay(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();
	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_MINIGAME_IDLE:
	case MONSTER_STATE_MINIGAME_MOVE:
		if (Tag == L"Player")
			return	Monster::Hurdle_CollisionStay(this, _Other);
	}
	return FALSE;
}
BOOL Bullet_Chain::OnCollisionExit(GameObject* _Other)
{
	return 0;
}
VOID Bullet_Chain::Free()
{
	GameObject::Free();
}