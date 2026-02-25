#include "../Include/PCH.h"

EvilFrog::EvilFrog(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
EvilFrog::EvilFrog(const GameObject& _RHS) : GameObject(_RHS) {}
EvilFrog::~EvilFrog() {}

HRESULT EvilFrog::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	m_tInfo.eState[0] = MONSTER_STATE_APPEAR;

	Component_Collider->Set_Hp(EVILFROG_HP);

	m_tInfo.vDirection = { -1.f,0.f,0.f };

	return S_OK;
}
INT EvilFrog::Update_GameObject(const FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);

	MYPOS->y = MYSCALE->y * 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x * 0.5f, MYSCALE->y, MYSCALE->x * 0.5f);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.Change_State(MONSTER_STATE_DISAPPEAR);

	GameObject::Update_GameObject(_DT);

	if (Component_Collider->Get_Hp() <= 0.f)
		m_tInfo.eState[0] = MONSTER_STATE_DISAPPEAR;

	switch (m_tInfo.eState[0])
	{
	default:
		break;
	case MONSTER_STATE_APPEAR:
		EvilFrog::State_Appear(_DT);
		break;
	case MONSTER_STATE_DISAPPEAR:
		break;
	case MONSTER_STATE_IDLE:
		EvilFrog::State_Idle(_DT);
		break;
	case MONSTER_STATE_TRACKING:
		EvilFrog::State_Tracking(_DT);
		break;
	case MONSTER_STATE_CASTING:
		EvilFrog::State_Casting(_DT);
		break;
	case MONSTER_STATE_CHANNELING:
		EvilFrog::State_Channeling(_DT);
		break;
	case MONSTER_STATE_DEAD:
		EvilFrog::State_Dead();
		break;
	}
	return 0;
}
VOID EvilFrog::LateUpdate_GameObject(const FLOAT& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
}
VOID EvilFrog::Render_GameObject() {
	GameObject::Render_GameObject();
}
HRESULT EvilFrog::Component_Initialize() {
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(EVILFROG_WIDTH, EVILFROG_HEIGHT, 1.f);
	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);

	return S_OK;
}

VOID EvilFrog::State_Appear(const _float& _DT) {


}
VOID EvilFrog::State_Idle(const _float& _DT) {

}
VOID EvilFrog::State_Tracking(const _float& _DT) {

}
VOID EvilFrog::State_Casting(const _float& _DT) {

}
VOID EvilFrog::State_Channeling(const _float& _DT) {

}
VOID EvilFrog::State_Dead() {

}


BOOL EvilFrog::OnCollisionEnter(GameObject* _Other) {
	return FALSE;
}
BOOL EvilFrog::OnCollisionStay(GameObject* _Other) {
  wstring Tag = _Other->Get_ObjectTag();

	if(Tag== L"PlayerArrow") {
		Component_Collider->Set_Hp(Component_Collider->Get_Hp() - COLLIDER(_Other)->Get_Att());
  }
	return FALSE;
}
BOOL EvilFrog::OnCollisionExit(GameObject* _Other) {
	return FALSE;
}

VOID Set_Target(CONST TCHAR* _TAG) {
	
}

EvilFrog* EvilFrog::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	EvilFrog* EF = new EvilFrog(_GRPDEV);
	if (FAILED(EF->Ready_GameObject())) {
		MessageBox(0, L"EvilFrog Create Failed", L"Error", MB_OK);
		Safe_Release(EF);
	}
	return EF;
}

VOID EvilFrog::Free() {
	GameObject::Free();
}