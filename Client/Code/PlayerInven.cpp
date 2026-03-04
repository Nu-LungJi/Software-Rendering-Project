#include "../Include/PCH.h"
#include "PlayerInven.h"

PlayerInven::PlayerInven(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), Equip_ItemList{ nullptr }, Saved_ItemList{ nullptr } {}
PlayerInven::PlayerInven(CONST GameObject& _RHS)	: GameObject(_RHS)	 , Equip_ItemList{ nullptr }, Saved_ItemList{ nullptr } {}
PlayerInven::~PlayerInven() {}

HRESULT	PlayerInven::Ready_GameObject() {
	if (FAILED(Component_Initialize()))		return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;
	if (FAILED(Item_Initialize()))			return E_FAIL;

	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));

	SavedItemIndex = 1;
	EquipedItemIndex = 1;

	FocusOn_SavedItem	= TRUE;
	FocusOn_EquipedItem = FALSE;

	EquipMode = FALSE;

	for (auto& TXT : ItemInfo_Text) {
		TXT->Visible = FALSE;
		UIManager::GetInstance()->Find_FontObject(L"Inven_QText")->Visible = FALSE;
		UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Visible = FALSE;

		EquipMode = FALSE;
		EquipObject = nullptr;
	}

	return S_OK;
}
INT		PlayerInven::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	if (KEY_DOWN(DIK_B)) {
		UIManager::GetInstance()->Get_Active() ? UIManager::GetInstance()->Set_Active(FALSE) : UIManager::GetInstance()->Set_Active(TRUE);
		UIManager::GetInstance()->Get_Active() ? PlayerObject->Set_PlayerStop(TRUE) : PlayerObject->Set_PlayerStop(FALSE);

		if (UIManager::GetInstance()->Get_Active() == TRUE) {
			// 인벤 하이라이트
			for (int i = 1; i <= 8; i++) {
				wstring Frame = L"EQP_HighLight" + to_wstring(i);
				Component_Sprite->Get_Texture(Frame)->Set_Visible(FALSE);
			}
			for (int i = 2; i <= 10; i++) {
				wstring Frame = L"INV_HighLight" + to_wstring(i);
				Component_Sprite->Get_Texture(Frame)->Set_Visible(FALSE);
			}
			Component_Sprite->Get_Texture(L"INV_HighLight1")->Set_Visible(TRUE);
		}


		if (UIManager::GetInstance()->Get_Active() == TRUE) {
			SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/Open_Inven.mp3", CHANNELID::SOUND_EFFECT03, 0.4f);
			FocusOn_SavedItem = TRUE;
			for (auto& TXT : ItemInfo_Text) {
				TXT->Visible = TRUE;
				UIManager::GetInstance()->Find_FontObject(L"Inven_QText")->Visible = TRUE;
				UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Visible = TRUE;
			}
		}
		else if (UIManager::GetInstance()->Get_Active() == FALSE){
			SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/Close_Inven.mp3", CHANNELID::SOUND_EFFECT03, 0.4f);
			FocusOn_SavedItem = FALSE;
			FocusOn_EquipedItem = FALSE;

			for (auto& TXT : ItemInfo_Text) {
				TXT->Visible = FALSE;
				UIManager::GetInstance()->Find_FontObject(L"Inven_QText")->Visible = FALSE;
				UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Visible = FALSE;

				EquipMode = FALSE;
				EquipObject = nullptr;
			}

			SavedItemIndex = 1;
			EquipedItemIndex = 1;
		}
	}
	
	if (UIManager::GetInstance()->Get_Active() == TRUE) {
		Selecting_SavedItem();
		Selecting_EquipItem();
		Display_ItemInfo();
		Equip_Item();
	}


	return 0;
}
VOID	PlayerInven::LateUpdate_GameObject(CONST FLOAT& _DT) {

}
VOID	PlayerInven::Render_GameObject() {
	if (UIManager::GetInstance()->Get_Active() == TRUE) {
		Component_Sprite->Render_Sprite();
	}
	else if (UIManager::GetInstance()->Get_Active() == FALSE) {
	}
}

