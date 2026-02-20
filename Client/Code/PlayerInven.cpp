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

	FocusOn_SavedItem = FALSE;
	FocusOn_EquipedItem = FALSE;

	EquipMode = FALSE;

	return S_OK;
}
INT		PlayerInven::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	if (KEY_DOWN(DIK_B)) {
		UIManager::GetInstance()->Get_Active() ? UIManager::GetInstance()->Set_Active(FALSE) : UIManager::GetInstance()->Set_Active(TRUE);

		if (UIManager::GetInstance()->Get_Active() == TRUE) {
			FocusOn_SavedItem = TRUE;

			for (auto& TXT : ItemInfo_Text) {
				TXT->Visible = TRUE;
				UIManager::GetInstance()->Find_FontObject(L"Inven_QText")->Visible = TRUE;
				UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Visible = TRUE;
			}
		}
		else if (UIManager::GetInstance()->Get_Active() == FALSE){
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
	
	Selecting_SavedItem();
	Selecting_EquipItem();
	Display_ItemInfo();
	Equip_Item();

	return 0;
}
VOID	PlayerInven::LateUpdate_GameObject(CONST FLOAT& _DT) {

}
VOID	PlayerInven::Render_GameObject() {	
	if (UIManager::GetInstance()->Get_Active() == TRUE) {
		PlayerObject->Set_PlayerStop(TRUE);
		Component_Sprite->Render_Sprite();
	}
	else if(UIManager::GetInstance()->Get_Active() == FALSE){
		PlayerObject->Set_PlayerStop(FALSE);
	}
}	

HRESULT PlayerInven::Component_Initialize() {
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
		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_ItemInfo_Top.png",	 L"INFO_BGTop", 140.f, 180.f, 335, 120, TRUE, 150));
		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_ItemInfo_Mid.png",	 L"INFO_BGMid", 140.f, 300.f, 335, 110, TRUE, 150));
		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_ItemInfo_Bot.png",	 L"INFO_BGBot", 140.f, 410.f, 335, 63, TRUE, 150));

		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"",						 L"INFO_ItemSpace", 173.f, 223.f, 90, 90, TRUE, 200));
		ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Inven_ItemInfo_Frame.png", L"INFO_ItemFrame", 160.f, 210.f, 105, 105, TRUE, 200));
}

	return S_OK;
}
HRESULT PlayerInven::Text_Initialize() {
	//////////////////////////////////////////////////////// INVEN OPTION ///////////////////////////////////////////////////////////////
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"πˆ∏Æ±‚", { 1050.f, 437.f }, 16, L"Inven_QText", L"Yoon\u00AE ¥Î«—", D3DCOLOR_ARGB(255, 255, 255, 255), 100, FALSE);
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"º±≈√"	, { 1150.f, 437.f }, 16, L"Inven_EText", L"Yoon\u00AE ¥Î«—", D3DCOLOR_ARGB(255, 255, 255, 255), 100, FALSE);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////// INVEN INFO ////////////////////////////////////////////////////////////////
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 184.f }, 15, L"ITEM_Title"	 , L"Yoon\u00AE ¥Î«—", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 360.f, 225.f }, 12, L"ITEM_Class"	 , L"Yoon\u00AE ¥Î«—", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 363.f, 243.f }, 12, L"ITEM_ATKType", L"Yoon\u00AE ¥Î«—", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 363.f, 255.f }, 12, L"ITEM_ATK"	 , L"Yoon\u00AE ¥Î«—", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 363.f, 267.f }, 12, L"ITEM_Add"	 , L"Yoon\u00AE ¥Î«—", D3DCOLOR_ARGB(200, 255, 255, 255)));

	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 160.f, 330.f }, 12, L"ITEM_DESC"	 , L"Yoon\u00AE ¥Î«—", D3DCOLOR_ARGB(200, 0, 255, 0), 100, TRUE, DT_LEFT));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 160.f, 420.f }, 12, L"ITEM_ExDESC" , L"Yoon\u00AE ¥Î«—", D3DCOLOR_ARGB(120, 255, 255, 255), 100, TRUE, DT_LEFT));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return S_OK;
}
HRESULT PlayerInven::Item_Initialize() {
	wstring BaseFolder = L"../../UI/Inventory_UI/";

	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"DarkBow.png",	 L"DIC_InvenFrame_DarkBow", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"GreenBow.png",	 L"DIC_InvenFrame_GreenBow", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"IceBow.png",	 L"DIC_InvenFrame_IceBow", 0.f, 0.f, 60, 60, FALSE, 255));
	ItemDictionary_InvenFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"MichaelBow.png", L"DIC_InvenFrame_MichaelBow", 0.f, 0.f, 60, 60, FALSE, 255));

	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"DarkBow.png",	 L"DIC_InfoFrame_DarkBow", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"GreenBow.png",	 L"DIC_InfoFrame_GreenBow", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"IceBow.png",		 L"DIC_InfoFrame_IceBow", 0.f, 0.f, 80, 80, FALSE, 255));
	ItemDictionary_InfoFrame.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"MichaelBow.png",  L"DIC_InfoFrame_MichaelBow", 0.f, 0.f, 80, 80, FALSE, 255));

	ItemINFO* it01 = new ItemINFO;
	it01->ItemDesc = {
		L"?§Îèô?òÎ¨¥ ??,
		L"Î¨¥Í∏∞/?¨Í?",

		L"?ºÎ∞ò Í≥µÍ≤©",
		L"?ºÎ∞ò Í≥µÍ≤©??24 ~ 26",
		L"Í≥µÍ≤© ?çÎèÑ 2.5",

		L"ÏπòÎ™Ö?Ä ?ïÎ•†??+3% Ï¶ùÍ??©Îãà??",

		L"" ,

		L"DIC_InvenFrame_MichaelBow",
		L"DIC_InfoFrame_MichaelBow" 
	};
	it01->ItemPrice = 68;
	it01->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;

	Append_Item(it01);

	ItemINFO* it02 = new ItemINFO;
	it02->ItemDesc = { 
		L"?çÏàò????, 
		L"Î¨¥Í∏∞/?¨Í?", 

		L"?ºÎ∞ò Í≥µÍ≤©", 
		L"?¥Îèô ?çÎèÑ + 20%", 
		L"Í≥µÍ≤© ?çÎèÑ 2.5", 

		L"Í∑∏ÎÉ• Îπ®ÎùºÏßëÎãà??", 

		L"", 

		L"DIC_InvenFrame_DarkBow", 
		L"DIC_InfoFrame_DarkBow" 
	};
	it02->ItemPrice = 68;
	it02->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;

	Append_Item(it02);

	ItemINFO* it03 = new ItemINFO;
	it03->ItemDesc = { 
		L"?çÏàò????, 
		L"Î¨¥Í∏∞/?¨Í?", 

		L"?ºÎ∞ò Í≥µÍ≤©", 
		L"?¥Îèô ?çÎèÑ + 20%", 
		L"Í≥µÍ≤© ?çÎèÑ 2.5", 
		
		L"Í∑∏ÎÉ• Îπ®ÎùºÏßëÎãà??", 

		L"", 

		L"DIC_InvenFrame_GreenBow", 
		L"DIC_InfoFrame_GreenBow" 
	};
	it03->ItemPrice = 68;
	it03->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;

	Append_Item(it03);

	ItemINFO* it04 = new ItemINFO;
	it04->ItemDesc = { 
		L"?ºÏùå ?ïÎ†π????, 
		L"Î¨¥Í∏∞/?¨Í?", 
		
		L"?ºÎ∞ò Í≥µÍ≤©", 
		L"?ºÎ∞ò Í≥µÍ≤©??14 - 16", 
		L"Í≥µÍ≤© ?çÎèÑ 2", 

		L"?ºÏùå???îÏÇ¥ : 3 Î∞úÏùò ?ºÏùå???îÏÇ¥??Î∞úÏÇ¨?©Îãà?? \n?îÏÇ¥??ÎßûÏ? ?ÅÏóêÍ≤?3Ï¥?Í∞?ÎπôÍ≤∞??Î∂Ä?¨Ìï©?àÎã§.", 

		L"\"?ºÏùå?ïÎ†π???òÏù¥ ?¥Í∏¥ ?? Ï•êÍ≥† ?àÏùå ?êÏù¥ ?ºÏñ¥Î≤ÑÎ¶¥ Í≤?Í∞ôÎã§.", 

		L"DIC_InvenFrame_IceBow", 
		L"DIC_InfoFrame_IceBow" 
	};
	it04->ItemPrice = 68;
	it04->ItemType = (int)ITEM_TYPE::RARE_WEAPON;

	Append_Item(it04);

	ItemINFO* it05 = new ItemINFO;
	it05->ItemDesc = { L"?ºÏùå ?ïÎ†π????,
		L"Î¨¥Í∏∞/?¨Í?",

		L"?ºÎ∞ò Í≥µÍ≤©",
		L"?ºÎ∞ò Í≥µÍ≤©??14 - 16",
		L"Í≥µÍ≤© ?çÎèÑ 2",

		L"?ºÏùå???îÏÇ¥ : 3 Î∞úÏùò ?ºÏùå???îÏÇ¥??Î∞úÏÇ¨?©Îãà?? ?îÏÇ¥??ÎßûÏ? ?ÅÏóêÍ≤?\n3Ï¥?Í∞?ÎπôÍ≤∞??Î∂Ä?¨Ìï©?àÎã§.",

		L"\"?ºÏùå?ïÎ†π???òÏù¥ ?¥Í∏¥ ?? Ï•êÍ≥† ?àÏùå ?êÏù¥ ?ºÏñ¥Î≤ÑÎ¶¥ Í≤?Í∞ôÎã§.",

		L"DIC_InvenFrame_IceBow",
		L"DIC_InfoFrame_IceBow"
	};
	it05->ItemPrice = 68;
	it05->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;

	Append_Item(it05);

	return S_OK;
}

