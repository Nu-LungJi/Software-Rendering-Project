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

	// ±¤À± Ãß°¡ ¡å
	VOID	Set_CreditStart(BOOL _Value)	{ m_bCreditStart = TRUE; }
	BOOL	Get_CreditStart()				{ return m_bCreditStart; }

private:
	SpriteObject* Component_Sprite;
	vector<SpriteINFO*> TextureList;
	vector<FontObject*>	FontObjectList;

	SpriteINFO* _BlackScreen;
	SpriteINFO* BackGround;
	

	// ÂüÁ¶ º¯¼ö
	bool m_bCreditStart;

	// º¯¼ö
	BOOL	Start;
	FLOAT	POSX;
	FLOAT	POSY;
	FLOAT	WIDTH;
	FLOAT	HEIGHT;
	INT		Frame;
	INT		Timer;
	FLOAT	FrameTimer;
	FLOAT	FrameDelay;
	FLOAT	SizeRatio;
	FLOAT	MoveSpeed;

public:
	static	EndingCredit* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual	VOID		Free();


};

