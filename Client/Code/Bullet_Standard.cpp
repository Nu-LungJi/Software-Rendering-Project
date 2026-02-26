#include "../Include/PCH.h"
#include"Bullet_Standard.h"

Bullet_Standard::Bullet_Standard(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Bullet_Standard::Bullet_Standard(const GameObject& _RHS) : GameObject(_RHS) {}
Bullet_Standard::~Bullet_Standard() {}

HRESULT Bullet_Standard::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	return S_OK;
}
INT	Bullet_Standard::Update_GameObject(const _float& _DT) {
	//if (m_tInfo.eState[0] == MONSTER_STATE_MINIGAME_IDLE) {
	//	ObjectDead = false;
	//	return 0;
	//}
	//else if (m_tInfo.eState[0] == MONSTER_STATE_MINIGAME_MOVE) {
	//	ObjectDead = false;
	//	return 0;
	//}
	//else
	//{
	//	MYPOS->y = MYSCALE->y * 0.5f;
	//}
	
	Monster::Destory_Tile(this);	
	//GameObject::Update_GameObject(_DT);
	Component_Buffer->Update_Component(_DT);
	Component_Collider->Update_Component(_DT);
	
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->y, MYSCALE->x * 0.5f);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.fTimer[0] = 5.f;

	if (m_tInfo.fTimer[0] <= 0.f)
	{
		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV,
			MONSTER_EFFECT::BULLET_STANDARD_BIRTH, *MYPOS, MYSCALE->x, 1.f, false);

		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);

		pEffect = MonsterEffect::Create(GRPDEV,
			MONSTER_EFFECT::BULLET_STANDARD_BIRTH, *MYPOS, MYSCALE->x, 1.2f, false);

		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);
	}

	m_tInfo.fTimer[0] += _DT;

	if (m_tInfo.fTimer[0] > 5.f)
	{
		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::BULLET_STANDARD_DEATH, *MYPOS, FALSE, 1.2f);

		_vec3 vEffectScale = { MYSCALE->x*2, MYSCALE->x*2, MYSCALE->x*2 };
		*static_cast<Transform*>(pEffect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = vEffectScale;
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);

		ObjectDead = true;
		return 0;
	}

	if (ObjectDead)
	{
		return -1;
	}

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	return 0;
}
VOID Bullet_Standard::LateUpdate_GameObject(const _float& _DT) {
	
	if (m_tInfo.fTimer[0]< 1.f)
		return;

	GameObject::LateUpdate_GameObject(_DT);

	m_tInfo.vDirection.y = 0.f;
	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);

	//if (!static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"))->IsIn_Frustum(this)) return;

	m_tInfo.Textureinfo._frameTick += _DT;
	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		if (m_tInfo.Textureinfo._Endframe > 0)
			++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
		m_tInfo.Textureinfo._frameTick = 0.f;
	}

	if (static_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->IsIn_Frustum(*MYPOS, 10.f)) {
		AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV, m_tInfo.vDirection, false);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}
}
VOID Bullet_Standard::Render_GameObject() {

	if (m_tInfo.fTimer[0] < 1.f)
		return;

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTexture(0, (*m_tInfo.Textureinfo.pTexture)[m_tInfo.Textureinfo._frame]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT Bullet_Standard::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(1.f, 0.5f, 1.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(BULLET_STANDARD_WIDTH, BULLET_STANDARD_HEIGHT, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	Component_Collider->Set_Scale(BULLET_STANDARD_WIDTH* 0.5f, BULLET_STANDARD_HEIGHT, BULLET_STANDARD_WIDTH * 0.5f);

	m_tInfo.Change_State(MONSTER_STATE_SUMMON);
	m_tInfo.fSpeed = BULLET_STANDARD_SPEED;
	Component_Collider->Set_Hp(1.f);
	Component_Collider->Set_Att(1.f);

	m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Bullet,
		(uint8_t)BULLET_TYPE::Standard, 0);

	return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}
BOOL Bullet_Standard::OnCollisionEnter(GameObject* _Other)
{
	wstring Tag = _Other->Get_ObjectTag();

	if (Tag == L"PlayerArrow") {
		
		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
		return TRUE;
	}
	return FALSE;
}
BOOL Bullet_Standard::OnCollisionStay(GameObject* _Other)
{
	return 0;
}
BOOL Bullet_Standard::OnCollisionExit(GameObject* _Other)
{
	return 0;
}
//Bullet_Standard* Bullet_Standard::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
//	Bullet_Standard* NPN = new Bullet_Standard(_GRPDEV);
//	if (FAILED(NPN->Ready_GameObject())) {
//		MSG_BOX("Cannot Create Bullet_Standard.");
//		Safe_Release(NPN);
//		return nullptr;
//	}
//	return NPN;
//}
VOID Bullet_Standard::Free() {
	GameObject::Free();
}
//
//VOID Bullet_Standard::BillBoard()
//{
//	_matrix		matBill, matWorld, matView;
//
//	matWorld = *Component_Transform->Get_World();
//	GRPDEV->GetTransform(D3DTS_VIEW, &matView);
//
//	D3DXMatrixIdentity(&matBill);
//
//	//X축
//	matBill._11 = matView._11;
//	matBill._12 = matView._12;
//	matBill._13 = matView._13;
//	//Y축
//	matBill._21 = matView._21;
//	matBill._22 = matView._22;
//	matBill._23 = matView._23;
//	//Z축
//	matBill._31 = matView._31;
//	matBill._32 = matView._32;
//	matBill._33 = matView._33;
//
//	D3DXMatrixInverse(&matBill, 0, &matBill);
//
//	// 주의 할 것
//	matWorld = matBill * matWorld;
//
//	Component_Transform->Set_World(&matWorld);
//}
