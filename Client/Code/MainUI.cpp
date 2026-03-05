#include "../Include/PCH.h"
#include "MainUI.h"

MainUI::MainUI(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
MainUI::MainUI(CONST GameObject& _RHS)		: GameObject(_RHS)		{}
MainUI::~MainUI()													{}

HRESULT	MainUI::Ready_GameObject() {
	if (FAILED(Component_Initialize()))		return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Effect_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;
	
	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));
	D3DXCreateSprite(GRPDEV, &BossHPSprite);

	Enable_SpeechBubble = FALSE;
	Speech_Text = L"";
	ArrowCountText = L"";
	Timer01 = 0.f; Timer02 = 0.f; Timer03 = 0.f;
	Enable_Interaction = 2;
	Interaction_Text = L"";

	MainUIOpacity = 0.f;
	
	Current_KeyCount		= 0;
	Current_CoinCount		= 0;
	Current_CrystalCount	= 0;
	Cur_BowIMGIDX			= 0;

	GuiVar.GUIInit(1166.f, 580.f, 100, 100);

	GlobalOPC = 255;
	Enable_MainUIFade = 3;
	EffectFaded = FALSE;
	BarScale = { 1.f, 1.f, 1.f };
	MaxHP = 0.f; CurrentHP = 0.f;
	Enable_BossTitle = 2;
	BossTitleTimer = 0.f;
	BarScale = { 1.f, 1.f, 1.f };
	Enable_DisplayHPBar = 2;
	HPOPC = 0.f;

	memset(BossClear, TRUE, sizeof(BossClear));

	Enable_FadeFilter = 2;
	FadeOPC = UIManager::GetInstance()->Find_FilterObjects(L"FadeFilter")->OPACITY;

	PopUpItem = nullptr;
	Enable_BossClearUI = FALSE;
	BossClearTimer = 0.f;

	return S_OK;
}
INT		MainUI::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	
	PopUp_Speech_Bubble(Speech_Text, _DT);
	PopUp_Speech_Bubble_Skill(Speech_Text, _DT, skillType);
  
	//PopUp_Speech_Bubble(ItemTag, _DT);
	Timer02 += _DT; 
	if(Timer02 > 0.04f){
		Player_KeyModify();
		Player_MoneyModify();
		Player_CrystalModify();
		Timer02 = 0.f;
	}
	PopUp_ItemInfo(PopUpItem, PopUpSprite, _DT);
	MainUI_FadeAction(_DT, FadeSpeed);
	Synchronize_BossHPBar();

	Display_InteractionUI();

	Display_BossTitle(_DT);
	Display_BossHPBar(_DT);
	Display_FadeFilter(_DT);
	Display_ClearBossUI(_DT);

	ArrowCountText = to_wstring(PlayerObject->Get_CurArrowCount()) + L" / " +  to_wstring(PlayerObject->Get_MaxArrow());
	FO_ArrowCount->Set_Text(ArrowCountText);

	int cur_Equip_BowIDX = PlayerObject->Get_Bow_ImgIDX();
	if (Cur_BowIMGIDX != cur_Equip_BowIDX) {
		BowIMG_List[Cur_BowIMGIDX]->Set_Visible(false);
		BowIMG_List[cur_Equip_BowIDX]->Set_Visible(true);
		Cur_BowIMGIDX = cur_Equip_BowIDX;
	}

	return 0;
}
VOID	MainUI::LateUpdate_GameObject(CONST FLOAT& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
	if (KEY_DOWN(DIK_J)) {
		Set_BossClearUI(TRUE);
	}
}
VOID	MainUI::Render_GameObject() {
	_matrix matWorld, matScale, matTrans;

	GRPDEV->GetTransform(D3DTS_WORLD, &matWorld);

	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixTranslation(&matTrans, HPBarFill->POS.x + (1.f - BarScale.x) * 640.f / 4.f, HPBarFill->POS.y, 0);
	D3DXMatrixScaling(&matScale, BarScale.x, BarScale.y, BarScale.z);

	matWorld = matScale * matTrans;
	BossHPSprite->SetTransform(&matWorld);
	
	BossHPSprite->Begin(D3DXSPRITE_ALPHABLEND);

	BossHPSprite->Draw(HPBarFill->TEXTURE, NULL, NULL, &HPBarFill->POS, D3DCOLOR_ARGB(HPBarFill->OPACITY, 255, 255, 255));

	D3DXMatrixIdentity(&matWorld);
	BossHPSprite->SetTransform(&matWorld);

	BossHPSprite->Draw(BossTitleBar->TEXTURE, NULL, NULL, &BossTitleBar->POS, D3DCOLOR_ARGB(BossTitleBar->OPACITY, 255, 255, 255));

	BossHPSprite->End();
	Component_Sprite->Render_Sprite();
}