HRESULT PlayerInven::Selecting_SavedItem() {
	if (FocusOn_SavedItem) {
		if		(KEY_DOWN(DIK_W)) {
			if (SavedItemIndex <= 5)	return E_FAIL;
			else {
				SavedItemIndex -= 5;
				wstring PastFrame = L"INV_HighLight" + to_wstring(SavedItemIndex + 5);
				wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_A)) {
			if (SavedItemIndex == 1 || SavedItemIndex == 6) {
				wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(FALSE);
				FocusOn_EquipedItem = TRUE;
				Selecting_EquipItem();
			}
			else {
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
				SavedItemIndex += 5;
				wstring PastFrame = L"INV_HighLight" + to_wstring(SavedItemIndex - 5);
				wstring SelectedFrame = L"INV_HighLight" + to_wstring(SavedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_D)) {
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
	if		(SavedItemIndex == 1 && FocusOn_SavedItem && FocusOn_EquipedItem) {
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
		if		(KEY_DOWN(DIK_W)) {
			if (EquipedItemIndex <= 4)	return E_FAIL;
			else {
				EquipedItemIndex -= 4;
				wstring PastFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex + 4);
				wstring SelectedFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_A)){
			if (EquipedItemIndex == 1 || EquipedItemIndex == 5)	return E_FAIL;
			else {
				EquipedItemIndex -= 1;
				wstring PastFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex + 1);
				wstring SelectedFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_S)){
			if (EquipedItemIndex >= 5)	return E_FAIL;
			else {
				EquipedItemIndex += 4;
				wstring PastFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex - 4);
				wstring SelectedFrame = L"EQP_HighLight" + to_wstring(EquipedItemIndex);
				Component_Sprite->Get_Texture(SelectedFrame)->Set_Visible(TRUE);
				Component_Sprite->Get_Texture(PastFrame)->Set_Visible(FALSE);
			}
		}
		else if (KEY_DOWN(DIK_D)){
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
		UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Text = L"¿Â¬¯";
		if (KEY_DOWN(DIK_E)){
			if (FocusOn_SavedItem) {
				swap(Saved_ItemList[SavedItemIndex - 1], *EquipObject);
				EquipObject = nullptr;
				EquipMode = FALSE;
				Component_Sprite->Get_Texture(L"Inven_KEY_Q")->Set_Visible(TRUE);
				UIManager::GetInstance()->Find_FontObject(L"Inven_QText")->Text = L"πˆ∏Æ±‚";
				UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Text = L"º±≈√";
			}
			if (FocusOn_EquipedItem) {
				if (((*EquipObject)->ItemType == (INT)ITEM_TYPE::NORMAL_WEAPON || (*EquipObject)->ItemType == (INT)ITEM_TYPE::RARE_WEAPON)
					&& (EquipedItemIndex >= 5 && EquipedItemIndex <= 8)) return E_FAIL;
				if (((*EquipObject)->ItemType == (INT)ITEM_TYPE::NORMAL_UTILITY || (*EquipObject)->ItemType == (INT)ITEM_TYPE::RARE_UTILITY)
					&& (EquipedItemIndex >= 1 && EquipedItemIndex <= 4)) return E_FAIL;

				swap(Equip_ItemList[EquipedItemIndex - 1], *EquipObject);
				
				EquipObject = nullptr;
				EquipMode = FALSE;
				Component_Sprite->Get_Texture(L"Inven_KEY_Q")->Set_Visible(TRUE);
				UIManager::GetInstance()->Find_FontObject(L"Inven_QText")->Text = L"πˆ∏Æ±‚";
				UIManager::GetInstance()->Find_FontObject(L"Inven_EText")->Text = L"º±≈√";
			}
		}
	}

	if (FocusOn_SavedItem) {
		if (Saved_ItemList[SavedItemIndex - 1] != nullptr && KEY_DOWN(DIK_E)) {
			EquipMode = TRUE;
			EquipObject = &Saved_ItemList[SavedItemIndex - 1];
		}
		if (Saved_ItemList[SavedItemIndex - 1] != nullptr && KEY_DOWN(DIK_Q)) {
			JunkObject = Saved_ItemList[SavedItemIndex - 1];
			Safe_Delete(Saved_ItemList[SavedItemIndex - 1]);
		}
	}
	if (FocusOn_EquipedItem) {
		if (Equip_ItemList[EquipedItemIndex - 1] != nullptr && KEY_DOWN(DIK_E)) {
			EquipMode = TRUE;
			EquipObject = &Equip_ItemList[EquipedItemIndex - 1];
		}
		if (Equip_ItemList[EquipedItemIndex - 1] != nullptr && KEY_DOWN(DIK_Q)) {
			JunkObject = Equip_ItemList[EquipedItemIndex - 1];
			Safe_Delete(Equip_ItemList[EquipedItemIndex - 1]);
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
	else if( FocusOn_EquipedItem ){
		Component_Sprite->Get_Texture(L"INFO_ItemSpace")->TEXTURE = nullptr;
	}

	for (INT IDX = 0; IDX < 10; ++IDX) {
		if (Saved_ItemList[IDX] == nullptr) {
			Component_Sprite->Get_Texture(L"INV_ItemSpace" + to_wstring(IDX + 1))->TEXTURE = nullptr;
			continue;
		}
		Component_Sprite->Get_Texture(L"INV_ItemSpace" + to_wstring(IDX +1))->TEXTURE = Component_Sprite->Get_Texture(Saved_ItemList[IDX]->ItemDesc[7])->TEXTURE;
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
	else if( FocusOn_SavedItem ){
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
	if		(_ITEM->ItemType == (INT)ITEM_TYPE::NORMAL_WEAPON ||
			_ITEM->ItemType == (INT)ITEM_TYPE::RARE_WEAPON) {
		for (INT IDX = 0; IDX < 4; ++IDX) {
			if (Equip_ItemList[IDX] == nullptr) {
				Equip_ItemList[IDX] = _ITEM;
				return S_OK;
			}
		}
		// Î™®Îì† Equip Ïπ∏Ïù¥ Ï±ÑÏõå???àÎäî Í≤ΩÏö∞
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
		// Î™®Îì† Equip Ïπ∏Ïù¥ Ï±ÑÏõå???àÎäî Í≤ΩÏö∞
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
