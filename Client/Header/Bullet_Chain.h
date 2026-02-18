#pragma once
#include "GameObject.h"

class Bullet_Chain : public GameObject
{
public:
	explicit Bullet_Chain(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Bullet_Chain(CONST GameObject& _RHS);
	virtual ~Bullet_Chain();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

	static			Bullet_Chain* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	VOID			Free();
	HRESULT			Component_Initialize();

private:
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Collider* Component_Collider;

public:

	VOID			Set_Master(GameObject* pOwner) { pOwner = m_tInfo.pGameObj[0]; }
	GameObject*		Get_Master() { return m_tInfo.pGameObj[0]; }

	VOID			Set_Dir(_vec3 vDir) { m_tInfo.vDirection = vDir; }
	VOID			Set_Dir(_float x, _float y, _float z) { m_tInfo.vDirection = { x,y,z }; }
	_vec3*			Get_Dir() { return &m_tInfo.vDirection; }

	MONBULLETINFO*	Get_Info() { return &m_tInfo; }
private:
	MONBULLETINFO	m_tInfo;
};
