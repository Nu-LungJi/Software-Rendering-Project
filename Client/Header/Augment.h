#pragma once
#include "GameObject.h"

class Augment : public GameObject {
	explicit Augment(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Augment(CONST GameObject& _RHS);
	virtual ~Augment();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

	virtual			BOOL		OnCollisionEnter(GameObject* _Other);
	virtual			BOOL		OnCollisionStay(GameObject* _Other);
	virtual			BOOL		OnCollisionExit(GameObject* _Other);

public:
	HRESULT Component_Initialize();
	HRESULT Sprite_Initialize();
	HRESULT Text_Initialize();
	HRESULT	Perk_Initialize();
public:
	VOID Display_PerkInfo(ItemINFO* _pItem);
	BOOL IsMouseOnPerk();

public:
	static	Augment* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	SpriteObject* Component_Sprite;
	Texture* Component_Texture;
	Collider* Component_Collider;
	Transform* Component_Transform;

private:
	
	vector<ItemINFO*> Perk_Info;
	vector<FontObject*> Perk_Text;

	vector<SpriteINFO*> PerkInfo_Screen;
	vector<SpriteINFO*> PerkInfo_InfoFrame;
	
	_vec2 PerkInfo_Pos;

	BOOL isActive;
	
private:
	virtual	VOID		Free();
};