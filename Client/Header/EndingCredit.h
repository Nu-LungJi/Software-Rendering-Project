#pragma once
#include "GameObject.h"

class EndingCredit : public GameObject
{
	explicit EndingCredit(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit EndingCredit(CONST GameObject& _RHS);
	virtual ~EndingCredit();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

public:
	HRESULT Component_Initialize();
	HRESULT Text_Initialize();
	HRESULT Sprite_Initialize();
	HRESULT	Update_Ready();

private:
	SpriteObject* Component_Sprite;
	vector<FontObject*>	FontList;

	SpriteINFO* _BlackScreen;
	SpriteINFO* _WhiteScreen;
	SpriteINFO* BackGround;
	SpriteINFO* BlackFont_Frame;
	SpriteINFO* Credit1;
	SpriteINFO* Credit2;
	

	// 참조 변수
	bool m_bCreditStart;

	// 변수
	BOOL	Start;
	FLOAT	POSX;
	FLOAT	POSY;
	FLOAT	FramePosX;
	FLOAT	FramePosY;
	FLOAT	WIDTH;
	FLOAT	HEIGHT;
	INT		Frame;
	INT		Timer;
	FLOAT	FrameTimer;
	FLOAT	FrameDelay;
	FLOAT	SizeRatio;
	FLOAT	MoveSpeed;
	FLOAT	FrameSpeed;
	FLOAT	WhiteScreenOpacity;
	FLOAT   Credit1_Timer;
	FLOAT   Credit2_Timer;

public:
	static	EndingCredit* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual	VOID		Free();


};

