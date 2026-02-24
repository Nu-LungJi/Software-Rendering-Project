#pragma once
#include "GameObject.h"

class Augment : public GameObject {
private:
	explicit Augment(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Augment(CONST GameObject& _RHS);
	virtual ~Augment();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

	enum PERK {INIT ,FIRST, SECOND, THIRD};

public:
	HRESULT Component_Initialize();
	HRESULT Sprite_Initialize();
	HRESULT Text_Initialize();
	HRESULT	Perk_Initialize();
	HRESULT Effect_Initialize();

public:
	HRESULT Add_PlayerStatus(INT _PerkType);

	VOID Display_PerkInfo(ItemINFO* _pItem);
	VOID Perk_Selected_Effect(INT _PerkType);
	
public:
	INT IsMouseOnPerk();

public:
	static	Augment* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	SpriteObject* Component_Sprite;
	Texture* Component_Texture;
	Transform* Component_Transform;

private:
	
	FLOAT Screen_Timer;
	FLOAT Anim_Timer;

	FLOAT TargetX;

	vector<SpriteINFO*>	Anim_PerkSelect;

	_uint Frame;
	_uint FrameTick;

	vector<SpriteINFO*> Perk_Pos;

	vector<ItemINFO*> Perk_Info;
	vector<FontObject*> Perk_Text;

	vector<SpriteINFO*> PerkInfo_Screen;
	vector<SpriteINFO*> PerkInfo_InfoFrame;
	
	_vec2 PerkInfo_Pos;
	Player* PlayerObject;

  INT m_iPrevHoverType;

	BOOL isActive;
	
	vector<EffectINFO*> PerkEffectList;

private:
	virtual	VOID		Free();
};