HRESULT PlayerInven::Component_Initialize() {
	UIManager::GetInstance()->Set_Active(FALSE);
	Component_Sprite = ADD_COMPONENT_SPRITE;

	return S_OK;
}
HRESULT PlayerInven::Sprite_Initialize() {
	wstring BaseFolder = L"../../UI/Inventory_UI/";
	Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_BG.png", L"INV_BackGround", 0.f, 0.f, 1280, 720, TRUE, 155);
	//////////////////////////////////////////////////////////////// EQUIPMENT ///////////////////////////////////////////////////////////////
	{
		//////////////////////////////////////////////////////////// EQP - BackGround ////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SpaceBG.png", L"EQP_BackGround1", 410.f + 80.F, 285.f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SpaceBG.png", L"EQP_BackGround2", 485.f + 80.F, 230.f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SpaceBG.png", L"EQP_BackGround3", 560.f + 80.F, 230.f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SpaceBG.png", L"EQP_BackGround4", 635.f + 80.F, 285.f, 70, 70, TRUE, 200);

			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SpaceBG.png", L"EQP_BackGround5", 410.f + 80.F, 367.5f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SpaceBG.png", L"EQP_BackGround6", 485.f + 80.F, 422.5f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SpaceBG.png", L"EQP_BackGround7", 560.f + 80.F, 422.5f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SpaceBG.png", L"EQP_BackGround8", 635.f + 80.F, 367.5f, 70, 70, TRUE, 200);
		}
		////////////////////////////////////////////////////////////// EQP - Frame ///////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"EQP_NormalFrame1", 410.f + 80.F, 285.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"EQP_NormalFrame2", 485.f + 80.F, 230.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"EQP_NormalFrame3", 560.f + 80.F, 230.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"EQP_NormalFrame4", 635.f + 80.F, 285.f, 70, 70, TRUE, 255);

			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"EQP_NormalFrame5", 410.f + 80.F, 367.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"EQP_NormalFrame6", 485.f + 80.F, 422.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"EQP_NormalFrame7", 560.f + 80.F, 422.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"EQP_NormalFrame8", 635.f + 80.F, 367.5f, 70, 70, TRUE, 255);
		}
		///////////////////////////////////////////////////////////// EQP - Emblem ///////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Weapon.png", L"EQP_Emblem1", 410.f + 85.F, 290.f, 60, 60, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Weapon.png", L"EQP_Emblem2", 485.f + 85.F, 235.f, 60, 60, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Weapon.png", L"EQP_Emblem3", 560.f + 85.F, 235.f, 60, 60, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Weapon.png", L"EQP_Emblem4", 635.f + 85.F, 290.f, 60, 60, TRUE, 200);

			Component_Sprite->Import_SpriteEX(BaseFolder, L"EquipMent.png", L"EQP_Emblem5", 410.f + 85.F, 365.f, 65, 65, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"EquipMent.png", L"EQP_Emblem6", 485.f + 85.F, 420.f, 65, 65, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"EquipMent.png", L"EQP_Emblem7", 560.f + 85.F, 420.f, 65, 65, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"EquipMent.png", L"EQP_Emblem8", 635.f + 85.F, 365.f, 65, 65, TRUE, 200);
		}
		//////////////////////////////////////////////////////////// EQP - ItemSpace /////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"EQP_ItemSpace1", 410.f + 83.F, 290.f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"EQP_ItemSpace2", 485.f + 83.F, 235.f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"EQP_ItemSpace3", 560.f + 83.F, 235.f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"EQP_ItemSpace4", 635.f + 83.F, 290.f, 70, 70, TRUE, 200);

			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"EQP_ItemSpace5", 410.f + 80.F, 370.f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"EQP_ItemSpace6", 485.f + 80.F, 425.f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"EQP_ItemSpace7", 560.f + 80.F, 425.f, 70, 70, TRUE, 200);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"EQP_ItemSpace8", 635.f + 80.F, 370.f, 70, 70, TRUE, 200);
		}
		//////////////////////////////////////////////////////////// EQP - HighLight /////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"EQP_HighLight1", 410.f + 80.F, 285.f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"EQP_HighLight2", 485.f + 80.F, 230.f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"EQP_HighLight3", 560.f + 80.F, 230.f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"EQP_HighLight4", 635.f + 80.F, 285.f, 70, 70, FALSE, 255);

			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"EQP_HighLight5", 410.f + 80.F, 367.5f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"EQP_HighLight6", 485.f + 80.F, 422.5f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"EQP_HighLight7", 560.f + 80.F, 422.5f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"EQP_HighLight8", 635.f + 80.F, 367.5f, 70, 70, FALSE, 255);
		}
	}
	//////////////////////////////////////////////////////////////// INVENTORY ///////////////////////////////////////////////////////////////
	{
		//////////////////////////////////////////////////////////// INV - BackGround ////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SavedSpace_BG.png", L"INV_BackGround", 810.f, 250.f, 393, 215, TRUE, 155);
		}
		////////////////////////////////////////////////////////////// INV - Frame ///////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame1", 835.f, 292.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame2", 835.f + 67.5f, 292.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame3", 835.f + 67.5f * 2, 292.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame4", 835.f + 67.5f * 3, 292.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame5", 835.f + 67.5f * 4, 292.5f, 70, 70, TRUE, 255);

			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame6", 835.f, 360.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame7", 835.f + 67.5f, 360.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame8", 835.f + 67.5f * 2, 360.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame9", 835.f + 67.5f * 3, 360.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_NonSelectFrame.png", L"INV_NormalFrame10", 835.f + 67.5f * 4, 360.f, 70, 70, TRUE, 255);
		}
		//////////////////////////////////////////////////////////// INV - ItemSpace /////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace1", 837.f, 297.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace2", 837.f + 67.5f, 297.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace3", 837.f + 67.5f * 2, 297.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace4", 837.f + 67.5f * 3, 297.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace5", 837.f + 67.5f * 4, 297.5f, 70, 70, TRUE, 255);

			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace6", 837.f, 365.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace7", 837.f + 67.5f, 365.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace8", 837.f + 67.5f * 2, 365.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace9", 837.f + 67.5f * 3, 365.f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INV_ItemSpace10", 837.f + 67.5f * 4, 365.f, 70, 70, TRUE, 255);
		}
		//////////////////////////////////////////////////////////// INV - HighLight /////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight1", 835.f, 292.5f, 70, 70, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight2", 835.f + 67.5f, 292.5f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight3", 835.f + 67.5f * 2, 292.5f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight4", 835.f + 67.5f * 3, 292.5f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight5", 835.f + 67.5f * 4, 292.5f, 70, 70, FALSE, 255);

			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight6", 835.f, 360.f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight7", 835.f + 67.5f, 360.f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight8", 835.f + 67.5f * 2, 360.f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight9", 835.f + 67.5f * 3, 360.f, 70, 70, FALSE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_SelectFrame.png", L"INV_HighLight10", 835.f + 67.5f * 4, 360.f, 70, 70, FALSE, 255);
		}
		///////////////////////////////////////////////////////////// INV - KeyMenu //////////////////////////////////////////////////////////////
		{
			Component_Sprite->Import_SpriteEX(BaseFolder, L"KEY_Q.png", L"Inven_KEY_Q", 1000.f, 435.f, 20, 20, TRUE, 255);
			Component_Sprite->Import_SpriteEX(BaseFolder, L"KEY_E.png", L"Inven_KEY_E", 1110.f, 435.f, 20, 20, TRUE, 255);
		}

	}
	/////////////////////////////////////////////////////////////// INFORMATION //////////////////////////////////////////////////////////////
	{
		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_ItemInfo_Top.png", L"INFO_BGTop", 140.f, 180.f, 335, 120, TRUE, 150));
		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_ItemInfo_Mid.png", L"INFO_BGMid", 140.f, 300.f, 335, 110, TRUE, 150));
		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_ItemInfo_Bot.png", L"INFO_BGBot", 140.f, 410.f, 335, 63, TRUE, 150));

		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"", L"INFO_ItemSpace", 173.f, 223.f, 90, 90, TRUE, 200));
		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_ItemInfo_Frame.png", L"INFO_ItemFrame", 160.f, 210.f, 105, 105, TRUE, 200));
	}

	return S_OK;
}
HRESULT PlayerInven::Text_Initialize() {
	//////////////////////////////////////////////////////// INVEN OPTION ///////////////////////////////////////////////////////////////
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"버리기", { 1050.f, 437.f }, 16, L"Inven_QText", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(255, 255, 255, 255), 100, FALSE);
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"선택"	, { 1150.f, 437.f }, 16, L"Inven_EText", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(255, 255, 255, 255), 100, FALSE);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////// INVEN INFO ////////////////////////////////////////////////////////////////
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 184.f }, 15, L"ITEM_Title"	 , L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 360.f, 225.f }, 12, L"ITEM_Class"	 , L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 363.f, 243.f }, 12, L"ITEM_ATKType", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 363.f, 255.f }, 12, L"ITEM_ATK"	 , L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 363.f, 267.f }, 12, L"ITEM_Add"	 , L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));

	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 160.f, 330.f }, 12, L"ITEM_DESC"	 , L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 0, 255, 0), 100, TRUE, DT_LEFT));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 160.f, 420.f }, 12, L"ITEM_ExDESC" , L"Yoon\u00AE 대한", D3DCOLOR_ARGB(120, 255, 255, 255), 100, TRUE, DT_LEFT));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return S_OK;
}
HRESULT PlayerInven::Item_Initialize() {
	wstring BaseFolder = L"../../UI/Inventory_UI/";

	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"DarkBow.png",	 L"DIC_InvenFrame_DarkBow", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"GreenBow.png",	 L"DIC_InvenFrame_GreenBow", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"IceBow.png",	 L"DIC_InvenFrame_IceBow", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"MichaelBow.png", L"DIC_InvenFrame_MichaelBow", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Relic_Item1.png", L"DIC_InvenFrame_Relic_Item1", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Relic_Horcrux.png", L"DIC_InvenFrame_Relic_Horcrux", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Relic_quiver.png", L"DIC_InvenFrame_Relic_quiver", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Relic_Glove.png", L"DIC_InvenFrame_Relic_Glove", 0.f, 0.f, 60, 60, FALSE, 255));

	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"DarkBow.png",	 L"DIC_InfoFrame_DarkBow", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"GreenBow.png",	 L"DIC_InfoFrame_GreenBow", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"IceBow.png",		 L"DIC_InfoFrame_IceBow", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"MichaelBow.png", L"DIC_InfoFrame_MichaelBow", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Relic_Item1.png", L"DIC_InfoFrame_Relic_Item1", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Relic_Horcrux.png", L"DIC_InfoFrame_Relic_Horcrux", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Relic_Relic_quiver.png", L"DIC_InfoFrame_Relic_quiver", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Relic_Glove.png",  L"DIC_InfoFrame_Relic_Glove", 0.f, 0.f, 80, 80, FALSE, 255));

	ItemINFO* it06 = new ItemINFO;
	it06->ItemDesc = {
		L"헤르메스의 신발",
		L"아티펙트/노말",

		L"이동속도가 20% 증가합니다.",
		L"",
		L"",

		L"",


		L"" ,

		L"DIC_InvenFrame_Relic_Item1",
		L"DIC_InfoFrame_Relic_Item1"
	};
	it06->ItemPrice = 70;
	it06->ItemType = (int)ITEM_TYPE::NORMAL_UTILITY;

	ItemINFO* it07 = new ItemINFO;
	it07->ItemDesc = {
		L"호크룩스",
		L"아티펙트/노말",

		L"공격력이 10 증가합니다.",
		L"",
		L"",

		L"",


		L"" ,

		L"DIC_InvenFrame_Relic_Horcrux",
		L"DIC_InfoFrame_Relic_Horcrux"
	};
	it07->ItemPrice = 70;
	it07->ItemType = (int)ITEM_TYPE::NORMAL_UTILITY;

	ItemINFO* it01 = new ItemINFO;
	it01->ItemDesc = {
		L"오동나무 활",
		L"무기/희귀",

		L"일반 공격",
		L"일반 공격력 24 ~ 26",
		L"공격 속도 2.5",

		L"치명타 확률이 +3% 증가합니다.",

		L"" ,

		L"DIC_InvenFrame_MichaelBow",
		L"DIC_InfoFrame_MichaelBow"
	};
	it01->ItemPrice = 68;
	it01->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;

	ItemINFO* it04 = new ItemINFO;
	it04->ItemDesc = {
		L"얼음 정령의 활",
		L"무기/희귀",

		L"일반 공격",
		L"일반 공격력 14 - 16",
		L"공격 속도 2",

		L"얼음의 화살 : 3 발의 얼음의 화살을 발사합니다. \n화살에 맞은 적에게 3초 간 빙결을 부여합니다.",

		L"\"얼음정령의 힘이 담긴 활. 쥐고 있음 손이 얼어버릴 것 같다.",

		L"DIC_InvenFrame_IceBow",
		L"DIC_InfoFrame_IceBow"
	};
	it04->ItemPrice = 68;
	it04->ItemType = (int)ITEM_TYPE::RARE_WEAPON;

	ItemINFO* it02 = new ItemINFO;
	it02->ItemDesc = {
		L"풍수의 활",
		L"무기/희귀",

		L"일반 공격",
		L"이동 속도 + 20%",
		L"공격 속도 2.5",

		L"그냥 빨라집니다.",

		L"",

		L"DIC_InvenFrame_DarkBow",
		L"DIC_InfoFrame_DarkBow"
	};
	it02->ItemPrice = 68;
	it02->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;

	ItemINFO* it03 = new ItemINFO;
	it03->ItemDesc = {
		L"풍수의 활",
		L"무기/희귀",

		L"일반 공격",
		L"이동 속도 + 20%",
		L"공격 속도 2.5",

		L"그냥 빨라집니다.",

		L"",

		L"DIC_InvenFrame_GreenBow",
		L"DIC_InfoFrame_GreenBow"
	};
	it03->ItemPrice = 68;
	it03->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;



	Append_Item(it03);
	Append_Item(it04);
	Append_Item(it02);
	Append_Item(it01);
	Append_Item(it06);
	Append_Item(it07);
	//Append_Item(it05);

	return S_OK;
}

