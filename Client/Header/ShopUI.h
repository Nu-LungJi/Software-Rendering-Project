#pragma once
#include "GameObject.h"

class ShopUI : public GameObject {
private:
	explicit ShopUI(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit ShopUI(CONST GameObject& _RHS);
	virtual ~ShopUI();

public:
	virtual   HRESULT      Ready_GameObject();
	virtual INT         Update_GameObject(CONST FLOAT& _DT);
	virtual VOID      LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID      Render_GameObject();

public:
	HRESULT            Component_Initialize();
	HRESULT            Sprite_Initialize();
	HRESULT            Effect_Initialize();
	HRESULT            Text_Initialize();
	HRESULT            Item_Initialize();
	void			   Show_Item();
	_bool			   buy_Item(Player* pPlayer,_int iIndex);
public:
	static	ShopUI*		Create(LPDIRECT3DDEVICE9 _GRPDEV);
	VOID Display_ShopItemInfo(ItemINFO* _pItem); 

public:
	BOOL               Get_Collision_Enter(GameObject* _Other);
	BOOL               Get_Collision_Stay(GameObject* _Other);
	BOOL               Get_Collision_Exit(GameObject* _Other);
private:
	SpriteObject* Component_Sprite;
	Transform* Component_Transform;
	Collider* Component_Collider;
	Buffer* Component_Buffer;


	vector<SpriteINFO*> ItemInfo_Screen;

	vector<FontObject*>   ItemInfo_Text;
	vector<ItemINFO*>      Item_Index;
	INT      m_iCurrentItemIndex;

	BOOL         isActive;
	ItemINFO* m_pShopItem;

private:
	virtual   VOID      Free();
};