VOID MainUI::Player_LostHP() {
	INT PlayerHP = COLLIDER(PlayerObject)->Get_Hp();
	if (PlayerHP > 0 && !PlayerObject->Get_Invincible()) {
		if (PlayerHP > 5) { PlayerHP = 5; }
		wstring UIKey_HP = L"HP_EFFECT" + to_wstring(PlayerHP);
		dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"))->Set_HP(PlayerHP - 1);
		dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"))->Set_Invincible(true);
		REPLAY_UI_EFFECT(UIKey_HP);

		UIKey_HP = L"HP_SPRITE" + to_wstring(PlayerHP);
		Component_Sprite->Get_Texture(UIKey_HP)->Set_Visible(FALSE);

		UIKey_HP = L"EHP_SPRITE" + to_wstring(PlayerHP);
		Component_Sprite->Get_Texture(UIKey_HP)->Set_Visible(TRUE);

		SoundManager::GetInstance()->Play_Sound_Once(L"Player/Haetae_Attacked.wav", CHANNELID::SOUND_EFFECT07, 0.35f);

		CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));
		Camera->Camera_Shaking(30, 5.f);
	}
}
VOID MainUI::Player_ReFillHP(INT _HP) {
	INT PlayerHP = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"))->Get_HP();
	INT healHP = PlayerHP + _HP;

	if (healHP > 5) healHP = 5;

	for (int i = PlayerHP + 1; i <= healHP; i++) {
		wstring UIKey_HP = L"HP_EFFECT" + to_wstring(i);

		UIKey_HP = L"HP_SPRITE" + to_wstring(i);
		Component_Sprite->Get_Texture(UIKey_HP)->Set_Visible(TRUE);

		UIKey_HP = L"EHP_SPRITE" + to_wstring(i);
		Component_Sprite->Get_Texture(UIKey_HP)->Set_Visible(FALSE);
	}
	dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"))->Set_HP(healHP);
}
VOID MainUI::Player_KeyModify() {
	if (Current_KeyCount != PlayerObject->Get_Key()) {
		FontObject* Font = UIManager::GetInstance()->Find_FontObject(L"KeyCountText");
		if(Current_KeyCount < PlayerObject->Get_Key())
			Font->Set_Text(to_wstring(++Current_KeyCount));
		else
			Font->Set_Text(to_wstring(--Current_KeyCount));
	}
}
VOID MainUI::Player_MoneyModify() {
	if (Current_CoinCount != PlayerObject->Get_Coin()) {
		FontObject* Font = UIManager::GetInstance()->Find_FontObject(L"CoinCountText");
		if (Current_CoinCount < PlayerObject->Get_Coin())
			Font->Set_Text(to_wstring(++Current_CoinCount));
		else
			Font->Set_Text(to_wstring(--Current_CoinCount));
	}
}
VOID MainUI::Player_CrystalModify() {
	if (Current_CrystalCount != PlayerObject->Get_Crystal()) {
		FontObject* Font = UIManager::GetInstance()->Find_FontObject(L"CrystalCountText");
		if (Current_CrystalCount < PlayerObject->Get_Crystal())
			Font->Set_Text(to_wstring(++Current_CrystalCount));
		else
			Font->Set_Text(to_wstring(--Current_CrystalCount));
	}
}
VOID MainUI::Player_UseSkill() {
	INT PlayerToken = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"))->Get_Token();
	wstring UIKey_HP = L"TOKEN_EFFECT" + to_wstring(PlayerToken);
	if (PlayerToken > 0) {
		dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"))->Set_Token(PlayerToken - 1);
		REPLAY_UI_EFFECT(UIKey_HP);

		UIKey_HP = L"Token" + to_wstring(PlayerToken);
		//Component_Sprite->Get_Texture(UIKey_HP)->Set_Visible(FALSE);
	}
}

VOID MainUI::PopUp_Speech_Bubble(wstring _Text, FLOAT _DT) {
	if (Enable_SpeechBubble) {
		SpriteINFO* BackGround = Component_Sprite->Get_Texture(L"SpeechBubble_BG");
		SpriteINFO* Frame = Component_Sprite->Get_Texture(L"SpeechBubble_Frame");
		SpriteINFO* Character = Component_Sprite->Get_Texture(L"SpeechBubble_Tif");
		FontObject* Font = UIManager::GetInstance()->Find_FontObject(L"TifNotice_Text");

		Font->Text = _Text;
		if (Timer01 < 1.f) {
			Timer01 += _DT;

			BackGround->Set_Pos(BackGround->Get_Pos().x, BackGround->Get_Pos().y - Timer01 * 2 * cosf(Timer01));
			Frame->Set_Pos(Frame->Get_Pos().x, Frame->Get_Pos().y - Timer01 * 2 * cosf(Timer01));
			Character->Set_Pos(Character->Get_Pos().x, Character->Get_Pos().y - Timer01 * 2 * cosf(Timer01));
			//Effect->Set_Pos(Frame->Get_Pos().x, Frame->Get_Pos().y - Timer01 * 2 * cosf(Timer01) + 10.f);
			Font->Set_Pos( Font->Get_Pos().x, Font->Get_Pos().y - Timer01 * 2 * cosf(Timer01) );
			
			if (Timer01 < 1.f) {
				BackGround->Set_Opacity(Timer01 * 255);
				Frame->Set_Opacity(Timer01 * 255);
				Character->Set_Opacity(Timer01 * 255);
				//Effect->Set_Opacity(Timer01 * 255);
				Font->Set_Color(Timer01 * 200, 255, 255, 255);
			}
			else {
				BackGround->Set_Opacity(255);
				Frame->Set_Opacity(255);
				Character->Set_Opacity(255);
				//Effect->Set_Opacity(255);
				Font->Set_Color(200, 255, 255, 255);
			}
		}
		else if (Timer01 >= 1.f && Timer01 < 6.f) {
			Timer01 += _DT;
		}
		else if (Timer01 >= 6.f && Timer01 < 7.f) {
			Timer01 += _DT;

			BackGround->Set_Pos(BackGround->Get_Pos().x, BackGround->Get_Pos().y + (Timer01 - 6) * 3);
			Frame->Set_Pos(Frame->Get_Pos().x, Frame->Get_Pos().y + (Timer01 - 6) * 3);
			Character->Set_Pos(Character->Get_Pos().x, Character->Get_Pos().y + (Timer01 - 6) * 3);
			Font->Set_Pos( Font->Get_Pos().x, Font->Get_Pos().y + (Timer01 - 6) * 3 );

			if (Timer01 < 7.f) {
				BackGround->Set_Opacity(255 - 255 * (Timer01 - 6));
				Frame->Set_Opacity(255 - 255 * (Timer01 - 6));
				Character->Set_Opacity(255 - 255 * (Timer01 - 6));
				Font->Set_Color(200 - 200 * (Timer01 - 6), 255, 255, 255);
			}
			else {
				BackGround->Set_Opacity(0);
				Frame->Set_Opacity(0);
				Character->Set_Opacity(0);
				Font->Set_Color(0, 255, 255, 255);
			}
		}
		else if (Timer01 >= 7.f) {
			Timer01 = 0;
			Enable_SpeechBubble = FALSE;

			BackGround->Set_Opacity(0);
			Frame->Set_Opacity(0);
			Character->Set_Opacity(0);
			Font->Set_Color(0, 255, 255, 255);

			BackGround->Set_Pos(0.f, 529.f + 30.f);
			Character->Set_Pos(0.f, 529.f + 30.f);
			Frame->Set_Pos(0.f, 503.f + 30.f);
			Font->Set_Pos( 180.f, 555.f + 30.f );
		}
	}
}
VOID MainUI::PopUp_Speech_Bubble_Skill(wstring _Text, FLOAT _DT, int type)
{
	if (Enable_SpeechBubbleSkill) {
		SpriteINFO* BackGround = Component_Sprite->Get_Texture(L"SpeechBubble_BG");
		SpriteINFO* Frame = Component_Sprite->Get_Texture(L"SpeechBubble_Frame");
		SpriteINFO* Character = nullptr;
		FontObject* Font = UIManager::GetInstance()->Find_FontObject(L"TifNotice_Text");

		if(type == 0)
			Character = Component_Sprite->Get_Texture(L"SpeechBubble_TimeStop");
		else if(type == 1)
			Character = Component_Sprite->Get_Texture(L"SpeechBubble_Angry");

		// 이펙트
		FrameTimer += _DT;
		if (Effect != nullptr)
			if (FrameTimer > 0.1) {
				Effect->Set_Opacity(0);
				ImgFrame++;
				if (ImgFrame > 19) ImgFrame = 1;
			}
		TCHAR FileName[128] = L"";
		wsprintfW(FileName, L"BubblEffect_%d.png", ImgFrame);
		Effect = Component_Sprite->Get_Texture(FileName);

		Font->Text = _Text;
		if (Timer01 < 1.f) {
			Timer01 += _DT;

			BackGround->Set_Pos(BackGround->Get_Pos().x, BackGround->Get_Pos().y - Timer01 * 2 * cosf(Timer01));
			Frame->Set_Pos(Frame->Get_Pos().x, Frame->Get_Pos().y - Timer01 * 2 * cosf(Timer01));
			Character->Set_Pos(Character->Get_Pos().x, Character->Get_Pos().y - Timer01 * 2 * cosf(Timer01));
			Effect->Set_Pos(Frame->Get_Pos().x, Frame->Get_Pos().y - Timer01 * 2 * cosf(Timer01) + 17.f);
			Font->Set_Pos(Font->Get_Pos().x, Font->Get_Pos().y - Timer01 * 2 * cosf(Timer01));

			if (Timer01 < 1.f) {
				BackGround->Set_Opacity(Timer01 * 255);
				Frame->Set_Opacity(Timer01 * 255);
				Character->Set_Opacity(Timer01 * 255);
				Effect->Set_Opacity(Timer01 * 255);
				Font->Set_Color(Timer01 * 200, 255, 255, 255);
			}
			else {
				BackGround->Set_Opacity(255);
				Frame->Set_Opacity(255);
				Character->Set_Opacity(255);
				Effect->Set_Opacity(255);
				Font->Set_Color(200, 255, 255, 255);
			}
		}
		else if (Timer01 >= 1.f && Timer01 < 4.f) {
			Timer01 += _DT;
			Effect->Set_Opacity(255);
			Effect->Set_Pos(Frame->Get_Pos().x, Frame->Get_Pos().y - 2 * cosf(Timer01) + 17.f);
		}
		else if (Timer01 >= 4.f && Timer01 < 5.f) {
			Timer01 += _DT;

			BackGround->Set_Pos(BackGround->Get_Pos().x, BackGround->Get_Pos().y + (Timer01 - 4) * 3);
			Frame->Set_Pos(Frame->Get_Pos().x, Frame->Get_Pos().y + (Timer01 - 4) * 3);
			Character->Set_Pos(Character->Get_Pos().x, Character->Get_Pos().y + (Timer01 - 4) * 3);
			Effect->Set_Pos(Character->Get_Pos().x, Character->Get_Pos().y + (Timer01 - 4) * 3 + 17.f);
			Font->Set_Pos(Font->Get_Pos().x, Font->Get_Pos().y + (Timer01 - 4) * 3);

			if (Timer01 < 5.f) {
				BackGround->Set_Opacity(255 - 255 * (Timer01 - 4));
				Frame->Set_Opacity(255 - 255 * (Timer01 - 4));
				Character->Set_Opacity(255 - 255 * (Timer01 - 4));
				//Effect->Set_Opacity(255 - 255 * (Timer01 - 6));
				Effect->Set_Opacity(0);
				Font->Set_Color(200 - 200 * (Timer01 - 4), 255, 255, 255);
			}
			else {
				BackGround->Set_Opacity(0);
				Frame->Set_Opacity(0);
				Character->Set_Opacity(0);
				Effect->Set_Opacity(0);
				Font->Set_Color(0, 255, 255, 255);
			}
		}
		else if (Timer01 >= 5.f) {
			Timer01 = 0;
			Enable_SpeechBubbleSkill = FALSE;

			BackGround->Set_Opacity(0);
			Frame->Set_Opacity(0);
			Character->Set_Opacity(0);
			Effect->Set_Opacity(0);
			Font->Set_Color(0, 255, 255, 255);

			BackGround->Set_Pos(0.f, 529.f + 30.f);
			Character->Set_Pos(0.f, 529.f + 30.f);
			Frame->Set_Pos(0.f, 503.f + 30.f);
			Effect->Set_Pos(0.f, 503.f + 30.f);
			Font->Set_Pos(180.f, 555.f + 30.f);
		}
	}
}

