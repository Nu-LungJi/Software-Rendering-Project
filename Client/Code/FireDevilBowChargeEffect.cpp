#include "../Include/PCH.h"
#include"FireDevilBowChargeEffect.h"

FireDevilBowChargeEffect::FireDevilBowChargeEffect(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
FireDevilBowChargeEffect::FireDevilBowChargeEffect(const GameObject& _RHS) : GameObject(_RHS) {}
FireDevilBowChargeEffect::~FireDevilBowChargeEffect() {}

HRESULT FireDevilBowChargeEffect::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	ObjectTAG = L"MonsterBullet";
	ObjectTYPE = GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET;

	m_tInfo.Change_State(MONSTER_STATE_SUMMON);
	m_tInfo.fSpeed = 2.f;
	Component_Collider->Set_Hp(1.f);
	Component_Collider->Set_Att(1.f);

	_vec3 vGravity = Monster::Get_Gravity();
	if (vGravity.y == -1.f) {
		m_tInfo.vDirection = { 1.0001f,0.f,0.f };
	}
	else if (vGravity.z == 1.f) {
		m_tInfo.vDirection = { 0.f,1.f,0.f };
	}
	else if (vGravity.y == 1.f) {
		m_tInfo.vDirection = { 0.f,0.f,1.f };
	}

	return S_OK;
}
INT	FireDevilBowChargeEffect::Update_GameObject(const _float& _DT) {


	m_tInfo.fTimer[0] += _DT;

	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_MINIGAME_IDLE:
		m_tInfo.fSpeed = 0.f;
		m_tInfo.Textureinfo._frameTick += _DT;
		if (m_tInfo.Textureinfo._frameTick > 0.15f) {
			if (++m_tInfo.Textureinfo._frame > m_tInfo.Textureinfo._Endframe) {
				m_tInfo.Change_State(MONSTER_STATE_MINIGAME_MOVE);
				m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Effect,
													(uint8_t)BULLET_TYPE::FireDevilBowChargeEffect,
													(uint8_t)FIREDEVILBOWCHARGEEFFECT::Charge);
				if (FAILED(Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo)))
					ObjectDead = true;
			}
		}
		break;
	case MONSTER_STATE_MINIGAME_MOVE:
		m_tInfo.fSpeed = 3.f;

		m_tInfo.Textureinfo._frameTick += _DT;
		if (m_tInfo.Textureinfo._frameTick > 0.25f) {
			++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe + 1;
		}
		break;
	case MONSTER_STATE_DEAD:
		m_tInfo.fSpeed = 0.f;
	}

	return 0;
}
VOID FireDevilBowChargeEffect::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);

	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);
	AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV, m_tInfo.vDirection, false);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
}
VOID FireDevilBowChargeEffect::Render_GameObject() {


	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTexture(0, (*m_tInfo.Textureinfo.pTexture)[m_tInfo.Textureinfo._frame]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT FireDevilBowChargeEffect::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(1.f, 0.5f, 1.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(2.74f, 10.f, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	Component_Collider->Set_Scale(2.74f * 0.5f, 2.74f* 0.f, 2.74f * 0.5f);

	m_tInfo.ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Effect,
		(uint8_t)BULLET_TYPE::FireDevilBowChargeEffect, (uint8_t)FIREDEVILBOWCHARGEEFFECT::Birth);

	return Monster::Set_TextureList(m_tInfo.ID, &m_tInfo.Textureinfo);
}
BOOL FireDevilBowChargeEffect::OnCollisionEnter(GameObject* _Other)
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
BOOL FireDevilBowChargeEffect::OnCollisionStay(GameObject* _Other)
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
BOOL FireDevilBowChargeEffect::OnCollisionExit(GameObject* _Other)
{
	return 0;
}
//FireDevilBowChargeEffect* FireDevilBowChargeEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
//	FireDevilBowChargeEffect* NPN = new FireDevilBowChargeEffect(_GRPDEV);
//	if (FAILED(NPN->Ready_GameObject())) {
//		MSG_BOX("Cannot Create FireDevilBowChargeEffect.");
//		Safe_Release(NPN);
//		return nullptr;
//	}
//	return NPN;
//}
VOID FireDevilBowChargeEffect::Free() {
	GameObject::Free();
}