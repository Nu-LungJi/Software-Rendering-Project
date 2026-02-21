#pragma once
#include "GameObject.h"

class EvilSlimeGroundIce : public GameObject {
public:
	explicit EvilSlimeGroundIce(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit EvilSlimeGroundIce(const GameObject& _RHS);
	virtual ~EvilSlimeGroundIce();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Collider* Component_Collider;

public:
	//static EvilSlimeGroundIce* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;


	VOID			Set_Master(GameObject* pOwner) { m_tInfo.pGameObj[0] = pOwner; }
	GameObject*		Get_Master() { return m_tInfo.pGameObj[0]; }
	MONBULLETINFO*	Get_Info() { return &m_tInfo; }

private:
	virtual VOID Free();

	MONBULLETINFO m_tInfo;
};