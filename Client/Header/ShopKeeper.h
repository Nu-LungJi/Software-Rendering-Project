#pragma once
#include "GameObject.h"
#include "MainUI.h"
#include "NPCTalk.h"
#include "SpeechBubble.h"

class ShopKeeper : public GameObject
{
public:
	explicit ShopKeeper(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit ShopKeeper(CONST GameObject& _RHS);

	virtual ~ShopKeeper();

public:
	virtual			HRESULT		Ready_GameObject(_vec3 vPos);

	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();

	virtual			BOOL		OnCollisionEnter(GameObject* _Other);
	virtual			BOOL		OnCollisionStay(GameObject* _Other);
	virtual			BOOL		OnCollisionExit(GameObject* _Other);

	VOID			TalkWithShopKeeper(FLOAT _DT);
	HRESULT			Make_TextureList(wstring _FileName);
public:
	static			ShopKeeper* Create(LPDIRECT3DDEVICE9 _GRPDEV,_vec3 vPos);

private:
	HRESULT			Component_Initialize();

private:
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Collider* Component_Collider;
	Texture* Component_Texture;

	GameObject* Player;
	MainUI* PlayerUI;
	NPCTalk* NPCTalkUI;
	SpeechBubble* Speech_BubbleUI;

	vector<IDirect3DTexture9*>	Shop_TextureList;

	_float  Timer_Shop;
	_int Shop_AnimIDX;

	BOOL	bQuest;
	BOOL	Interaction_Possible;

private:
	virtual VOID Free();
};