VOID MainUI::Display_InteractionUI() {
	if (Enable_Interaction == 2) return;

	SpriteINFO* KeyBoard = Component_Sprite->Get_Texture(L"KEY_E");
	SpriteINFO* InterBG = Component_Sprite->Get_Texture(L"Interaction_BG");
	FontObject* FO = UIManager::GetInstance()->Find_FontObject(L"Interaction_Text");

	FO->Set_Text(Interaction_Text.c_str());
	if (Enable_Interaction == TRUE) {
		KeyBoard->Set_Visible(TRUE);
		InterBG->Set_Visible(TRUE);

		FO->Set_Visible(TRUE);

		Enable_Interaction = 2;
	}
	else if (Enable_Interaction == FALSE) {
		KeyBoard->Set_Visible(FALSE);
		InterBG->Set_Visible(FALSE);

		FO->Set_Visible(FALSE);

		Enable_Interaction = 2;
	}
}

VOID MainUI::MainUI_FadeAction(CONST FLOAT& _DT, FLOAT _SPEED) {
	if		(Enable_MainUIFade == 2)		return;
	else if (Enable_MainUIFade == TRUE) {
		if (GlobalOPC > 2.f)	GlobalOPC -= _DT * 255.f / _SPEED;
		else					GlobalOPC = 0;
	
		for (auto& FO : AllFontOBJ) 
			FO->TextColor = D3DCOLOR_ARGB((INT)GlobalOPC, 255, 255, 255);
	
		for (auto& SO : AllSpriteOBJ)
			SO->Set_Opacity((INT)GlobalOPC);

		if (EffectFaded == FALSE) {
			for (auto& AUE : AllUIEffect) {
				AUE->Set_EffectFadeOption(TRUE);
			}
			EffectFaded = TRUE;
		}
	}
	else if (Enable_MainUIFade == FALSE) {
		if (Component_Sprite->Get_Texture(L"HPBar_Frame")->VISIBLE == FALSE) {
			Component_Sprite->Get_Texture(L"HPBar_Frame")->VISIBLE = TRUE;
			HPBarFill->VISIBLE = TRUE;
		}
		if (GlobalOPC < 253.f) GlobalOPC += _DT * 255.f / _SPEED;
		else {
			GlobalOPC = 255;
			Enable_MainUIFade = 2;
		}
		for (auto& FO : AllFontOBJ) {
			HPBarFill->Set_Opacity((INT)GlobalOPC);
			FO->TextColor = D3DCOLOR_ARGB((INT)GlobalOPC, 255, 255, 255);
		}
			
		for (auto& SO : AllSpriteOBJ) 
			SO->Set_Opacity((INT)GlobalOPC);

		if (EffectFaded == TRUE) {
			for (auto& AUE : AllUIEffect) {
				AUE->Set_EffectFadeOption(FALSE);
			}
			EffectFaded = FALSE;
		}
		
	}
	else if (Enable_MainUIFade == 3) {		// Vector Initialize
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"HP_BG"		));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"TEARDROP_BG"	));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"KEY_BG"		));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"COIN_BG"		));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"CRYSTAL_BG"	));

		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"HP_SPRITE1"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"HP_SPRITE2"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"HP_SPRITE3"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"HP_SPRITE4"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"HP_SPRITE5"));

		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"EHP_SPRITE1"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"EHP_SPRITE2"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"EHP_SPRITE3"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"EHP_SPRITE4"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"EHP_SPRITE5"));

		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"CRYSTAL"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"KEY"));

		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"SkillState_BG"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"SkillState_Frame"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"SkillState_Symbol_OFF"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"SkillState_SkillOn"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"SkillState_Symbol_ON"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"KEY_Q"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"Token1"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"Token2"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"WeaponBG_Arrow"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"WeaponBG_ArrowCount"				));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"FairyBow_IMG"				));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"EvilHeadBow_IMG"				));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"FairyBow_IMG"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"IRABow_IMG"));
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"HPBar_Frame"));
		
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"HP_EFFECT1")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"HP_EFFECT2")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"HP_EFFECT3")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"HP_EFFECT4")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"HP_EFFECT5")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"DASHSTOCK_EFFECT1")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"DASHSTOCK_EFFECT2")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"DASHSTOCK_EFFECT3")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"COIN_EFFECT")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"TOKEN_EFFECT1")));
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"TOKEN_EFFECT2")));

		Enable_MainUIFade = 2;								
	}														
}

