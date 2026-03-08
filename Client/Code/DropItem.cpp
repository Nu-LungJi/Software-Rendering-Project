#include "../Include/PCH.h"

DropItem::DropItem(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
DropItem::DropItem(const GameObject& _RHS) : GameObject(_RHS) {}
DropItem::~DropItem() {}

HRESULT DropItem::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	SoundManager::GetInstance()->Play_Sound_Once(L"Object/MiniGameItemDrop.wav", CHANNELID::SOUND_BGM03);

	m_pEventTrigger = static_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene())->Get_EventTrigger();

	return S_OK;
}
INT	DropItem::Update_GameObject(const _float& _DT)
{
	if (ObjectDead)
		return -1;

	if (m_iPickUpEvent == 0) {
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}
	else if (m_iPickUpEvent == 1) {
		Monster::Get_Camera()->Camera_Shaking(30, 3);
	}

	return 0;
}
VOID DropItem::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV) - 0.001f;

}
VOID DropItem::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, m_pTexture);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT DropItem::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(10.f, 0.112f, 10.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(0.5f, 0.5f, 0.5f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(0.5f, 0.5f, 0.5f);

	uint16_t ID = MonsterManager::Make_Key((uint8_t)MONSTER_SEP::Tile, 2, 0);
	m_pTexture = *MonsterManager::GetInstance()->Find_Textures(ID)->begin();

	return S_OK;
}
DropItem* DropItem::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	DropItem* MST = new DropItem(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create DropItem.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
BOOL DropItem::OnCollisionEnter(GameObject* _Other)
{
	return 0;
}
BOOL DropItem::OnCollisionStay(GameObject* _Other)
{
	if (m_iPickUpEvent == 0) {
		wstring Tag = _Other->Get_ObjectTag();
		if (Tag == L"Player") {
			if (KEY_DOWN(DIK_Z)) {
				if (nullptr != dynamic_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene())) {
					m_iPickUpEvent = 1;
					*m_pEventTrigger = -1;	//Temp;
					return true;
				}
			}
		}
	}
	return 0;
}
BOOL DropItem::OnCollisionExit(GameObject* _Other)
{
	return 0;
}
VOID DropItem::Free() {

	GameObject::Free();
}

