#include"../Include/PCH.h"
#include "ShopKeeper.h"

ShopKeeper::ShopKeeper(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV){}
ShopKeeper::ShopKeeper(const GameObject& _RHS) : GameObject(_RHS) {}
ShopKeeper::~ShopKeeper() {}

HRESULT ShopKeeper::Ready_GameObject(_vec3 vPos) {
	if (FAILED(Component_Initialize())) return E_FAIL;
	Timer_Shop = 0.f;
	Shop_AnimIDX = 1;

	CamLock = FALSE;
	Component_Transform->Set_Pos(vPos.x,0.6f,vPos.z);
	Enable_Interaction = FALSE;

	QuestState = (INT)QUESTSTATE::NOT_ACCEPTED;
	CollisionManager::GetInstance()->Add_ColliderObject(this);
	return S_OK;
}
INT	ShopKeeper::Update_GameObject(const _float& _DT) {
	if (ObjectDead)
	{
		CollisionManager::GetInstance()->Delete_ColliderObject(this);
		return -1;
	}
		
	GameObject::Update_GameObject(_DT);
	TalkWithShopKeeper(_DT);

	if (QuestState == (INT)QUESTSTATE::ACCEPTED) {
		SoundManager::GetInstance()->Play_Sound_Once(L"Object/Hit_Wood_Normal_03.wav", CHANNELID::SOUND_EFFECT03, 0.3f);
		TileManager::GetInstance()->Set_Trigger(TileManager::GetInstance()->Get_CurrentStage(), TILEMODE_CHANGE::MODE_TILE, TILE_STATE::STATE_POTALEFFECT);
		QuestState = (INT)QUESTSTATE::ONGOING;
	}
	//dynamic_cast<SpeechBubble*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"NPC_Shop"))->Set_SpeechPos(Component_Transform->Get_Position());

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}
VOID ShopKeeper::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	if (Timer_Shop > 0.2f) {
		Shop_AnimIDX = Shop_AnimIDX % 8 + 1;
		Timer_Shop = 0.f;
	}
	if (TileManager::GetInstance()->Get_Stage() == TILE_STAGE::TILE_DOCHER1)
		ObjectDead = true;

}
VOID ShopKeeper::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, Shop_TextureList[0]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GRPDEV->SetTexture(0, nullptr);
}
HRESULT ShopKeeper::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Rotation(80.f, 0.f, 0.f);
	Component_Transform->Set_Scale(0.8f , 1.5f, 1.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(1.5f, 1.5f, 1.5f);

	Component_Texture = ADD_COMPONENT_TEXTURE;

	Shop_TextureList.push_back(ResourceManager::GetInstance()->Find_Texture(L"NPCTalk_Shop.png"));

	return S_OK;
}
ShopKeeper* ShopKeeper::Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 vPos) {
	ShopKeeper* NPN = new ShopKeeper(_GRPDEV);
	if (FAILED(NPN->Ready_GameObject(vPos))) {
		MSG_BOX("Cannot Create ShopKeeper.");
		Safe_Release(NPN);
		return nullptr;
	}
	return NPN;
}
VOID		ShopKeeper::Free() {
	GameObject::Free();
}

BOOL ShopKeeper::OnCollisionEnter(GameObject* _Other) {
	//Speech_BubbleUI = dynamic_cast<SpeechBubble*>(SceneManager::GetInstance()->Get_GameObject(L"Speech_Bubble"));

	if (_Other->Get_ObjectTag() == L"Player") {
		static_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->PopUp_Interaction_Notice(L"��ȭ�ϱ� - ���� ����", TRUE);
		//Speech_BubbleUI->Set_Active(TRUE);
		return TRUE;
	}
	return FALSE;
}
BOOL ShopKeeper::OnCollisionStay(GameObject* _Other) {
	if (KEY_DOWN(DIK_E))
		Enable_Interaction = TRUE;

	return FALSE;
}
BOOL ShopKeeper::OnCollisionExit(GameObject* _Other) {
	if (_Other->Get_ObjectTag() == L"Player") {
		static_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->PopUp_Interaction_Notice(L"", FALSE);
		//Speech_BubbleUI->Set_Active(FALSE);
		return TRUE;
	}
	return FALSE;
}

VOID ShopKeeper::TalkWithShopKeeper(FLOAT _DT) {
	if (Enable_Interaction) {
		if (CamLock == FALSE) {
			static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"))->Set_SCamDest({ 22.767F, 17.05f, 112.95f });
			static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"))->Ready_SmoothCamera(TRUE);
			CamLock = TRUE;
		}
		if (ObjectTAG == L"ShopNPC") {
			if (static_cast<NPCTalk*>(SceneManager::GetInstance()->Get_GameObject(L"NPCTalk"))->Activate_NPCTalk(NPC_CHARACTER::NPC_SHOP, _DT) == TRUE) {
				Enable_Interaction = FALSE;
				CamLock = FALSE;
			}
		}
	}
}

HRESULT ShopKeeper::Make_TextureList(wstring _FileName)
{
	INT FRAME = 0;

	while (++FRAME) {
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";
		auto TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { Shop_TextureList.push_back(TEX); }
	}

	return S_OK;
}
