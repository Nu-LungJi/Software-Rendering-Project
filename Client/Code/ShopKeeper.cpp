#include"../Include/PCH.h"
#include "ShopKeeper.h"

ShopKeeper::ShopKeeper(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
ShopKeeper::ShopKeeper(const GameObject& _RHS) : GameObject(_RHS) {}
ShopKeeper::~ShopKeeper() {}

HRESULT ShopKeeper::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
  Timer_Shop = 0.f;
	Shop_AnimIDX = 1;

	Interaction_Possible = FALSE;

	CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->
		Get_GameObject(L"Camera"));

	_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
	_vec3 planeDir = { 0.f, 1.f, 0.f };

	_float angle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&cameraDir, &cameraDir), D3DXVec3Normalize(&planeDir, &planeDir)));
	_float _cameraAngle = angle / D3DX_PI * 180.f;

	Component_Transform->Rotation(ROT_X, 90.f - _cameraAngle);
	return S_OK;
}
INT	ShopKeeper::Update_GameObject(const _float& _DT) {
  if (ObjectDead)
		return -1;
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	TalkWithShopKeeper(_DT);
	dynamic_cast<SpeechBubble*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"NPC_Shop"))->Set_SpeechPos(Component_Transform->Get_Position());

	return 0;
}
VOID ShopKeeper::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
	if (Timer_Shop > 0.2f) {
		Shop_AnimIDX = Shop_AnimIDX % 8 + 1;
		Timer_Shop = 0.f;
	}
}
VOID ShopKeeper::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	//GRPDEV->SetTexture(0, Shop_TextureList[Shop_AnimIDX]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT ShopKeeper::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(10.f, 0.112f, 10.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(0.5f, 0.5f, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(0.5f, 1.f, 0.5f);

	Component_Texture = ADD_COMPONENT_TEXTURE;
	Component_Collider = ADD_COMPONENT_COLLIDER;

	ResourceManager::GetInstance()->PathFinder(GRPDEV,L"../../UI/NPCDialog");

	for (INT PIC = 1; PIC <= 8; ++PIC) {
		wstring Base = L"SupplyCrew" + to_wstring(PIC) + L".png";
		Shop_TextureList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}

	return S_OK;
}
ShopKeeper* ShopKeeper::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	ShopKeeper* NPN = new ShopKeeper(_GRPDEV);
	if (FAILED(NPN->Ready_GameObject())) {
		MSG_BOX("Cannot Create ShopKeeper.");
		Safe_Release(NPN);
		return nullptr;
	}
	return NPN;
}
VOID ShopKeeper::Free() {
	GameObject::Free();
}

BOOL ShopKeeper::OnCollisionEnter(GameObject* _Other) {
	PlayerUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"));
	NPCTalkUI = dynamic_cast<NPCTalk*>(SceneManager::GetInstance()->Get_GameObject(L"NPCTalk"));
	Speech_BubbleUI = dynamic_cast<SpeechBubble*>(SceneManager::GetInstance()->Get_GameObject(L"Speech_Bubble"));

	if (_Other->Get_ObjectTag() == L"Player") {

		PlayerUI->PopUp_Interaction_Notice(L"대화하기", TRUE);
		Speech_BubbleUI->Set_Active(TRUE);
		return TRUE;
	}
	return FALSE;
}
BOOL ShopKeeper::OnCollisionStay(GameObject* _Other) {
	if (KEY_DOWN(DIK_E))
		Interaction_Possible = TRUE;

	return FALSE;
}
BOOL ShopKeeper::OnCollisionExit(GameObject* _Other) {
	if (_Other->Get_ObjectTag() == L"Player") {
		PlayerUI->PopUp_Interaction_Notice(L"", FALSE);
		Speech_BubbleUI->Set_Active(FALSE);
		return TRUE;
	}
	return FALSE;
}

VOID ShopKeeper::TalkWithShopKeeper(FLOAT _DT) {
	if (Interaction_Possible) {		
		if (ObjectTAG == L"NPC_Shop")
			if (NPCTalkUI->Activate_NPCTalk(NPC_CHARACTER::NPC_SHOP, _DT) == TRUE) {
				Interaction_Possible = FALSE;
			}
	}
}
