#pragma once
#include"GameObject.h"

enum HurdleType
{
	Hurdle_Pedestal = 1,
	Hurdle_DropItem = 2,
	End,
};

class Hurdle : public GameObject
{
public:
	explicit Hurdle(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Hurdle(const GameObject& _RHS);
	virtual ~Hurdle();

public:
	static	Hurdle* Create(LPDIRECT3DDEVICE9 _GRPDEV, HurdleType eType = Hurdle_Pedestal, _vec3 vPos = { 0.f,0.f,0.f }, _vec3 vScale = {0.f,0.f,0.f});

	HRESULT			Ready_GameObject(HurdleType eType, _vec3 vPos, _vec3 vScale);
	INT				Update_GameObject(const _float& _DT)		override;
	VOID			LateUpdate_GameObject(const _float& _DT)	override;
	VOID			Render_GameObject()							override;
	HRESULT			Component_Initiaize();	


	BOOL			OnCollisionStay(GameObject* _Other)		override;

	void			Set_bMoved(bool b) { m_bMoved = b; }

private:
	VOID			Free();

	Buffer*				Component_Buffer;
	Transform*			Component_Transform;
	Collider*			Component_Collider;
	IDirect3DTexture9*	m_pTexture;
	bool				m_bMoved = false;

private:
	CameraObject*		m_pCam;
	MONSTERINFO			m_tInfo;
};