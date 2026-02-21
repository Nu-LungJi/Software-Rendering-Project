#pragma once
#include "GameObject.h"

struct PerkINFO {
	enum PEINFO {KEY, NAME, DESC};

	LPDIRECT3DTEXTURE9 TEXTURE;

	vector<wstring> PerkDesc;

};

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

public:
	VOID Display_Perk_Font();
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

	vector<SpriteINFO> PerkInfo_Screen;
	vector<SpriteINFO*> PerkInfo_InfoFrame;

	BOOL isActive;

private:
	virtual	VOID		Free();
};