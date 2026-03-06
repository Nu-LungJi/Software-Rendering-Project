#pragma once
#include "GameObject.h"
#include "UIManager.h"

struct GUIVar {
	float POSX;
	float POSY;
	float WIDTHX;
	float WIDTHY;

	void	GUIInit(float posx, float posy, float widthx, float widthy) { POSX = posx; POSY = posy; WIDTHX = widthx; WIDTHY = widthy; }
};

class MainUI : public GameObject {
private:
	explicit MainUI(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit MainUI(CONST GameObject& _RHS);
	virtual ~MainUI();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual INT			Update_GameObject(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
	virtual VOID		Render_GameObject();

public:		// Trigger Function
	VOID			Player_LostHP();
	VOID			Player_ReFillHP(INT _HP);
	VOID			Player_KeyModify();
	VOID			Player_MoneyModify();
	VOID			Player_CrystalModify();
	VOID			Player_UseSkill();
	VOID			PopUp_Interaction_Notice(CONST TCHAR* _Text, BOOL _Vis) { Enable_Interaction = _Vis; Interaction_Text = _Text; }
	VOID			PopUp_Speech_Bubble(wstring _Text, FLOAT _DT);
	VOID			PopUp_Speech_Bubble_Skill(wstring _Text, FLOAT _DT, int type);

	VOID			Speech_PopUp(wstring _Text) { Speech_Text = _Text; Enable_SpeechBubble = TRUE; }
	VOID			Speech_PopUp_Skill(wstring _Text, int type) { Speech_Text = _Text; Enable_SpeechBubbleSkill = TRUE; ImgFrame = 1; FrameTimer = 0.f;  Effect = nullptr; skillType = type; }

	VOID			Set_FadeOption(INT _OPT, FLOAT _SPEED) { Enable_MainUIFade = _OPT; FadeSpeed = _SPEED; }

	VOID			Set_EnableSpeechBubble(BOOL _ESB) {}

	VOID			Set_BossMaxHP(FLOAT _HP) { MaxHP = _HP; }

	VOID			Set_EnableBossTitle(INT _EBT)		{ Enable_BossTitle = _EBT; }
	VOID			Set_EnableDisplayHPBar(BOOL _EDB)	{ Enable_DisplayHPBar = _EDB; }
	VOID			Set_EnableFade(BOOL _EDB)			{ Enable_FadeFilter = _EDB; }

	VOID			PopUp_ItemInfo(ItemINFO* Item, SpriteINFO* SPR, FLOAT _DT);
	VOID			Set_EnableItemPopUP(BOOL _POP) { Enable_ItemPopUp = _POP; }
	VOID			Set_PopUpItem(ItemINFO* _IT) { PopUpItem = _IT; }
	VOID			Set_PopUpSprite(SpriteINFO* _SPR) { PopUpSprite = _SPR; }

	VOID			Set_BossClearUI(BOOL _BCU) { Enable_BossClearUI = _BCU; }


	// 렐릭용
	VOID			Reset_Relic();
	VOID			Set_RelicIcon();

	vector<FontObject*> Get_AllFontObject() { return AllFontOBJ; }
private:
	VOID			Display_InteractionUI();
	VOID			MainUI_FadeAction(CONST FLOAT& _DT, FLOAT _SPEED);
	VOID			Display_BossTitle(CONST FLOAT& _DT);
	VOID			Display_BossHPBar(CONST FLOAT& _DT);
	VOID			Display_FadeFilter(CONST FLOAT& _DT);
	VOID			Synchronize_BossHPBar();
	VOID			Display_ClearBossUI(CONST FLOAT& _DT);
public:
	HRESULT Component_Initialize();
	HRESULT Sprite_Initialize();
	HRESULT Effect_Initialize();
	HRESULT	Text_Initialize();

	static	MainUI*		Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	void                        Imgui_Setting();
	void                        Imgui();
	void                        Imgui_ButtonStyle();
private:
	SpriteObject*		Component_Sprite;
	vector<SpriteINFO>*	TextureList;

	BOOL	Enable_SpeechBubble;
	wstring Speech_Text;
	INT		Enable_Interaction;
	wstring Interaction_Text;

	INT		Current_KeyCount;
	INT		Current_CoinCount;
	INT		Current_CrystalCount;

	Player* PlayerObject;

	FLOAT	Timer01, Timer02, Timer03;

	FLOAT	MainUIOpacity;

	BOOL	Enable_BossClearUI;
	FLOAT	BossClearTimer;
	BOOL	BossClear[10];

	INT				Enable_MainUIFade;
	BOOL			Enable_ItemPopUp;
	FLOAT			ItemPopUp_Timer;
	ItemINFO*		PopUpItem;
	SpriteINFO*		PopUpSprite;
  
	GUIVar				GuiVar;				// GUI용 변수
	wstring				ArrowCountText;		// 화살 카운트
	FontObject*			FO_ArrowCount;		// 화살 카운트
	int					Cur_BowIMGIDX;		// 현재 활 이미지 인덱스
	vector<SpriteINFO*> BowIMG_List;		// 활 스프라이트 모음

////////////RelicEffect//
	vector<SpriteINFO*> relicEffectList;
	int					Cur_RelicIMGIDX;

////////////////////////////////////////////// 보스전 활용 변수들
	vector<FontObject*> AllFontOBJ;
	vector<SpriteINFO*> AllSpriteOBJ;
	vector<UIEffect*>	AllUIEffect;
	FLOAT				GlobalOPC;	
	INT					EffectFaded;
	FLOAT				FadeSpeed;

	FLOAT				MaxHP;
	FLOAT				CurrentHP;

	INT					Enable_DisplayHPBar;
	FLOAT				HPOPC;
	_vec3				BarScale;
	SpriteINFO*			HPBarFill;
	SpriteINFO*			BossTitleBar;
	ID3DXSprite*		BossHPSprite;

	INT					Enable_BossTitle;
	FLOAT				BossTitleTimer;
	FontObject*			Title_Name;
	FontObject*			Title_Tag;
//////////////////////////////////////////////
////////////////////////////////////////////// 스킬용 애니메이션
	INT				ImgFrame;
	FLOAT			FrameTimer;
	SpriteINFO*		Effect;
	BOOL			Enable_SpeechBubbleSkill;
	INT				skillType;
//////////////////////////////////////////////
////////////////////////////////////////////// 인트로, 엔딩
	INT				Enable_FadeFilter;
	FLOAT			FadeOPC;
//////////////////////////////////////////////



private:
	virtual	VOID		Free();
};

