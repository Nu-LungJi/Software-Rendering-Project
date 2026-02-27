#include "../Include/PCH.h"
#include "ShopUI.h"

ShopUI::ShopUI(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
ShopUI::ShopUI(CONST GameObject& _RHS) : GameObject(_RHS) {}
ShopUI::~ShopUI() {}

HRESULT   ShopUI::Ready_GameObject() {
    if (FAILED(Component_Initialize())) return E_FAIL;
    if (FAILED(Sprite_Initialize()))      return E_FAIL;
    if (FAILED(Text_Initialize()))         return E_FAIL;
    if (FAILED(Item_Initialize()))         return E_FAIL;

    return S_OK;
}
INT      ShopUI::Update_GameObject(CONST FLOAT& _DT) {
    GameObject::Update_GameObject(_DT);
    RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    //if (KEY_DOWN(DIK_T) && KEY_DOWN(DIK_LCONTROL))
    //{
    //    isActive = !isActive;
        if (!isActive)
        {
            UIManager::GetInstance()->Find_FontObject(L"ITEM_Title")->Visible = FALSE;
            UIManager::GetInstance()->Find_FontObject(L"ITEM_Class")->Visible = FALSE;
            UIManager::GetInstance()->Find_FontObject(L"ITEM_ATKType")->Visible = FALSE;
            UIManager::GetInstance()->Find_FontObject(L"ITEM_ATK")->Visible = FALSE;
            UIManager::GetInstance()->Find_FontObject(L"ITEM_Add")->Visible = FALSE;
            UIManager::GetInstance()->Find_FontObject(L"ITEM_DESC")->Visible = FALSE;
            UIManager::GetInstance()->Find_FontObject(L"ITEM_ExDESC")->Visible = FALSE;
            UIManager::GetInstance()->Find_FontObject(L"ITEM_PRICE")->Visible = FALSE;
        }
  //  }
    if (isActive) {
        Display_ShopItemInfo(Item_Index[m_iCurrentItemIndex]);

        if (KEY_DOWN(DIK_RIGHT)) {
            m_iCurrentItemIndex++;
            if (m_iCurrentItemIndex >= (INT)Item_Index.size()) m_iCurrentItemIndex = 0;
            Display_ShopItemInfo(Item_Index[m_iCurrentItemIndex]);
        }

        if (KEY_DOWN(DIK_LEFT)) {
            m_iCurrentItemIndex--;
            if (m_iCurrentItemIndex < 0) m_iCurrentItemIndex = (INT)Item_Index.size() - 1;
            Display_ShopItemInfo(Item_Index[m_iCurrentItemIndex]);
        }
    }
    
     Show_Item();
    
    return 0;
}
VOID   ShopUI::LateUpdate_GameObject(CONST FLOAT& _DT) {

}
VOID   ShopUI::Render_GameObject() {
    if (isActive)
    {
        Component_Sprite->Render_Sprite();
    }
        
}

HRESULT   ShopUI::Component_Initialize() {
    Component_Sprite = ADD_COMPONENT_SPRITE;
    Component_Transform = ADD_COMPONENT_TRANSFORM;
    Component_Collider = ADD_COMPONENT_COLLIDER;

    m_iCurrentItemIndex = 0;

	return S_OK;
}
HRESULT	ShopUI::Sprite_Initialize() {
	wstring BaseFolder = L"../../UI/Shop/";

	////////////////////////////////////////////COIN////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Coin2.png", L"COIN", 280.f, 435.f, 20, 20, TRUE);
	////////////////////////////////////////////ITEM_INFO////////////////////////////////////////////
	ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"Spr_UI_Icon_2.png", L"INFO_BGPaw", 298.f, 170.f, 25, 25, TRUE, 255));
	ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"ItemInformation_Top.png", L"INFO_BGTop", 140.f, 180.f, 335, 120, TRUE, 175));
	ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"ItemInformation_Mid.png", L"INFO_BGMid", 153.f, 300.f, 305, 110, TRUE, 175));
	ItemInfo_Screen.push_back(Component_Sprite->Import_SpriteEX(BaseFolder, L"ItemInformation_Bottom.png", L"INFO_BGBot", 140.f, 410.f, 335, 63, TRUE, 175));
	////////////////////////////////////////////ITEM_SPRITE//////////////////////////////////////////
	
    return S_OK;
}
HRESULT   ShopUI::Effect_Initialize() {
    return S_OK;
}
HRESULT	ShopUI::Text_Initialize() {
	/////////////////////////////////////////////////////// ITEM INFO ////////////////////////////////////////////////////////////////
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 215.f }, 15, L"ITEM_Title", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 255.f }, 12, L"ITEM_Class", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 275.f }, 12, L"ITEM_ATKType", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 295.f }, 12, L"ITEM_ATK", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 315.f }, 12, L"ITEM_Add", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
							
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 190.f, 350.f }, 12, L"ITEM_DESC", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 0, 255, 0), 100, TRUE, DT_LEFT));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 190.f, 400.f }, 12, L"ITEM_ExDESC", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(120, 255, 255, 255), 100, TRUE, DT_LEFT));
	ItemInfo_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 310.f, 438.f }, 12, L"ITEM_PRICE", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!isActive)
    {
        for (auto& Comp : ItemInfo_Screen) Comp->Set_Visible(FALSE);
        for (auto& Txt : ItemInfo_Text) Txt->Set_Visible(FALSE);
    }
	return S_OK;
}