VOID MainUI::Display_BossTitle(CONST FLOAT& _DT) {
	if (Enable_BossTitle == TRUE) {
		if (BossTitleTimer < 254.f) {
			BossTitleTimer += _DT * 255;
			BossTitleBar->Set_Opacity(BossTitleTimer);
			Title_Name->Set_Color(BossTitleTimer, 255, 255, 255);
			Title_Tag->Set_Color(BossTitleTimer, 255, 255, 255);
		}
		else {
			BossTitleTimer = 255.f;
			BossTitleBar->Set_Opacity(255);
			Title_Name->Set_Color(255, 255, 255, 255);
			Title_Tag->Set_Color(255, 255, 255, 255);
		}
		if (BossTitleTimer <= 10.f && TileManager::GetInstance()->Get_Stage() == TILE_FIRSTBOSS) {
			UIManager::GetInstance()->Find_FontObject(L"Boss_Title_Name")->Set_Text(L"라 우 라");
			UIManager::GetInstance()->Find_FontObject(L"Boss_Title_Tag")->Set_Text(L"타락한 자연의 사도");
		}
		else if (BossTitleTimer <= 10.f && TileManager::GetInstance()->Get_Stage() == TILE_DOCHERBOSS) {
			UIManager::GetInstance()->Find_FontObject(L"Boss_Title_Name")->Set_Text(L"도 철");
			UIManager::GetInstance()->Find_FontObject(L"Boss_Title_Tag")->Set_Text(L"분노의 거대 사념체");
		}
	}
	else if (Enable_BossTitle == FALSE) {
		if (BossTitleBar->OPACITY > 3 && BossTitleTimer > 1.f) {
			BossTitleTimer -= _DT * 255;
			BossTitleBar->Set_Opacity(BossTitleTimer);
			Title_Name->Set_Color(BossTitleTimer, 255, 255, 255);
			Title_Tag->Set_Color(BossTitleTimer, 255, 255, 255);
		}
		else {
			BossTitleBar->Set_Opacity(0);
			Title_Name->Set_Color(0, 255, 255, 255);
			Title_Tag->Set_Color(0, 255, 255, 255);
			Enable_BossTitle = 2;
			BossTitleTimer = 0.f;
		}
	}
}
VOID MainUI::Display_BossHPBar(CONST FLOAT& _DT) {
	if		(Enable_DisplayHPBar == 2) return;
	else if (Enable_DisplayHPBar == TRUE) {		// 쓸일 없음
		if	(HPOPC < 253.f)	{ HPOPC += _DT * 255.f / 2.f; }
		else				{ HPOPC = 255.f; Enable_DisplayHPBar = 2;}

		if (TileManager::GetInstance()->Get_Stage() == TILE_FIRSTBOSS) {
			UIManager::GetInstance()->Find_FontObject(L"Boss_Name")->Set_Text(L"라 우 라");
			UIManager::GetInstance()->Find_FontObject(L"Boss_Tag")->Set_Text(L"타락한 자연의 사도");
		}
		else if (TileManager::GetInstance()->Get_Stage() == TILE_DOCHERBOSS) {
			UIManager::GetInstance()->Find_FontObject(L"Boss_Name")->Set_Text(L"도 철");
			UIManager::GetInstance()->Find_FontObject(L"Boss_Tag")->Set_Text(L"분노의 거대 사념체");
		}
		UIManager::GetInstance()->Find_FontObject(L"Boss_Name")->Set_Color((INT)HPOPC, 255, 255, 255);
		UIManager::GetInstance()->Find_FontObject(L"Boss_Tag")->Set_Color((INT)HPOPC, 255, 255, 255);
	}
	else if (Enable_DisplayHPBar == FALSE) {
		if	(HPOPC > 2.f)	{ HPOPC -= _DT * 255.f / 2.f; }
		else				{ 
			HPOPC = 255.f; Enable_DisplayHPBar = 2; 
			UIManager::GetInstance()->Find_FontObject(L"Boss_Name")->Set_Text(L"");
			UIManager::GetInstance()->Find_FontObject(L"Boss_Tag")->Set_Text(L"");

			UIManager::GetInstance()->Find_FontObject(L"Boss_Name")->Set_Color(0, 255, 255, 255);
			UIManager::GetInstance()->Find_FontObject(L"Boss_Tag")->Set_Color(0, 255, 255, 255);
			Component_Sprite->Get_Texture(L"HPBar_Frame")->Set_Opacity(0);
			HPBarFill->Set_Opacity(0);

			return;
		}
		Component_Sprite->Get_Texture(L"HPBar_Frame")->Set_Opacity((INT)HPOPC);
		HPBarFill->Set_Opacity((INT)HPOPC);

		UIManager::GetInstance()->Find_FontObject(L"Boss_Name")->Set_Color((INT)HPOPC, 255, 255, 255);
		UIManager::GetInstance()->Find_FontObject(L"Boss_Tag")->Set_Color((INT)HPOPC, 255, 255, 255);
	}
}
VOID MainUI::Display_FadeFilter(CONST FLOAT& _DT) {
	if		(Enable_FadeFilter == 2) return;
	SpriteINFO* Filter = UIManager::GetInstance()->Find_FilterObjects(L"FadeFilter");
	if (Enable_FadeFilter == TRUE) {		// FADE IN - 어두워지게
		if (Filter->Get_Vislble() == FALSE) {
			Filter->Set_Visible(TRUE);
		}
		if (FadeOPC < 253.f) { FadeOPC += _DT * 255.f; Filter->Set_Opacity(FadeOPC); }
		else					{ Filter->Set_Opacity(255.f); Enable_FadeFilter = 2; return;	}
	}
	else if (Enable_FadeFilter == FALSE) {		// FADE OUT - 밝아지게
		if (FadeOPC > 2.f)	{ FadeOPC -= _DT * 255.f; Filter->Set_Opacity(FadeOPC);}
		else					{ 
			Filter->Set_Opacity(0.f);
			Filter->Set_Visible(FALSE);
			Enable_FadeFilter = 2; 
			return;	}
	}
}
VOID MainUI::Synchronize_BossHPBar() {
	if		(TileManager::GetInstance()->Get_Stage() == TILE_FIRSTBOSS && SceneManager::GetInstance()->Get_GameObject(L"CheonLog") != nullptr) {
		Collider* CheonLog = dynamic_cast<Collider*>(SceneManager::GetInstance()->Get_GameObject(L"CheonLog")->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER));
		CurrentHP = 1.f - ((float)(CheonLog->Get_Hp()) / (float)MaxHP);

		FLOAT HPRatio = (FLOAT)(CheonLog->Get_Hp()) / (FLOAT)MaxHP;

		BarScale = { HPRatio, BarScale.y, BarScale.z };
		if (BarScale.x <= 0) BarScale.x = 0;
	}
	else if (TileManager::GetInstance()->Get_Stage() == TILE_DOCHERBOSS && SceneManager::GetInstance()->Get_GameObject(L"Docheol") != nullptr) {
		Collider* Docheol = dynamic_cast<Collider*>(SceneManager::GetInstance()->Get_GameObject(L"Docheol")->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER));

		FLOAT HPRatio = (FLOAT)(Docheol->Get_Hp()) / (FLOAT)MaxHP;

		BarScale = { HPRatio, BarScale.y, BarScale.z };
		if (BarScale.x <= 0) BarScale.x = 0;
	}
}

