#pragma once
#include "GameObject.h"
class ShopKeeper : public GameObject
{
private:
	explicit ShopKeeper(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit ShopKeeper(const GameObject& _RHS);
	virtual ~ShopKeeper();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();

	virtual			BOOL		OnCollisionEnter(GameObject* _Other);
	virtual			BOOL		OnCollisionStay(GameObject* _Other);
	virtual			BOOL		OnCollisionExit(GameObject* _Other);

	VOID			TalkWithShopKeeper(FLOAT _DT);

private:
	HRESULT			Component_Initialize();

private:
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Texture* Component_Texture;
	Collider* Component_Collider;

	GameObject* Player;
	MainUI* PlayerUI;
	NPCTalk* NPCTalkUI;

	vector<IDirect3DTexture9*>	Shop_TextureList;

	_float  Timer_Shop;
	_int Shop_AnimIDX;


	BOOL	Interaction_Possible;

public:
	static ShopKeeper* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual VOID Free();
};
