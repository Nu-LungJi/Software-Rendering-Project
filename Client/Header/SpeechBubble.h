#pragma once
#include "GameObject.h"

class SpeechBubble : public GameObject
{
	explicit SpeechBubble(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit SpeechBubble(CONST GameObject& _RHS);
	virtual ~SpeechBubble();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

public:
	HRESULT Component_Initialize();
	static	SpeechBubble* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	HRESULT Sprite_Initialize();
	HRESULT Text_Initialize();

	BOOL Get_Active() { return isActive; }
	VOID Set_Active(BOOL _isActive) { isActive = _isActive; }

	void SetTarget(GameObject* pTarget) { m_pTarget = pTarget; }

	void			Set_SpeechPos(_vec3* _POS) { _ShopPos = _POS; }

private:
	SpriteObject* Component_Sprite;
	Transform* Component_Transform;
	GameObject* m_pTarget;
	_vec3* _ShopPos;

	BOOL					isActive;
private:
	virtual	VOID		Free();

};