VOID MainUI::Display_ClearBossUI(CONST FLOAT& _DT) {
	if (Enable_BossClearUI) {
		BossClearTimer += _DT;
		FLOAT Delay = 0.f;
		if (BossClearTimer > Delay && BossClear[0]) {
			//Enable_FadeFilter = TRUE;
			BossClear[0] = FALSE;
		}
		if		(BossClearTimer > Delay && BossClear[1]) {
			REPLAY_UI_EFFECT(L"CLEAR_BREAK");
			BossClear[1] = FALSE;
		}
		else if (BossClearTimer > Delay + 0.5f && BossClear[2]) {
			REPLAY_UI_EFFECT(L"CLEAR_CHARGE");
			BossClear[2] = FALSE;
		}
		else if (BossClearTimer > Delay + 1.25f && BossClear[3]) {
			REPLAY_UI_EFFECT(L"CLEAR_LINE");
			BossClear[3] = FALSE;
		}
		else if (BossClearTimer > Delay + 2.f && BossClearTimer < Delay + 2.5f) {
			UIManager::GetInstance()->Find_FontObject(L"Destroyed")->Set_Color(255 * 2 * (BossClearTimer - (Delay + 2.f)), 255, 255, 255);
		}
		else if (BossClearTimer > Delay + 3.f && BossClearTimer < Delay + 3.5f) {
			UIManager::GetInstance()->Find_FontObject(L"Destroyed")->Set_Color(255 - 255 * 2 * (BossClearTimer - (Delay + 2.f)), 255, 255, 255);
		}
		else if (BossClearTimer > Delay + 3.5f) {
			memset(BossClear, TRUE, sizeof(BossClear));
			BossClearTimer = 0.f;
			Enable_BossClearUI = FALSE;
		}
	}
}

