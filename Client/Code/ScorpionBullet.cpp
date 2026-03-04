#include "../Include/PCH.h"

ScorpionBullet::ScorpionBullet(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
ScorpionBullet::ScorpionBullet(const GameObject& _RHS) :	GameObject(_RHS)	{}
ScorpionBullet::~ScorpionBullet() {}

HRESULT ScorpionBullet::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;



	return S_OK;
}
INT	ScorpionBullet::Update_GameObject(const _float& _DT)
{

	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->z * 0.5f);

	Monster::Destory_Tile(this);

	m_tInfo.fTimer[0] += _DT;

	//Kill Timer
	if (m_tInfo.fTimer[0] > 5.f)
	{
		Component_Collider->Set_Hp(-1.f);
	}

	if (Component_Collider->Get_Hp() <= 0.f)
	{
		m_tInfo.ID = MonsterManager::Update_Key(m_tInfo.ID, (uint8_t)MONSTER_ANIM::Death);
		if (FAILED(Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo))) ObjectDead = true;

		m_tInfo.fTimer[1] += _DT;
		ObjectDead = m_tInfo.fTimer[1] >= 2.f;

		if (!m_tInfo.bTrigger[1])
		{
			m_tInfo.fSpeed *= 0.3f;
			for (int i = 0; i < SCORPIONBULLET_CHAINBULLET_NUM; ++i)
			{
				m_tInfo.pGameObj[1] = Monster::Create<Bullet_Chain_Head>(GRPDEV, *MYPOS, MYSCALE->x * 0.8f);

				_float fRadian = 2.f * D3DX_PI * ((_float)i / SCORPIONBULLET_CHAINBULLET_NUM);
				fRadian = (fRadian > D3DX_PI) ? fRadian - (2.f * D3DX_PI) : fRadian;

				static_cast<Bullet_Chain_Head*>(m_tInfo.pGameObj[1])->Set_Dir(cosf(fRadian), 0.f, sinf(fRadian));

				Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], L"MonsterBullet", GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
				m_tInfo.pGameObj[1] = nullptr;
			}
			m_tInfo.bTrigger[1] = true;
			SoundManager::GetInstance()->Play_Sound_Once(L"Monster/Scorpion_Chain.wav", CHANNELID::SOUND_EFFECT08, 0.3f);
		}
	}

	m_tInfo.Textureinfo._frameTick += _DT;
	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		++m_tInfo.Textureinfo._frame;
		if (m_tInfo.bTrigger[0])
		{
			ObjectDead = m_tInfo.Textureinfo._frame >= m_tInfo.Textureinfo._Endframe;
		}
		else
		{
			m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
		}
	}

	Component_Collider->Update_Component(_DT);

	if (ObjectDead)
		return -1;

	return 0;
}
VOID ScorpionBullet::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);

	if (static_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->IsIn_Frustum(*MYPOS, MYSCALE->x)) {

		_float fRadian = m_tInfo.fTimer[0] * D3DX_PI * 6.f;
		AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV, { cosf(fRadian),0.f,sinf(fRadian) }, false);

		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}
	}
VOID ScorpionBullet::Render_GameObject() {

	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, (*m_tInfo.Textureinfo.pTexture)[m_tInfo.Textureinfo._frame]);
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT ScorpionBullet::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(SCORPIONBULLET_WIDTH, SCORPIONBULLET_HEIGHT, 1.f);
	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Hp(1.f);
	Component_Collider->Set_Att(1.f);

	m_tInfo.fSpeed = SCORPIONBULLET_SPEED;

	m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Bullet,
		(uint8_t)BULLET_TYPE::ScorpionBullet, 0);

	return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}

BOOL ScorpionBullet::OnCollisionEnter(GameObject* _Other)
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

BOOL ScorpionBullet::OnCollisionStay(GameObject* _Other)
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

BOOL ScorpionBullet::OnCollisionExit(GameObject* _Other)
{
	return 0;
}

ScorpionBullet* ScorpionBullet::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	ScorpionBullet* MST = new ScorpionBullet(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create ScorpionBullet.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
VOID ScorpionBullet::Free()
{
	GameObject::Free();
}