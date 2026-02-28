#pragma once
#include"GameObject.h"
enum class BOSSUI_INFO {DOCHEOL, CHLG};

class BossUI : public GameObject
{
private:
	explicit BossUI(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit BossUI(CONST GameObject& _RHS);
	virtual ~BossUI();

public:
	virtual	HRESULT		Ready_GameObject(BOSSUI_INFO eid, GameObject* pObj);
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

public:
	VOID			    Set_HpInfo(GameObject* pObj) {
		Component_Collider = dynamic_cast<Collider*>(pObj->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER)); MaxHp = Component_Collider->Get_Hp();
		m_pObj = pObj;
	}
	VOID				Check_HpBar();
	VOID				Set_Dead() { m_bDead = true; }
public:
	HRESULT				Component_Initialize();
	static	BossUI*		Create(LPDIRECT3DDEVICE9 _GRPDEV, BOSSUI_INFO eid, GameObject* pObj);

private:
	_float				MaxHp;
	_float				CurHp;

private:
	_vec3				vScale;
	_bool				m_bDead;
	BOSSUI_INFO			m_eInfo;

	Transform*			Component_Transform;
	Buffer*				Componenet_Buffer;
	ID3DXSprite*		Sprite;
	SpriteINFO*			SpriteInfo[3];
		
	Collider*			Component_Collider;
	GameObject*			m_pObj;
private:
	virtual	VOID		Free();

};