HRESULT MainUI::Component_Initialize() {
	Component_Sprite = ADD_COMPONENT_SPRITE;
	TextureList = Component_Sprite->Get_TextureList();

	return S_OK;
}
HRESULT MainUI::Sprite_Initialize() {
	////////////////////////////////////////////// BACKBAR //////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/HP_BG.png", L"HP_BG", 13.f, -60.f, 183, 180, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/HP_BG.png", L"TEARDROP_BG", 13.f, -15.f, 130, 180, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/HP_BG.png", L"KEY_BG", 13.f, 42.f, 100, 150, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/HP_BG.png", L"COIN_BG", 13.f, 77.f, 100, 150, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/HP_BG.png", L"CRYSTAL_BG", 13.f, 113.f, 100, 150, TRUE, 255);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////// HP/MP /////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_HpStock.png",		L"HP_SPRITE1", -45.f, -60.f, 180, 180, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_HpStock.png",		L"HP_SPRITE2", -15.f, -60.f, 180, 180, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_HpStock.png",		L"HP_SPRITE3", 15.f, -60.f, 180, 180, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_HpStock.png",		L"HP_SPRITE4", 45.f, -60.f, 180, 180, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_HpStock.png",		L"HP_SPRITE5", 75.f, -60.f, 180, 180, TRUE);

	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_EmptyHpStock.png",	L"EHP_SPRITE1", -45.f, -60.f, 180, 180, FALSE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_EmptyHpStock.png",	L"EHP_SPRITE2", -15.f, -60.f, 180, 180, FALSE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_EmptyHpStock.png",	L"EHP_SPRITE3", 15.f, -60.f, 180, 180, FALSE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_EmptyHpStock.png",	L"EHP_SPRITE4", 45.f, -60.f, 180, 180, FALSE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_Ui_EmptyHpStock.png",	L"EHP_SPRITE5", 75.f, -60.f, 180, 180, FALSE);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// UTILITY //////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Crystal.png", L"CRYSTAL", 18.f, 175.f, 20, 20, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Key.png", L"KEY", 18.f, 103.f, 18, 18, TRUE);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// SPEECH ///////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SpeechBubble_BG.png", L"SpeechBubble_BG", 0.f, 529.f + 30.f, 360, 62, TRUE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SpeechBubble_Tif.png", L"SpeechBubble_Tif", 0.f, 529.f + 30.f, 153, 62, TRUE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SpeechBubble_TimeStop.png", L"SpeechBubble_TimeStop", 0.f, 529.f + 30.f, 153, 62, TRUE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SpeechBubble_Angry.png", L"SpeechBubble_Angry", 0.f, 529.f + 30.f, 153, 62, TRUE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SpeechBubble_Frame.png", L"SpeechBubble_Frame", 0.f, 503.f + 30.f, 566 * 0.7f, 126 * 0.7f, TRUE, 0);
	for (int i = 1; i <= 19; i++) {
		TCHAR FileName1[128] = L"";
		TCHAR FileName2[128] = L"";
		wsprintfW(FileName1, L"../../UI/MainUI/BubblEffect_%d.png", i);
		wsprintfW(FileName2, L"BubblEffect_%d.png", i);
		SpriteINFO* Effect = Component_Sprite->Get_Texture(FileName1);
		Component_Sprite->Import_Sprite(FileName1, FileName2, 0.f, 529.f + 30.f, 360, 80, TRUE, 0);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// SKILL ////////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SkillState_BG.png", L"SkillState_BG", 0.f, 600.f, 290.f, 120.f, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SkillState_Frame.png", L"SkillState_Frame", 0.f, 600.f, 290.f, 120.f, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SkillState_Symbol_OFF.png", L"SkillState_Symbol_OFF", 15.f, 615.f, 80.f, 80.f, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SkillState_SkillOn.png", L"SkillState_SkillOn", 15.f, 615.f, 80.f, 80.f, FALSE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/SkillState_Symbol_ON.png", L"SkillState_Symbol_ON", 15.f, 615.f, 80.f, 80.f, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/KEY_Q.png", L"KEY_Q", 45.f, 595.f, 20.f, 20.f, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Token.png", L"Token1", 104.f, 669.f, 33.f, 29.f, TRUE);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Token.png", L"Token2", 137.f, 669.f, 33.f, 29.f, TRUE);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// INTERACT /////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/KEY_E.png", L"KEY_E", 720.f, 590.f, 35, 35, FALSE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Interaction_BG.png", L"Interaction_BG", 700.f, 590.f, 215, 35, FALSE, 155);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// WEAPON /////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/WeaponBG_Arrow.png", L"WeaponBG_Arrow", 1166.f, 580.f, 108, 108, TRUE, 150);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/WeaponBG_ArrowCount.png", L"WeaponBG_ArrowCount", 1167.f, 681.f, 108, 30, TRUE, 150);
	SpriteINFO* EquipArrowImg = nullptr;
	EquipArrowImg = Component_Sprite->Import_Sprite(L"../../UI/Weapon_UI/FairyBow_UI.png", L"FairyBow_IMG", 1173.f, 586.f, 90, 90, TRUE, 150);
	BowIMG_List.push_back(EquipArrowImg);
	EquipArrowImg = Component_Sprite->Import_Sprite(L"../../UI/Weapon_UI/IceBow_UI.png", L"IceBow_IMG", 1174.f, 589.f, 85, 85, FALSE, 150);
	BowIMG_List.push_back(EquipArrowImg);
	EquipArrowImg = Component_Sprite->Import_Sprite(L"../../UI/Weapon_UI/EvilHeadBow_UI.png", L"EvilHeadBow_IMG", 1175.f, 589.f, 90, 90, FALSE, 150);
	BowIMG_List.push_back(EquipArrowImg);
	EquipArrowImg = Component_Sprite->Import_Sprite(L"../../UI/Weapon_UI/IRABow_UI.png", L"IRABow_IMG", 1173.f, 588.f, 95, 95, FALSE, 150);
	BowIMG_List.push_back(EquipArrowImg);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// BOSSUI /////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Boss_UI/Spr_Ui_Boss_HPFrame.png", L"HPBar_Frame", 320, 40, 600, 30, FALSE, 0);
	HPBarFill	= new SpriteINFO(L"HPBar_Fill", 590, 23, 162, 22, FALSE, 0);
	D3DXCreateTextureFromFileExW(GRPDEV, L"../../UI/Boss_UI/Spr_Ui_Boss_HP.png", HPBarFill->WIDTH, HPBarFill->HEIGHT,
		1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&HPBarFill->TEXTURE);
	BossTitleBar = new SpriteINFO(L"Boss_TitleBar", 260, 4, 2, 400, FALSE, 0);
	D3DXCreateTextureFromFileExW(GRPDEV, L"../../UI/Boss_UI/Boss_TitleBar.png", BossTitleBar->WIDTH, BossTitleBar->HEIGHT,
		1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&BossTitleBar->TEXTURE);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// FILTER /////////////////////////////////////////////////////
	SpriteINFO* FilterOBJ = new SpriteINFO(L"FadeFilter", 1280, 720, 0, 0, FALSE, 0);
	D3DXCreateTextureFromFileExW(GRPDEV, L"../../UI/Filter_Fade.png", FilterOBJ->WIDTH, FilterOBJ->HEIGHT,
		1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&FilterOBJ->TEXTURE);
	UIManager::GetInstance()->Add_FilterObjects(FilterOBJ);
	Component_Sprite->Import_Sprite(L"../../UI/Filter_Fade.png", L"BossClearBG", 1280, 720, 0, 0, true, 0);

	Component_Sprite->Import_Sprite(L"../../UI/MainUI/ItemNoticeBG.png", L"ItemNoticeBG", 1300.f, 320.f, 300, 40, TRUE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_Item1.png", L"Relic_Item1", 1300.f, 290.f, 80, 80, TRUE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_Item2.png", L"Relic_Item2", 1300.f, 290.f, 80, 80, TRUE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_Item3.png", L"Relic_Item3", 1300.f, 290.f, 80, 80, TRUE, 0);

	return S_OK;
}
HRESULT MainUI::Effect_Initialize() {

	/////////////////////////////////////// STATE ////////////////////////////////////////////////
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::HP_EFFECT, L"HP_EFFECT1",   8.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::HP_EFFECT, L"HP_EFFECT2",  38.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::HP_EFFECT, L"HP_EFFECT3",  68.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::HP_EFFECT, L"HP_EFFECT4",  98.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::HP_EFFECT, L"HP_EFFECT5", 128.f, -30.f, 75, 75, 0.75f, 255);

	PLAY_UI_EFFECT(MAIN_UI_EFFECT::DASHSTOCK_EFFECT, L"DASHSTOCK_EFFECT1", -43.f, -13.f, 175, 175, 2.f, 255);
	PLAY_UI_EFFECT(MAIN_UI_EFFECT::DASHSTOCK_EFFECT, L"DASHSTOCK_EFFECT2", -10.f, -13.f, 175, 175, 2.f, 255);
	PLAY_UI_EFFECT(MAIN_UI_EFFECT::DASHSTOCK_EFFECT, L"DASHSTOCK_EFFECT3",  23.f, -13.f, 175, 175, 2.f, 255);
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// UTILITY /////////////////////////////////////////////////
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::COIN_EFFECT, L"COIN_EFFECT", 20.f, 142.f, 15, 15, 1.f, 255);
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////// SKILL //////////////////////////////////////////////////
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::TOKEN_EFFECT,L"TOKEN_EFFECT1", 70.f,  630.f, 100, 100, 0.6f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::TOKEN_EFFECT,L"TOKEN_EFFECT2", 104.f, 630.f, 100, 100, 0.6f, 255);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::CLEAR_MARK,  L"CLEAR_MARK"	, WINCX / 2 - 350 / 2, WINCY / 2 - 475 / 2 + 20	, 350, 475, 0.75f, 100);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::CLEAR_BREAK, L"CLEAR_BREAK"	, WINCX / 2 - 256 / 2, WINCY / 2 - 400 / 2 + 15	, 256, 400, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::CLEAR_CHARGE,L"CLEAR_CHARGE", WINCX / 2 - 256 / 2, WINCY / 2 - 256 / 2 - 40	, 256, 256, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::CLEAR_LINE,  L"CLEAR_LINE"	, WINCX / 2 - 512 / 2, WINCY / 2 - 116 / 2 - 48 , 512, 116, 0.75f, 255);

	return S_OK;
}
HRESULT MainUI::Text_Initialize() {
	////////////////////////////////////////////// UTILITY //////////////////////////////////////////////////////
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"0", { 52.f, 106.f }, 16, L"KeyCountText",		L"08서울한강체 L",	D3DCOLOR_ARGB(200, 255, 255, 255)));
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"0", { 52.f, 142.f }, 16, L"CoinCountText",		L"08서울한강체 L",	D3DCOLOR_ARGB(200, 255, 255, 255)));
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"0", { 52.f, 178.f }, 16, L"CrystalCountText",	L"08서울한강체 L",	D3DCOLOR_ARGB(200, 255, 255, 255)));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// WEAPON ///////////////////////////////////////////////////////
	FO_ArrowCount = UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", {1220.f, 687.f}, 16, L"ArrowCountText",			L"08서울한강체 L",	D3DCOLOR_ARGB(200, 255, 255, 255));
	AllFontOBJ.push_back(FO_ArrowCount);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////// INTERACTION ////////////////////////////////////////////////////
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 760.f, 600.f }, 16, L"Interaction_Text",	L"08서울한강체 L",	D3DCOLOR_ARGB(200, 255, 255, 255), 100, TRUE, DT_LEFT));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////// GETITEM ///////////////////////////////////////////////////
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 1430.f, 330.f }, 20, L"ItemInfo",			L"08서울한강체 L",	D3DCOLOR_ARGB(200, 255, 255, 255)));
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 1430.f, 400.f }, 12, L"ItemClass",			L"08서울한강체 L",	D3DCOLOR_ARGB(200, 255, 255, 255)));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// SPEECH ///////////////////////////////////////////////////////
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 140.f, 550.f + 30.f }, 13, L"TifNotice_Text",					L"08서울한강체 L",	D3DCOLOR_ARGB(0, 255, 255, 255), 100, TRUE, DT_LEFT);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 615.091,	5.f }, 15, L"Boss_Name",		L"08서울한강체 L",	D3DCOLOR_ARGB(0, 255, 255, 255), 700));
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 616.967f, 25.f }, 13, L"Boss_Tag",			L"08서울한강체 L",	D3DCOLOR_ARGB(0, 160, 160, 160)));
	Title_Name = UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 30.f, 270.f }, 65, L"Boss_Title_Name",	TEXT("Yoon\u00AE 대한"),		D3DCOLOR_ARGB(0, 255, 255, 255), 600, TRUE, DT_LEFT);
	Title_Tag = UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 45.f, 370.f }, 32, L"Boss_Title_Tag",	TEXT("Yoon\u00AE 민국 Bold"),	D3DCOLOR_ARGB(0, 255, 255, 255), 600, TRUE, DT_LEFT);
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"근원 파괴", { 630.f, 295.f }, 40, L"Destroyed", L"08서울한강체 L", D3DCOLOR_ARGB(0, 255, 255, 255));
	return S_OK;
}
VOID MainUI::PopUp_ItemInfo(ItemINFO* Item, SpriteINFO* SPR, FLOAT _DT) {
	if (Enable_ItemPopUp) {
		SpriteINFO* BackGround = Component_Sprite->Get_Texture(L"ItemNoticeBG");
		BackGround->Set_Visible(TRUE);
		SpriteINFO* ITEM = SPR;

		ITEM->Set_Visible(TRUE);

		FontObject* InfoFont = UIManager::GetInstance()->Find_FontObject(L"ItemInfo");
		FontObject* ClassFont = UIManager::GetInstance()->Find_FontObject(L"ItemClass");

		InfoFont->Set_Text(Item->ItemDesc[1]);
		ClassFont->Set_Text(Item->ItemDesc[2]);

		if (ItemPopUp_Timer <= 1) {
			ItemPopUp_Timer += _DT;
			if (BackGround->Get_Pos().x >= 1000.f) {
				BackGround->Set_Pos(BackGround->Get_Pos().x - 20.f, BackGround->Get_Pos().y);
				ITEM->Set_Pos(ITEM->Get_Pos().x - 20.f, ITEM->Get_Pos().y);
			}if (InfoFont->Get_Pos().x >= 1130.f) {
				InfoFont->Set_Pos(InfoFont->Get_Pos().x - 20.f, InfoFont->Get_Pos().y);
				ClassFont->Set_Pos(ClassFont->Get_Pos().x - 20.f, ClassFont->Get_Pos().y);
			}


			if (ItemPopUp_Timer < 0.33f) {
				BackGround->Set_Opacity(200 * ItemPopUp_Timer * 3);
				ITEM->Set_Opacity(200 * ItemPopUp_Timer * 3);
				InfoFont->Set_Color(200 * ItemPopUp_Timer * 3, 255, 255, 255);
				ClassFont->Set_Color(200 * ItemPopUp_Timer * 3, 255, 255, 255);
			}
			else {
				BackGround->Set_Opacity(200);
				ITEM->Set_Opacity(200);
				InfoFont->Set_Color(200, 255, 255, 255);
				ClassFont->Set_Color(200, 255, 255, 255);
			}
		}
		else if (ItemPopUp_Timer >= 1.f && ItemPopUp_Timer < 3.f) {
			ItemPopUp_Timer += _DT;
		}
		else if (ItemPopUp_Timer >= 3.f && ItemPopUp_Timer < 4.f) {
			ItemPopUp_Timer += _DT;
			if (ItemPopUp_Timer < 4.f) {
				BackGround->Set_Opacity(200 - (200 * (ItemPopUp_Timer - 3)));
				ITEM->Set_Opacity((200 - (200 * (ItemPopUp_Timer - 3))));
				InfoFont->Set_Color((200 - (200 * (ItemPopUp_Timer - 3))), 255, 255, 255);
				ClassFont->Set_Color(200 - (200 * (ItemPopUp_Timer - 3)), 255, 255, 255);
			}
			else {
				BackGround->Set_Opacity(0);
				ITEM->Set_Opacity(0);
				InfoFont->Set_Color(0, 255, 255, 255);
				ClassFont->Set_Color(0, 255, 255, 255);
			}
		}
		else if (ItemPopUp_Timer >= 4.f) {
			BackGround->Set_Pos(1300.f, 320.f);
			ITEM->Set_Pos(1300.f, 290.f);
			InfoFont->Set_Pos(1430.f, InfoFont->Get_Pos().y);
			ClassFont->Set_Pos(1430.f, ClassFont->Get_Pos().y);

			BackGround->Set_Opacity(0);
			ITEM->Set_Opacity(0);
			InfoFont->Set_Color(0, 255, 255, 255);
			ClassFont->Set_Color(0, 255, 255, 255);

			ItemPopUp_Timer = 0.f;
			Enable_ItemPopUp = FALSE;
		}
	}
}
MainUI* MainUI::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	MainUI* MUI = new MainUI(_GRPDEV);
	if (FAILED(MUI->Ready_GameObject())) {
		MSG_BOX("Cannot Create MainUI.");
		Safe_Release(MUI);
		return nullptr;
	}
	return MUI;
}
void MainUI::Imgui_Setting()
{
	static _float fsScale(1);
	static _float sMin(-100), sMax(2000), sMin1(-100), sMax1(2000);
	static _float fPivot1(1);

	_float fMin(0.0f), fMax(100), UMin(0), UMax(0), fMovePosMin(-100), fMovePosMax(129), fMoveScaleMin(-100), fMoveScaleMax(129), fMoveRotMin(-360), fMoveRotMax(360), vMouseMin(0), vMouseMax(0), fHeightMin(0.f), fHeightMax(10.f), fPosMin(0.f), fPosMax(200.f);


	///////////위치
	_vec2 vPos = { GuiVar.POSX, GuiVar.POSY};
	ImGui::Text("UI_POS");
	ImGui::SameLine(200.f, 0.f);
	ImGui::SliderFloat2("##1", vPos, sMin, sMax);

	Imgui_ButtonStyle();				// 버튼 시작
	if (ImGui::Button("UI_POSX+"))		// 이름 달라야댐 누를 버튼
	{
		vPos.x += fPivot1;				// 수치입력
	}
	ImGui::SameLine(150, 0.f);			// 같은라인 버튼
	ImGui::PopStyleColor(3);			// 버튼 끝

	Imgui_ButtonStyle();
	if (ImGui::Button("UI_POSX-"))
	{
		vPos.x -= fPivot1;
	}
	ImGui::PopStyleColor(3);

	Imgui_ButtonStyle(); 
	if (ImGui::Button("UI_POSY+")) 
	{
		vPos.y += fPivot1;			
	}
	ImGui::SameLine(150, 0.f);			
	ImGui::PopStyleColor(3);	

	Imgui_ButtonStyle();
	if (ImGui::Button("UI_POSY-"))
	{
		vPos.y -= fPivot1;
	}
	ImGui::PopStyleColor(3);
	GuiVar.POSX = vPos.x;
	GuiVar.POSY = vPos.y;


	///////////스케일
	_vec2 vScale = { GuiVar.WIDTHX, GuiVar.WIDTHY };
	ImGui::Text("UI_SCALE");
	ImGui::SameLine(200.f, 0.f);
	ImGui::SliderFloat2("##2", vScale, sMin1, sMax1);

	Imgui_ButtonStyle(); 
	if (ImGui::Button("UI_SCALEX+"))
	{
		vScale.x += fPivot1;				
	}
	ImGui::SameLine(150, 0.f);			
	ImGui::PopStyleColor(3);			
	Imgui_ButtonStyle();
	if (ImGui::Button("UI_SCALEX-"))
	{
		vScale.x -= fPivot1;
	}
	ImGui::PopStyleColor(3);

	GuiVar.WIDTHX = vScale.x;
	GuiVar.WIDTHY = vScale.x;

}
void MainUI::Imgui()
{
	ImGui::SetNextWindowSize({ 600,300 });

	ImGui::Begin("Editor", NULL, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
				cout << ("Save clicked\n");
			ImGui::Separator(); //구분줄
			if (ImGui::MenuItem("Open"))
				cout << ("Open clicked\n");
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	Imgui_Setting();

	ImGui::End();

	ImGui::Begin("Mode Changer", NULL, ImGuiWindowFlags_MenuBar);

	ImGui::SetNextWindowSize({ 800,300 });

	ImGui::End();
}
void MainUI::Imgui_ButtonStyle()
{
	ImGui::PushStyleColor(ImGuiCol_Button, D3DXCOLOR(0.0f, 0.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.8f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.5f, 0.7f, 0.7f));
}
VOID MainUI::Free() {
	Safe_Delete(Title_Name);
	Safe_Delete(Title_Tag);
	Safe_Delete(HPBarFill);
	Safe_Delete(BossTitleBar);
	Safe_Release(BossHPSprite);
	GameObject::Free();
}