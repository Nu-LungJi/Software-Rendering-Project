#include "../Include/PCH.h"
#include "ShopUI.h"

ShopUI::ShopUI(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
ShopUI::ShopUI(CONST GameObject& _RHS)		: GameObject(_RHS)		{}
ShopUI::~ShopUI()													{}

HRESULT	ShopUI::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;
	if (FAILED(Item_Initialize()))			return E_FAIL;
	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));
		
	return S_OK;
}
INT		ShopUI::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return 0;
}
VOID	ShopUI::LateUpdate_GameObject(CONST FLOAT& _DT) {

}
VOID	ShopUI::Render_GameObject() {
	Component_Sprite->Render_Sprite();
}

HRESULT	ShopUI::Component_Initialize() {
	Component_Sprite		= ADD_COMPONENT_SPRITE;
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Collider	= ADD_COMPONENT_COLLIDER;

	return S_OK;
}
HRESULT	ShopUI::Sprite_Initialize() {
	wstring BaseFolder = L"../../UI/Shop/";
	////////////////////////////////////////////COIN////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Coin2.png", L"COIN", 280.f, 435.f, 20, 20, TRUE);
	////////////////////////////////////////////ITEM_INFO////////////////////////////////////////////
	ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"ItemInformation_Top.png", L"INFO_BGTop", 140.f, 180.f, 335, 120, TRUE, 150));
	ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"ItemInformation_Mid.png", L"INFO_BGMid", 153.f, 300.f, 305, 110, TRUE, 150));
	ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"ItemInformation_Bottom.png", L"INFO_BGBot", 140.f, 410.f, 335, 63, TRUE, 150));
	////////////////////////////////////////////ITEM_SPRITE//////////////////////////////////////////
	Component_Sprite->Import_SpriteEX(BaseFolder, L"DarkBow.png", L"DIC_Field_DarkBow", 0.f, 0.f, 60, 60, TRUE, 255);
	return S_OK;
}
HRESULT	ShopUI::Effect_Initialize() {
	return S_OK;
}
HRESULT	ShopUI::Text_Initialize() {
	/////////////////////////////////////////////////////// ITEM INFO ////////////////////////////////////////////////////////////////
	ItemInfo_Text.push_back(FontManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 215.f }, 15, L"ITEM_Title", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(FontManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 255.f }, 12, L"ITEM_Class", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(FontManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 0.f, 0.f }, 12, L"ITEM_ATKType", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(FontManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 0.f, 0.f }, 12, L"ITEM_ATK", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(FontManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 0.f, 0.f }, 12, L"ITEM_Add", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	
	ItemInfo_Text.push_back(FontManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 200.f, 350.f }, 12, L"ITEM_DESC", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 0, 255, 0), 100, TRUE, DT_LEFT));
	ItemInfo_Text.push_back(FontManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 200.f, 400.f }, 12, L"ITEM_ExDESC", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(120, 255, 255, 255), 100, TRUE, DT_LEFT));
	ItemInfo_Text.push_back(FontManager::GetInstance()->Add_FontSprite(GRPDEV, L"50", { 310.f, 438.f }, 12, L"ITEM_PRICE", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return S_OK;
}

HRESULT ShopUI::Item_Initialize() {
	wstring BaseFolder = L"../../UI/Inventory_UI/";
		
	ItemINFO* m_pShopItem = new ItemINFO;

	m_pShopItem->ItemDesc = {
		
			L"인내의 활",
			L"무기/희귀",

			L"",
			L"",
			L"",

			L"차징시간이 길수록 데미지가 증가합니다.",

			L"때를 기다려라... \n단 한 순간에 적의 숨통을 끊을 수 있도록..",

			L"",
			L"DIC_InfoFrame"
	};
	m_pShopItem->ItemPrice = 68;
	m_pShopItem->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;

	Display_ShopItemInfo(m_pShopItem);

	return S_OK;
}

BOOL ShopUI::Get_Collision_Enter(GameObject* _Other){
	return FALSE;
}

BOOL ShopUI::Get_Collision_Stay(GameObject* _Other)
{
	return FALSE;
}

BOOL ShopUI::Get_Collision_Exit(GameObject* _Other)
{
	return 0;
}

ShopUI* ShopUI::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	ShopUI* SUI = new ShopUI(_GRPDEV);
	if (FAILED(SUI->Ready_GameObject()))
	{
		MSG_BOX("Cannot Create ShopUI");
		Safe_Release(SUI);
		return nullptr;
	}
	return SUI;
}
	
VOID ShopUI::Display_ShopItemInfo(ItemINFO* _pItem)
{
	if (nullptr == _pItem) {
		for (auto& Comp : ItemInfo_Screen) Comp->Set_Visible(FALSE);
		for (auto& Txt : ItemInfo_Text) Txt->Set_Visible(FALSE);
		return;
	}

	for (auto& Comp : ItemInfo_Screen) Comp->Set_Visible(TRUE);
	for (auto& Txt : ItemInfo_Text) Txt->Set_Visible(TRUE);

	ItemInfo_Text[0]->Text = _pItem->ItemDesc[0];
	ItemInfo_Text[1]->Text = _pItem->ItemDesc[1];
	ItemInfo_Text[2]->Text = _pItem->ItemDesc[2];
	ItemInfo_Text[3]->Text = _pItem->ItemDesc[3];
	ItemInfo_Text[4]->Text = _pItem->ItemDesc[4];
	ItemInfo_Text[5]->Text = _pItem->ItemDesc[5];
	ItemInfo_Text[6]->Text = _pItem->ItemDesc[6];
}

VOID	ShopUI::Free() {
	GameObject::Free();
}