HRESULT PlayerInven::Selecting_SavedItem() {
	if (FocusOn_SavedItem) {
		if (KEY_DOWN(DIK_W)) {
			if (SavedItemIndex <= 5)	return E_FAIL;
			else {
				SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select up.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
				SavedItemIndex -= 5;
				wstring PastFrame = L"INV_HighLight" + to_wstring(SavedItemIndex + 5);
				wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_A)) {
			if (SavedItemIndex == 1 || SavedItemIndex == 6) {
				SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select up.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
				wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(FALSE);
				FocusOn_EquipedItem = TRUE;
				Selecting_EquipItem();
			}
			else {
				SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select up.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
				SavedItemIndex -= 1;
				wstring PastFrame = L"INV_HighLight" + to_wstring(SavedItemIndex + 1);
				wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_S)) {
			if (SavedItemIndex >= 6)	return E_FAIL;
			else {
				SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select up.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
				SavedItemIndex += 5;
				wstring PastFrame = L"INV_HighLight" + to_wstring(SavedItemIndex - 5);
				wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_D)) {
			SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select up.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
			if (SavedItemIndex == 10)	return E_FAIL;
			else {
				SavedItemIndex += 1;
				wstring PastFrame = L"INV_HighLight" + to_wstring(SavedItemIndex - 1);
				wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
	}
	return S_OK;
}
HRESULT PlayerInven::Selecting_EquipItem() {
	if (SavedItemIndex == 1 && FocusOn_SavedItem && FocusOn_EquipedItem) {
		EquipedItemIndex = 4;
		FocusOn_SavedItem = FALSE;
		wstring SelectedFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
		Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);

	}
	else if (SavedItemIndex == 6 && FocusOn_SavedItem && FocusOn_EquipedItem) {
		EquipedItemIndex = 8;
		FocusOn_SavedItem = FALSE;
		wstring SelectedFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
		Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
	}

	if (FocusOn_EquipedItem) {
		if (KEY_DOWN(DIK_W)) {
			if (EquipedItemIndex <= 4)	return E_FAIL;
			else {
				SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select up.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
				EquipedItemIndex -= 4;
				wstring PastFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex + 4);
				wstring SelectedFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_A)) {
			if (EquipedItemIndex == 1 || EquipedItemIndex == 5)	return E_FAIL;
			else {
				SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select up.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
				EquipedItemIndex -= 1;
				wstring PastFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex + 1);
				wstring SelectedFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_S)) {
			if (EquipedItemIndex >= 5)	return E_FAIL;
			else {
				SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select up.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
				EquipedItemIndex += 4;
				wstring PastFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex - 4);
				wstring SelectedFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_D)) {
			if (EquipedItemIndex == 4 || EquipedItemIndex == 8) {
				FocusOn_SavedItem = TRUE;
				FocusOn_EquipedItem = FALSE;

				if (EquipedItemIndex == 4) {
					wstring PastFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
					Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
					SavedItemIndex = 1;
					wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
					Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				}
				if (EquipedItemIndex == 8) {
					wstring PastFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
					Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
					SavedItemIndex = 6;
					wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
					Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				}

				return S_OK;
			}
			else {
				SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select up.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
				EquipedItemIndex += 1;
				wstring PastFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex - 1);
				wstring SelectedFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
	}
	return S_OK;
}
HRESULT PlayerInven::Equip_Item() {
	if (EquipMode) {
		Component_Sprite->Get_Texture(L"Inven_KEY_Q")->Set_Visible(FALSE);
		UIManager::GetInstance()->Find_FontObject(L"Inven_QText")->Text = L"";
		UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Text = L"장착";
		if (KEY_DOWN(DIK_E)) {
			SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
			if (FocusOn_SavedItem) {
				swap(Saved_ItemList[SavedItemIndex - 1], *EquipObject);
				PlayerObject->Chage_Item(srcIdx, SavedItemIndex + 7);
				EquipObject = nullptr;
				EquipMode = FALSE;
				Component_Sprite->Get_Texture(L"Inven_KEY_Q")->Set_Visible(TRUE);
				UIManager::GetInstance()->Find_FontObject(L"Inven_QText")->Text = L"버리기";
				UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Text = L"선택";
			}
			if (FocusOn_EquipedItem) {
				if (((*EquipObject)->ItemType == (INT)ITEM_TYPE::NORMAL_WEAPON || (*EquipObject)->ItemType == (INT)ITEM_TYPE::RARE_WEAPON)
					&& (EquipedItemIndex >= 5 && EquipedItemIndex <= 8)) return E_FAIL;
				if (((*EquipObject)->ItemType == (INT)ITEM_TYPE::NORMAL_UTILITY || (*EquipObject)->ItemType == (INT)ITEM_TYPE::RARE_UTILITY)
					&& (EquipedItemIndex >= 1 && EquipedItemIndex <= 4)) return E_FAIL;

				swap(Equip_ItemList[EquipedItemIndex - 1], *EquipObject);
				PlayerObject->Chage_Item(srcIdx, EquipedItemIndex - 1);

				EquipObject = nullptr;
				EquipMode = FALSE;
				Component_Sprite->Get_Texture(L"Inven_KEY_Q")->Set_Visible(TRUE);
				UIManager::GetInstance()->Find_FontObject(L"Inven_QText")->Text = L"버리기";
				UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Text = L"선택";
			}
		}
	}

	if (FocusOn_SavedItem) {
		if (Saved_ItemList[SavedItemIndex - 1] != nullptr && KEY_DOWN(DIK_E)) {
			SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
			EquipMode = TRUE;
			EquipObject = &Saved_ItemList[SavedItemIndex - 1];
			srcIdx = SavedItemIndex +7;
		}
		if (Saved_ItemList[SavedItemIndex - 1] != nullptr && KEY_DOWN(DIK_Q)) {
			SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select down.wav", CHANNELID::SOUND_EFFECT05, 0.7f);
			JunkObject = Saved_ItemList[SavedItemIndex - 1];
			Safe_Delete(Saved_ItemList[SavedItemIndex - 1]);
			PlayerObject->Delete_item(SavedItemIndex + 7);
		}
	}
	if (FocusOn_EquipedItem) {
		if (Equip_ItemList[EquipedItemIndex - 1] != nullptr && KEY_DOWN(DIK_E)) {
			SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select.wav", CHANNELID::SOUND_EFFECT05, 0.5f);
			EquipMode = TRUE;
			EquipObject = &Equip_ItemList[EquipedItemIndex - 1];
			srcIdx = EquipedItemIndex - 1;
		}
		if (Equip_ItemList[EquipedItemIndex - 1] != nullptr && KEY_DOWN(DIK_Q)) {
			SoundManager::GetInstance()->Play_Sound_Once(L"UI/Inventory/UI_Select down.wav", CHANNELID::SOUND_EFFECT05, 0.7f);
			JunkObject = Equip_ItemList[EquipedItemIndex - 1];
			Safe_Delete(Equip_ItemList[EquipedItemIndex - 1]);
			PlayerObject->Delete_item(EquipedItemIndex - 1);
		}
	}

	return S_OK;
}
HRESULT PlayerInven::Display_ItemInfo() {

	for (INT IDX = 0; IDX < 8; ++IDX) {
		if (Equip_ItemList[IDX] == nullptr) {
			Component_Sprite->Get_Texture(L"EQP_ItemSpace" + to_wstring(IDX + 1))->TEXTURE = nullptr;
			continue;
		}
		Component_Sprite->Get_Texture(L"EQP_ItemSpace" + to_wstring(IDX + 1))->TEXTURE = Component_Sprite->Get_Texture(Equip_ItemList[IDX]->ItemDesc[7])->TEXTURE;
	}
	if (Equip_ItemList[EquipedItemIndex - 1] != nullptr && FocusOn_EquipedItem) {
		Component_Sprite->Get_Texture(L"INFO_ItemSpace")->TEXTURE = Component_Sprite->Get_Texture(Equip_ItemList[EquipedItemIndex - 1]->ItemDesc[8])->TEXTURE;

		ItemInfo_Text[0]->Text = Equip_ItemList[EquipedItemIndex - 1]->ItemDesc[0];
		ItemInfo_Text[1]->Text = Equip_ItemList[EquipedItemIndex - 1]->ItemDesc[1];
		ItemInfo_Text[2]->Text = Equip_ItemList[EquipedItemIndex - 1]->ItemDesc[2];
		ItemInfo_Text[3]->Text = Equip_ItemList[EquipedItemIndex - 1]->ItemDesc[3];
		ItemInfo_Text[4]->Text = Equip_ItemList[EquipedItemIndex - 1]->ItemDesc[4];
		ItemInfo_Text[5]->Text = Equip_ItemList[EquipedItemIndex - 1]->ItemDesc[5];
		ItemInfo_Text[6]->Text = Equip_ItemList[EquipedItemIndex - 1]->ItemDesc[6];

		if (Equip_ItemList[EquipedItemIndex - 1]->ItemType == (INT)ITEM_TYPE::RARE_WEAPON ||
			Equip_ItemList[EquipedItemIndex - 1]->ItemType == (INT)ITEM_TYPE::RARE_UTILITY) {
			UIManager::GetInstance()->Find_FontObject(L"ITEM_Class")->TextColor = D3DCOLOR_ARGB(200, 100, 100, 255);
		}
		else {
			UIManager::GetInstance()->Find_FontObject(L"ITEM_Class")->TextColor = D3DCOLOR_ARGB(200, 255, 255, 255);
		}
	}
	else if (FocusOn_EquipedItem) {
		Component_Sprite->Get_Texture(L"INFO_ItemSpace")->TEXTURE = nullptr;
	}

	for (INT IDX = 0; IDX < 10; ++IDX) {
		if (Saved_ItemList[IDX] == nullptr) {
			Component_Sprite->Get_Texture(L"INV_ItemSpace" + to_wstring(IDX + 1))->TEXTURE = nullptr;
			continue;
		}
		Component_Sprite->Get_Texture(L"INV_ItemSpace" + to_wstring(IDX + 1))->TEXTURE = Component_Sprite->Get_Texture(Saved_ItemList[IDX]->ItemDesc[7])->TEXTURE;
	}
	if (Saved_ItemList[SavedItemIndex - 1] != nullptr && FocusOn_SavedItem) {
		Component_Sprite->Get_Texture(L"INFO_ItemSpace")->TEXTURE = Component_Sprite->Get_Texture(Saved_ItemList[SavedItemIndex - 1]->ItemDesc[8])->TEXTURE;

		ItemInfo_Text[0]->Text = Saved_ItemList[SavedItemIndex - 1]->ItemDesc[0];
		ItemInfo_Text[1]->Text = Saved_ItemList[SavedItemIndex - 1]->ItemDesc[1];
		ItemInfo_Text[2]->Text = Saved_ItemList[SavedItemIndex - 1]->ItemDesc[2];
		ItemInfo_Text[3]->Text = Saved_ItemList[SavedItemIndex - 1]->ItemDesc[3];
		ItemInfo_Text[4]->Text = Saved_ItemList[SavedItemIndex - 1]->ItemDesc[4];
		ItemInfo_Text[5]->Text = Saved_ItemList[SavedItemIndex - 1]->ItemDesc[5];
		ItemInfo_Text[6]->Text = Saved_ItemList[SavedItemIndex - 1]->ItemDesc[6];

		if (Saved_ItemList[SavedItemIndex - 1]->ItemType == (INT)ITEM_TYPE::RARE_WEAPON ||
			Saved_ItemList[SavedItemIndex - 1]->ItemType == (INT)ITEM_TYPE::RARE_UTILITY) {
			UIManager::GetInstance()->Find_FontObject(L"ITEM_Class")->TextColor = D3DCOLOR_ARGB(200, 100, 100, 255);
		}
		else {
			UIManager::GetInstance()->Find_FontObject(L"ITEM_Class")->TextColor = D3DCOLOR_ARGB(200, 255, 255, 255);
		}
	}
	else if (FocusOn_SavedItem) {
		Component_Sprite->Get_Texture(L"INFO_ItemSpace")->TEXTURE = nullptr;
	}

	if (FocusOn_SavedItem) {
		if (Saved_ItemList[SavedItemIndex - 1] != nullptr) {
			for (auto& Comp : ItemInfo_Screen)
				Comp->Set_Visible(TRUE);
			for (auto& Comp : ItemInfo_Text)
				Comp->Set_Visible(TRUE);
		}
		else {
			for (auto& Comp : ItemInfo_Screen)
				Comp->Set_Visible(FALSE);
			for (auto& Comp : ItemInfo_Text)
				Comp->Set_Visible(FALSE);
		}
	}
	if (FocusOn_EquipedItem) {
		if (Equip_ItemList[EquipedItemIndex - 1] != nullptr) {
			for (auto& Comp : ItemInfo_Screen)
				Comp->Set_Visible(TRUE);
			for (auto& Comp : ItemInfo_Text)
				Comp->Set_Visible(TRUE);
		}
		else {
			for (auto& Comp : ItemInfo_Screen)
				Comp->Set_Visible(FALSE);
			for (auto& Comp : ItemInfo_Text)
				Comp->Set_Visible(FALSE);
		}
	}
	return S_OK;
}


