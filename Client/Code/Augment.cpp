#include "../Include/PCH.h"
#include "Augment.h"

Augment::Augment(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Augment::Augment(CONST GameObject& _RHS)		: GameObject(_RHS)		{}
Augment::~Augment()													{}

HRESULT	Augment::Ready_GameObject() {
	if (FAILED(Component_Initialize()))		return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;

	return S_OK;
}
INT		Augment::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	if (KEY_DOWN(DIK_LCONTROL) && KEY_DOWN(DIK_A))
	{
		isActive = !isActive;
		if (!isActive)
		{
			UIManager::GetInstance()->Find_FontObject(L"PERK_TITLE")->Visible = FALSE;
			UIManager::GetInstance()->Find_FontObject(L"PERK_INFO")->Visible = FALSE;
		}
		if (isActive)
		{

		}
	}
	return 0;
}
VOID	Augment::LateUpdate_GameObject(CONST FLOAT& _DT) {

}
VOID	Augment::Render_GameObject() {
	Component_Sprite->Render_Sprite();
}

HRESULT Augment::Component_Initialize() {
	Component_Sprite		= ADD_COMPONENT_SPRITE;
	Component_Texture	  = ADD_COMPONENT_TEXTURE;
	Component_Collider  = ADD_COMPONENT_COLLIDER;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	return S_OK;
}
HRESULT Augment::Sprite_Initialize() {
	///////////////////////////////APOSTLE_BACKGROUND///////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_Apostle_BackGroun.png", L"Apostle_BackGround", 0.f, 0.f, 1280, 960, TRUE, 155);
	///////////////////////////////APOSTLE_EFFECT///////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Object_LevelUpBless_Effect.png", L"Apostle_Light", 540.f, 30.f, 180, 180, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevepUpPopupWindow.png", L"Apostle", 540.f, 30.f, 170, 190, TRUE, 255);
	//////////////////////////////////////PERK//////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_PerkIcon_1-04.png", L"Perk_01", 340.f, 270.f, 80.f, 80.f, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_PerkIcon_1-05.png", L"Perk_02", 590.f, 260.f, 108.f, 108.f, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_PerkIcon_1-07.png", L"Perk_03", 820.f, 260.f, 108.f, 108.f, TRUE, 255);
	/////////////////////////////////////BACKGROUND_TOP_BOTTOM//////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_ApostleLiberation_Background_Top.png", L"Y", 270.f, 400.f, 700, 80, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_ApostleLiberation_Background_Bottom.png", L"D", 270.f, 480.f, 700, 140, TRUE, 255);
	///////////////////////////////////BACKGROUND///////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Background.png", L"Perk_BackGround", 300.f, 230.f, 140, 140, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Background.png", L"Perk_BackGround", 560.f, 230.f, 140, 140, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Background.png", L"Perk_BackGround", 800.f, 230.f, 140, 140, TRUE, 255);
	///////////////////////////////////////FRAME////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Frame.png", L"Perk_Frame", 300.f, 230.f, 140, 140, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Frame.png", L"Perk_Frame", 560.f, 230.f, 140, 140, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Frame.png", L"Perk_Frame", 800.f, 230.f, 140, 140, TRUE, 255);
	
	return S_OK;
}

HRESULT Augment::Text_Initialize() {
	///////////////////////////////////////FONT//////////////////////////////////
	Perk_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 600.f }, 15, L"PERK_TITLE", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	Perk_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 650.f }, 15, L"PERK_INFO", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	
	return S_OK;
}

VOID Augment::Display_Perk_Font()
{
	if (isActive)
	{

	}
}

BOOL Augment::OnCollisionEnter(GameObject* _Other) {
	
	return FALSE;
}
BOOL Augment::OnCollisionStay(GameObject* _Other) {
	
	return FALSE;
}
BOOL Augment::OnCollisionExit(GameObject* _Other) {
	
	return FALSE;
}

Augment* Augment::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Augment* MUI = new Augment(_GRPDEV);
	if (FAILED(MUI->Ready_GameObject())) {
		MSG_BOX("Cannot Create Augments.");
		Safe_Release(MUI);
		return nullptr;
	}
	return MUI;
}
VOID  Augment::Free() {
	GameObject::Free();
}