HRESULT ShopUI::Item_Initialize() {
    //	m_pShopItem = new ItemINFO;
    //
    //	m_pShopItem->ItemDesc = {
    //		
    //			L"인내의 활",
    //			L"무기/희귀",
    //	
    //			L"",
    //			L"",
    //			L"",
    //	
    //			L"차징시간이 길수록 데미지가 증가합니다.",
    //	
    //			L"때를 기다려라... \n단 한 순간에 적의 숨통을 끊을 수 있도록..",
    //	
    //			L"",
    //			L"DIC_InfoFrame"
    //	};
    //	m_pShopItem->ItemPrice = 68;
    //	m_pShopItem->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;	
        //Display_ShopItemInfo(m_pShopItem);
    ItemINFO* pItem1 = new ItemINFO;
    pItem1->ItemDesc = { L"사도의 가호",L"소모품",L"",L"",L"",
                                            L"사도의 가호를 하나 충전합니다.",
                                            L"", L"20" };
    pItem1->ItemPrice = 20;
    pItem1->ItemType = (int)ITEM_TYPE::SUPPLY;
    Item_Index.push_back(pItem1);

    ItemINFO* pItem2 = new ItemINFO;
    pItem2->ItemDesc = { L"생명력", L"소모품", L"",L"",L"",
                                             L"잃은 체력을 한 칸 회복합니다.",
                                             L"", L"15" };
    pItem2->ItemPrice = 15;
    pItem2->ItemType = (int)ITEM_TYPE::SUPPLY;
    Item_Index.push_back(pItem2);

    ItemINFO* pItem3 = new ItemINFO;
    pItem3->ItemDesc = { L"화살 충전", L"소모품",L"",L"",L"",
                                            L"현재 사용중인 활의 화살을 전부 충전합니다.",
                                            L"", L"15" };
    pItem3->ItemPrice = 15;
    pItem3->ItemType = (int)ITEM_TYPE::SUPPLY;
    Item_Index.push_back(pItem3);

    ItemINFO* pItem4 = new ItemINFO;
    pItem4->ItemDesc = { L"빙결의 활", L"무기/희귀", L"일반 공격",
                                            L"일반 공격력 24 ~ 26", L"공격 속도 2.5",
                                            L"적에게 피해를 입힐 경우 \n적이 빙결 상태에 빠집니다.",
                                            L"얼어붙어라..", L"80" };

    pItem4->ItemPrice = 80;
    pItem4->ItemType = (int)ITEM_TYPE::RARE_WEAPON;
    Item_Index.push_back(pItem4);

    ItemINFO* pItem5 = new ItemINFO;
    pItem5->ItemDesc = { L"오동나무 활",L"무기/희귀",L"일반 공격",
                                            L"일반 공격력 24 ~ 26",L"공격 속도 2.5",
                                            L"가장 기본적인 활입니다.", L"이 활... 오동나무네?",
                                            L"0" };
    pItem5->ItemPrice = 0;
    pItem5->ItemType = (int)ITEM_TYPE::NORMAL_WEAPON;
    Item_Index.push_back(pItem5);

    ItemINFO* pItem6 = new ItemINFO;
    pItem6->ItemDesc = { L"어둠의 활",L"무기/희귀",L"일반 공격",
                                            L"일반 공격력 30 ~ 36",L"공격 속도 2.3",
                                            L"어둠의 힘을 담아 화살을 발사합니다.",L"어둠의 힘을 담아..",
                                            L"75" };
    pItem6->ItemPrice = 75;
    pItem6->ItemType = (int)ITEM_TYPE::RARE_WEAPON;
    Item_Index.push_back(pItem6);

    ItemINFO* pItem7 = new ItemINFO;
    pItem7->ItemDesc = { L"자연의 활",L"무기/희귀",L"일반 공격",
                                         L"일반 공격력 30 ~ 36", L"공격 속도 2.6",
                                         L"자연의 힘을 담아 화살을 발사합니다.", L"자연의 힘을 담아..",
                                         L"90" };
    pItem7->ItemPrice = 90;
    pItem7->ItemType = (int)ITEM_TYPE::RARE_WEAPON;
    Item_Index.push_back(pItem7);


    return S_OK;
}


