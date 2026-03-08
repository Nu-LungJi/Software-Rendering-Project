#pragma once
#include "GameObject.h"
#include "UIManager.h"


class IntroUI : public GameObject {
	explicit IntroUI(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit IntroUI(CONST GameObject& _RHS);
	virtual ~IntroUI();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

public:
	HRESULT Component_Initialize();
	HRESULT Sprite_Initialize();
	HRESULT Text_Initialize();

	static	IntroUI*	Create(LPDIRECT3DDEVICE9 _GRPDEV);

	VOID	Start_FadeIn(CONST FLOAT& _DT);
	VOID	Start_FadeOut(CONST FLOAT& _DT);

private:
	virtual	VOID		Free();

private:
	SpriteObject*	Component_Sprite;
	ID3DXSprite*	IntroSprite;
	BOOL	Enable_FadeIn, Enable_FadeOut, Enable_MenuBar, Enable_GameStart, Enable_ClickToStart;

	FontObject*		StartBTN;
	SpriteINFO*		Filter;
	FLOAT			FadeTimer;
};

