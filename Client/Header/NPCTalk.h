#pragma once
#include "GameObject.h"
#include "UIManager.h"

class NPCTalk : public GameObject {
private:
	explicit NPCTalk(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit NPCTalk(CONST GameObject& _RHS);
	virtual ~NPCTalk();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

	BOOL	Activate_NPCTalk(NPC_CHARACTER _NPCC, FLOAT _DT);
	BOOL	Shadow_Fade(CONST FLOAT& _DT);

public:
	HRESULT Component_Initialize();
	HRESULT Sprite_Initialize();
	HRESULT Effect_Initialize();
	HRESULT Text_Initialize();

	static	NPCTalk* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	enum TALKING { NOT_TALKING, TALKING_ACTIVE, QUEST_TALK01, QUEST_TALK02, QUEST_TALK03, QUEST_TALK04, QUEST_TALK_ACCEPT, QUEST_TALK_DENY, QUIT_TALK, QUEST_END};

private:
	SpriteObject* Component_Sprite;

	FLOAT		TalkTimer;
	INT			FadeState;
	_int		ContextPassing;

	BOOL		Enable_Interaction;

	SpriteINFO* BackGround;
	SpriteINFO* GRD_Top;
	SpriteINFO* GRD_Bottom;

	SpriteINFO* Sprite_Yeon;
	SpriteINFO* Sprite_Tif;
	SpriteINFO* Sprite_Shop;

	SpriteINFO* NameBar;
	SpriteINFO* Square;

	FontObject* Name;
	FontObject* Talk;

	Player*		PlayerObject;

	vector<SpriteINFO*> TALKOBJECT;
	vector<FontObject*> FOBJVec;

	BOOL	AcceptedQuest;
	BOOL	CompletedQuest;

private:
	virtual	VOID		Free();
};

