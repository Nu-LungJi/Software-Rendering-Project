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
	VOID			PopUp_Interaction_Notice(CONST TCHAR* _Text, BOOL _Vis);
	VOID			PopUp_ItemInfo(wstring ItemTag, FLOAT _DT);
	VOID			PopUp_Speech_Bubble(wstring _Text, FLOAT _DT);

	VOID			Speech_PopUp(wstring _Text) { Speech_Text = _Text; Enable_SpeechBubble = TRUE; }

	VOID			Set_FadeOption(INT _OPT) { Enable_MainUIFade = _OPT; }

	VOID			Set_EnableSpeechBubble(BOOL _ESB) {}


private:
	VOID			MainUI_FadeAction(CONST FLOAT& _DT);

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

	BOOL	ItemInfo;
	wstring ItemTag;

	INT		Current_KeyCount;
	INT		Current_CoinCount;
	INT		Current_CrystalCount;

	Player* PlayerObject;

	FLOAT	Timer01, Timer02, Timer03;

	FLOAT	MainUIOpacity;

	INT		Enable_MainUIFade;
  
	GUIVar				GuiVar;				// GUI용 변수
	wstring				ArrowCountText;		// 화살 카운트
	FontObject*			FO_ArrowCount;		// 화살 카운트
	int					Cur_BowIMGIDX;		// 현재 활 이미지 인덱스
	vector<SpriteINFO*> BowIMG_List;		// 활 스프라이트 모음
	

private:
	virtual	VOID		Free();
};

