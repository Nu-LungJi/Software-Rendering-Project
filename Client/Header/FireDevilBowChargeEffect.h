#pragma once
#include "GameObject.h"

class FireDevilBowChargeEffect : public GameObject {
public:
	explicit FireDevilBowChargeEffect(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit FireDevilBowChargeEffect(const GameObject& _RHS);
	virtual ~FireDevilBowChargeEffect();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();

private:
	Buffer*		Component_Buffer;
	Transform*	Component_Transform;
	Collider*	Component_Collider;

public:
	//static FireDevilBowChargeEffect* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;

	VOID			Set_Master(GameObject* pOwner) { m_tInfo.pGameObj[0] = pOwner; }
	GameObject*		Get_Master() { return m_tInfo.pGameObj[0]; }

	VOID			Set_Dir(_vec3 vDir) { m_tInfo.vDirection = vDir; }
	VOID			Set_Dir(_float x, _float y, _float z) { m_tInfo.vDirection = { x,y,z }; }
	_vec3*			Get_Dir() { return &m_tInfo.vDirection; }

	MONBULLETINFO*	Get_Info() { return &m_tInfo; }

private:
	virtual VOID Free();

	MONBULLETINFO m_tInfo;

};