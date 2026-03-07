#pragma once
#include "GameObject.h"
class Player_Shadow : public GameObject
{
private:
	explicit Player_Shadow(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Player_Shadow(const GameObject& _RHS);
	virtual ~Player_Shadow();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();

	static Player_Shadow* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	HRESULT			Component_Initialize();
	VOID			SetGrahpic();

private:
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Texture* Component_Texture;

private:
	virtual VOID Free();

private:
	BOOL lateReady;
	Player* PlayerObject;
	_vec3* PlayerPos;
	FLOAT	AlphaRatio;
};

