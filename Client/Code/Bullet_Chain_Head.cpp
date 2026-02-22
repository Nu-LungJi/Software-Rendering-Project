#include "../Include/PCH.h"

Bullet_Chain_Head::Bullet_Chain_Head(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Bullet_Chain_Head::Bullet_Chain_Head(const GameObject& _RHS)		: GameObject(_RHS)		{}
Bullet_Chain_Head::~Bullet_Chain_Head() {}

HRESULT Bullet_Chain_Head::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	m_tInfo.fSpeed = 2.f;
	fDis = 0.f;
	return S_OK;
}
INT	Bullet_Chain_Head::Update_GameObject(const _float& _DT)
{

	if (FAILED(Monster::Set_TextureList(L"Spr_Bullet_ChainHead", &m_tInfo)))
	{
		ObjectDead = true;
		return 0;
	}

	MYPOS->y = 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, 1.f, MYSCALE->y * 0.5f);


	m_tInfo.fTimer[0] += _DT;
	if (m_tInfo.fTimer[0] >= 2.f)
	{
		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::BULLET_STANDARD_DEATH, *MYPOS, FALSE, 1.2f);

		_vec3 vEffectScale = { MYSCALE->x, MYSCALE->x, MYSCALE->x };
		*static_cast<Transform*>(pEffect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = vEffectScale;
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);

		ObjectDead = true;
		return 0;
	}

	GameObject::Update_GameObject(_DT);

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

VOID Bullet_Chain_Head::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);

	fDis += m_tInfo.fSpeed * _DT;
	if (fDis >= MYSCALE->x)
	{
		m_tInfo.bTrigger[0] = !m_tInfo.bTrigger[0];
		fDis -= MYSCALE->x;

		m_tInfo.pGameObj[1] = Monster::Create<Bullet_Chain>(GRPDEV, *MYPOS);
		m_tInfo.pGameObj[1]->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		
		*dynamic_cast<Transform*>(m_tInfo.pGameObj[1]->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = *MYSCALE;
		
		MONBULLETINFO* pBulletinfo = static_cast<Bullet_Chain*>(m_tInfo.pGameObj[1])->Get_Info();
		pBulletinfo->vDirection = m_tInfo.vDirection;
		pBulletinfo->bTrigger[0] = m_tInfo.bTrigger[0];
		pBulletinfo->pGameObj[0] = m_tInfo.pGameObj[0];

		Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[1], GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);

		m_tInfo.pGameObj[1] = nullptr;
	}

	m_tInfo.Textureinfo._frameTick += _DT;

	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
	}
	m_tInfo.vDirection.y = 0.f;
	Monster::BillBoard(Component_Transform, GRPDEV, m_tInfo.vDirection, false);
}
VOID Bullet_Chain_Head::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	
	GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT Bullet_Chain_Head::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(0.11f, 0.1f, 1.f);
	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}

Bullet_Chain_Head* Bullet_Chain_Head::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Bullet_Chain_Head* MST = new Bullet_Chain_Head(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create Bullet_Chain_Head.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL Bullet_Chain_Head::OnCollisionEnter(GameObject* _Other)
{
	MainUI* mainUI;
	switch (_Other->Get_ObjectType())
	{
	default:
		break;
	case GAMEOBJECT_TYPE::OBJECT_PLAYER:
		mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
		mainUI->Player_LostHP();
	case GAMEOBJECT_TYPE::OBJECT_TERRAIN:

		break;
	}
	return true;
}
VOID Bullet_Chain_Head::Free()
{
	GameObject::Free();
}