void ShopUI::Show_Item()
{
    _vec3 vPos, vTilePos;
    TILE_SPAWNER eSpawn;
    dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Info(INFO_POS,&vPos);
    for (auto& iter : TileManager::GetInstance()->Get_TileList(TILE_STAGE4,TILEMODE_CHANGE::MODE_TILE))
    {
        eSpawn   = dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_Spawner();
        vTilePos = *dynamic_cast<Transform*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
 
        switch (eSpawn)
        {
        case TILE_SPAWNER::ITEM_SPAWN1:
           if (vPos.x > vTilePos.x - 1 && vPos.x < vTilePos.x + 1 && vPos.z > vTilePos.z - 1 && vPos.z < vTilePos.z + 1)
           {
               m_iCurrentItemIndex = 0;
               isActive = true;
               return;
           }
           else
               isActive = false;
           break;
        case TILE_SPAWNER::ITEM_SPAWN2:
            if (vPos.x > vTilePos.x - 1 && vPos.x < vTilePos.x + 1 && vPos.z > vTilePos.z - 1 && vPos.z < vTilePos.z + 1)
            {
                m_iCurrentItemIndex = 1;
                isActive = true;
                return;
            }
            else
                isActive = false;
            break;

        case TILE_SPAWNER::ITEM_SPAWN3:
            if (vPos.x > vTilePos.x - 1 && vPos.x < vTilePos.x + 1 && vPos.z > vTilePos.z - 1 && vPos.z < vTilePos.z + 1)
            {
                m_iCurrentItemIndex = 2;
                isActive = true;
                return;
            }
            else
                isActive = false;
            break;

        case TILE_SPAWNER::ITEM_SPAWN4:
            if (vPos.x > vTilePos.x - 1 && vPos.x < vTilePos.x + 1 && vPos.z > vTilePos.z - 1 && vPos.z < vTilePos.z + 1)
            {
                m_iCurrentItemIndex = 3;
                isActive = true;
                return;
            }
            else
                isActive = false;
            break;

        case TILE_SPAWNER::ITEM_SPAWN5:
            if (vPos.x > vTilePos.x - 1 && vPos.x < vTilePos.x + 1 && vPos.z > vTilePos.z - 1 && vPos.z < vTilePos.z + 1)
            {
                m_iCurrentItemIndex = 4;
                isActive = true;
                return;
            }
            else
                isActive = false;
            break;

        case TILE_SPAWNER::ITEM_SPAWN6:
            if (vPos.x > vTilePos.x - 1 && vPos.x < vTilePos.x + 1 && vPos.z > vTilePos.z - 1 && vPos.z < vTilePos.z + 1)
            {
                m_iCurrentItemIndex = 5;
                isActive = true;
                return;
            }
            else
                isActive = false;
            break;
        }
      
    }
}

BOOL ShopUI::Get_Collision_Enter(GameObject* _Other) {
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
	if(isActive)
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
			ItemInfo_Text[7]->Text = _pItem->ItemDesc[7];
	}
	
}

VOID	ShopUI::Free() {
    for (auto& II : Item_Index)
    {
        Safe_Release(II->TEXTURE);
            Safe_Delete(II);
    }
	GameObject::Free();
}