#pragma once
#include "GameObject.h"

class ShopKeeper : public GameObject
{
public:
	explicit ShopKeeper(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit ShopKeeper(CONST GameObject& _RHS);

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
	Collider* Component_Collider;
	Texture* Component_Texture;

public:
	static			ShopKeeper* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	BOOL			OnCollisionEnter(GameObject* _Other)	override;
	BOOL			OnCollisionStay(GameObject* _Other)		override;
	BOOL			OnCollisionExit(GameObject* _Other)		override;

	GameObject* Player;
	MainUI* PlayerUI;
	NPCTalk* NPCTalkUI;
	SpeechBubble* Speech_BubbleUI;

	vector<IDirect3DTexture9*>	Shop_TextureList;

	_float  Timer_Shop;
	_int Shop_AnimIDX;


	BOOL	Interaction_Possible;

public:
	static ShopKeeper* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual VOID Free();
};