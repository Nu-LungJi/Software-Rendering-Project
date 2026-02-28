#pragma once
#include "GameObject.h"
class Frog_Bullet :  public GameObject
{
public:
	explicit Frog_Bullet(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Frog_Bullet(CONST GameObject& _RHS);
	virtual ~Frog_Bullet();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(CONST FLOAT& _DT);
	virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual			VOID		Render_GameObject();

	static			Frog_Bullet* Create(LPDIRECT3DDEVICE9 _GRPDEV);

	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;

private:
	virtual VOID			Free();
	HRESULT			Component_Initialize();

private:
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Collider* Component_Collider;

public:


	VOID			Set_Master(GameObject* pOwner) { m_tInfo.pGameObj[0] = pOwner; }
	GameObject* Get_Master() { return m_tInfo.pGameObj[0]; }

	VOID			Set_Dir(_vec3 vDir) { m_tInfo.vDirection = vDir; }
	VOID			Set_Dir(_float x, _float y, _float z) { m_tInfo.vDirection = { x,y,z }; }
	_vec3* Get_Dir() { return &m_tInfo.vDirection; }

private:
	MONBULLETINFO	m_tInfo;
	_float			fDis;

};

