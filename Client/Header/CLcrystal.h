#pragma once
#include "GameObject.h"

class CLcrystal : public GameObject {
private:
	explicit CLcrystal(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit CLcrystal(const GameObject& _RHS);
	virtual ~CLcrystal();

public:
	virtual			HRESULT		Ready_GameObject() { return S_OK; }
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			void		LateUpdate_GameObject(const _float& _DT);
	virtual			void		Render_GameObject();

	virtual BOOL			 OnCollisionEnter(GameObject* _Other) { return 0; };
	virtual BOOL			 OnCollisionStay(GameObject* _Other) { return 0; };
	virtual BOOL			 OnCollisionExit(GameObject* _Other) { return 0; };
public:
	HRESULT					 Ready_Effect(_vec3 vPos);
	HRESULT					 Make_TextureList(wstring _FileName);

	static	CLcrystal* Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 vPos);



public:
	void				     Move_Frame(const _float& _DT);
	void					 Chek_CLPos();
private:
	HRESULT					 Component_Initialize();
	virtual	void			 Free();


private:
	vector<IDirect3DBaseTexture9*>	TextureList;
	INT								m_TextureIndex;
	FLOAT							m_FrameTick;

	Buffer* Component_Buffer;
	Transform* Component_Transform;

};