HRESULT PlayerInven::Append_Item(ItemINFO* _ITEM) {
	if (_ITEM->ItemType == (INT)ITEM_TYPE::NORMAL_WEAPON ||
		_ITEM->ItemType == (INT)ITEM_TYPE::RARE_WEAPON) {
		for (INT IDX = 0; IDX < 4; ++IDX) {
			if (Equip_ItemList[IDX] == nullptr) {
				Equip_ItemList[IDX] = _ITEM;
				return S_OK;
			}
		}
		for (INT IDX = 0; IDX < 10; ++IDX) {
			if (Saved_ItemList[IDX] == nullptr) {
				Saved_ItemList[IDX] = _ITEM;
				return S_OK;
			}
		}
		return E_FAIL;
	}
	else if (_ITEM->ItemType == (INT)ITEM_TYPE::NORMAL_UTILITY ||
		_ITEM->ItemType == (INT)ITEM_TYPE::RARE_UTILITY) {
		for (INT IDX = 4; IDX < 8; ++IDX) {
			if (Equip_ItemList[IDX] == nullptr) {
				Equip_ItemList[IDX] = _ITEM;
				return S_OK;
			}
		}
		for (INT IDX = 0; IDX < 10; ++IDX) {
			if (Saved_ItemList[IDX] == nullptr) {
				Saved_ItemList[IDX] = _ITEM;
				return S_OK;
			}
		}
		return E_FAIL;
	}
	return S_OK;
}
HRESULT PlayerInven::Buy_Item(INT itemIdx)
{
	ItemINFO* item = nullptr;
	switch (itemIdx) {
	case 3:
		item = new ItemINFO;
		item->ItemDesc = {
			L"얼음 정령의 활",
			L"무기/희귀",

			L"일반 공격",
			L"일반 공격력 14 - 16",
			L"공격 속도 2",

			L"얼음의 화살 : 3 발의 얼음의 화살을 발사합니다. \n화살에 맞은 적에게 3초 간 빙결을 부여합니다.",

			L"\"얼음정령의 힘이 담긴 활. 쥐고 있음 손이 얼어버릴 것 같다.",

			L"DIC_InvenFrame_IceBow",
			L"DIC_InfoFrame_IceBow"
		};
		item->ItemPrice = 68;
		item->ItemType = (int)ITEM_TYPE::RARE_WEAPON;

		Append_Item(item);
		break;
	case 4:
		item = new ItemINFO;
		item->ItemDesc = {
			L"맥스 퀴버",
			L"유물/희귀",

			L"",
			L"유물/희귀",
			L"최대 화살 개수 2배 증가",

			L"",

			L"",

			L"DIC_InvenFrame_Relic_quiver",
			L"DIC_InfoFrame_Relic_quiver"
		};

		item->ItemPrice = 10;
		item->ItemType = (int)ITEM_TYPE::RARE_UTILITY;
		Append_Item(item);
		break;

	case 5:
		item = new ItemINFO;
		item->ItemDesc = {
			L"라이트닝 글러브",
			L"유물/희귀",

			L"",
			L"유물/희귀",
			L"공격속도 2배 증가",

			L"",

			L"",

			L"DIC_InvenFrame_Relic_Glove",
			L"DIC_InfoFrame_Relic_Glove"
		};

		item->ItemPrice = 30;
		item->ItemType = (int)ITEM_TYPE::RARE_UTILITY;
		Append_Item(item);
		break;
	}

	return S_OK;
}
PlayerInven* PlayerInven::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	PlayerInven* MUI = new PlayerInven(_GRPDEV);
	if (FAILED(MUI->Ready_GameObject())) {
		MSG_BOX("Cannot Create PlayerInven.");
		Safe_Release(MUI);
		return nullptr;
	}
	return MUI;
}
VOID	PlayerInven::Free() {
	for (auto& EI : Equip_ItemList)
		Safe_Delete(EI);
	for (auto& SI : Saved_ItemList)
		Safe_Delete(SI);
	GameObject::Free();
}
