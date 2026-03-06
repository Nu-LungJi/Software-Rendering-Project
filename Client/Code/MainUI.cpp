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

	SpeechBubble_Activate = FALSE;
	SpeechBubble_FadeInTime = 1.f;
	SpeechBubble_FadeOutTime = 1.f;
	SpeechBubble_StayTime = 5.f;
	SpeechBubble_Text = L"";

	ArrowCountText = L"";
	Timer01 = 0.f; Timer02 = 0.f; Timer03 = 0.f;
	Enable_Interaction = 2;
	Interaction_Text = L"";
	ItemINFOSetting = TRUE;

	MainUIOpacity = 0.f;
	
	Current_KeyCount		= 0;
	Current_CoinCount		= 0;
	Current_CrystalCount	= 0;
	Cur_BowIMGIDX			= 0;

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
	FadeOPC = UIManager::GetInstance()->Find_GlobalObject(L"FadeFilter")->OPACITY;

	PopUpItem = nullptr;
	Enable_BossClearUI = FALSE;
	BossClearTimer = 0.f;

	Enable_Tutorial = FALSE;
	Tutorial_Timer = 0.f;
	Tutorial_Sequence = 0;

	BowIMG_List.push_back(Component_Sprite->Get_Texture(L"FairyBow_IMG"));
	BowIMG_List.push_back(Component_Sprite->Get_Texture(L"IceBow_IMG"));
	BowIMG_List.push_back(Component_Sprite->Get_Texture(L"EvilHeadBow_IMG"));
	BowIMG_List.push_back(Component_Sprite->Get_Texture(L"IRABow_IMG"));

	return S_OK;
}
INT		MainUI::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	PopUp_Speech_Bubble(SpeechBubble_Text, _DT);
	PopUp_Speech_Bubble_Skill(SpeechBubble_Text, _DT, skillType);
  
	Timer02 += _DT; 
	if(Timer02 > 0.04f){
		Player_KeyModify();
		Player_MoneyModify();
		Player_CrystalModify();
		Timer02 = 0.f;
	}
	PopUp_ItemInfo(PopUpItem, _DT);
	MainUI_FadeAction(_DT, FadeSpeed);
	Synchronize_BossHPBar();

	Display_InteractionUI();
	Display_Tutorial(_DT);
	Display_BossTitle(_DT);
	Display_BossHPBar(_DT);
	Display_FadeFilter(_DT);
	Display_ClearBossUI(_DT);

	ArrowCountText = to_wstring(PlayerObject->Get_CurArrowCount()) + L" / " +  to_wstring(PlayerObject->Get_MaxArrow());
	FO_ArrowCount->Set_Text(ArrowCountText);

	int cur_Equip_BowIDX = PlayerObject->Get_Bow_ImgIDX();
	if (Cur_BowIMGIDX != cur_Equip_BowIDX) {
		BowIMG_List[Cur_BowIMGIDX]->Set_Visible(FALSE);
		BowIMG_List[cur_Equip_BowIDX]->Set_Visible(TRUE);
		Cur_BowIMGIDX = cur_Equip_BowIDX;
	}

	Reset_Relic();
	Set_RelicIcon();	// 렐릭 아이콘

	if (KEY_DOWN(DIK_J)) {
		Enable_BossClearUI = TRUE;
	}
	return 0;
}
VOID	MainUI::LateUpdate_GameObject(CONST FLOAT& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
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
		Component_Sprite->Get_Texture(UIKey_HP)->Set_Visible(FALSE);
	}
}

VOID MainUI::PopUp_Speech_Bubble(wstring _Text, FLOAT _DT) {
	if (SpeechBubble_Activate) {
		SpriteINFO* BackGround	= Component_Sprite->Get_Texture(L"SpeechBubble_BG");
		SpriteINFO* Frame		= Component_Sprite->Get_Texture(L"SpeechBubble_Frame");
		SpriteINFO* Character	= Component_Sprite->Get_Texture(L"SpeechBubble_Tif");
		FontObject* Font		= UIManager::GetInstance()->Find_FontObject(L"TifNotice_Text");

		Font->Text = _Text;
		if		(Timer01 < SpeechBubble_FadeInTime) {
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
		else if (Timer01 >= SpeechBubble_FadeInTime							&& Timer01 < SpeechBubble_FadeInTime + SpeechBubble_StayTime) {
			Timer01 += _DT;
		}
		else if (Timer01 >= SpeechBubble_FadeInTime + SpeechBubble_StayTime && Timer01 < SpeechBubble_FadeInTime + SpeechBubble_FadeOutTime + SpeechBubble_StayTime) {
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
		else if (Timer01 >= SpeechBubble_FadeInTime + SpeechBubble_FadeOutTime + SpeechBubble_StayTime) {
			Timer01 = 0;
			SpeechBubble_Activate = FALSE;

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

VOID MainUI::Reset_Relic()
{
	for (auto idx : relicEffectList) {
		// 텍 비지블 오프
	}
}

VOID MainUI::Set_RelicIcon()
{
	SpriteINFO* sprite = nullptr;
	
	for (int i = 0; i < 4; i++) {
		int idx = PlayerObject->Get_Relic_ImgIdx();
		if (idx == -1) continue;
		switch (idx) {
		case 0:
			sprite = Component_Sprite->Get_Texture(L"Relic_Item1");			
			sprite->Set_Visible(TRUE);
			sprite->Set_Pos(600 + i * 50.f,600.f);
			break;
		case 1:
			sprite = Component_Sprite->Get_Texture(L"Relic_Item2");
			sprite->Set_Visible(TRUE);
			sprite->Set_Pos(600 + i * 50.f,600.f);
			break;
		case 2:
			sprite = Component_Sprite->Get_Texture(L"Relic_Item3");
			sprite->Set_Visible(TRUE);
			sprite->Set_Pos(600 + i * 50.f, 600.f);
			break;
		case 3:
			sprite = Component_Sprite->Get_Texture(L"Relic_Item4");
			sprite->Set_Visible(TRUE);
			sprite->Set_Pos(600 + i * 50.f, 600.f);
			break;
		case 4:
			sprite = Component_Sprite->Get_Texture(L"Perk_");
			sprite->Set_Visible(TRUE);
			sprite->Set_Pos(600 + i * 50.f, 600.f);
			break;
		default:
			break;
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
			for (auto& AUE : AllUIEffect) 
				AUE->Set_EffectFadeOption(TRUE);
			EffectFaded = TRUE;
		}
	}
	else if (Enable_MainUIFade == FALSE) {
		if (Component_Sprite->Get_Texture(L"HPBar_Frame")->VISIBLE == FALSE) {
			Component_Sprite->Get_Texture(L"HPBar_Frame")->Set_Opacity(0);
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
		AllSpriteOBJ.push_back(Component_Sprite->Get_Texture(L"Token3"));
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
		AllUIEffect.push_back(static_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, L"TOKEN_EFFECT3")));

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

		if (nullptr != dynamic_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene())) {
			Component_Sprite->Get_Texture(L"HPBar_Frame")->Set_Visible(true);
			Component_Sprite->Get_Texture(L"HPBar_Frame")->Set_Opacity((INT)HPOPC);
			HPBarFill->Set_Opacity(HPOPC);
			return;
		}
		
		if (TileManager::GetInstance()->Get_Stage() == TILE_FIRSTBOSS) {
			UIManager::GetInstance()->Find_FontObject(L"Boss_Name")->Set_Text(L"라 우 라");
			UIManager::GetInstance()->Find_FontObject(L"Boss_Tag")->Set_Text(L"타락한 자연의 사도");
		}
		else if (TileManager::GetInstance()->Get_Stage() == TILE_DOCHERBOSS) {
			UIManager::GetInstance()->Find_FontObject(L"Boss_Name")->Set_Text(L"도 철");
			UIManager::GetInstance()->Find_FontObject(L"Boss_Tag")->Set_Text(L"분노의 거대 사념체");
			Component_Sprite->Get_Texture(L"HPBar_Frame")->Set_Visible(TRUE);
			Component_Sprite->Get_Texture(L"HPBar_Frame")->Set_Opacity((INT)HPOPC);
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
	SpriteINFO* Filter = UIManager::GetInstance()->Find_GlobalObject(L"FadeFilter");
	if (Enable_FadeFilter == TRUE) {		// FADE IN - 어두워지게
		if (Filter->Get_Vislble() == FALSE) {
			Filter->Set_Visible(TRUE);
		}
		if (FadeOPC < 253.f) { 
			FadeOPC += _DT * 255.f; 
			Filter->Set_Opacity(FadeOPC); 
		}
		else					{ 
			Filter->Set_Opacity(255.f); 
			Enable_FadeFilter = 2; 
			return;	}
	}
	else if (Enable_FadeFilter == FALSE) {		// FADE OUT - 밝아지게
		FadeOPC = Filter->OPACITY;
		if (FadeOPC > 2.f)	{ 
			FadeOPC -= _DT * 255.f / 2; Filter->Set_Opacity(FadeOPC);
		}
		else					{ 
			Filter->Set_Opacity(0.f);
			Filter->Set_Visible(FALSE);
			Enable_FadeFilter = 2; 
			return;	}
	}
}
VOID MainUI::Synchronize_BossHPBar() {
	if (TileManager::GetInstance()->Get_Stage() == TILE_FIRSTBOSS && SceneManager::GetInstance()->Get_GameObject(L"CheonLog") != nullptr) {
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
	else if (nullptr != dynamic_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene())) {
		
		_float fProgressRatio = 0.f;

		fProgressRatio = (POS(PlayerObject)->x + POS(PlayerObject)->y) * 0.01f;

		if		(fProgressRatio > 1)		fProgressRatio = 1.f;
		else if (fProgressRatio < 0)		fProgressRatio = 0.f;

		BarScale = { fProgressRatio, BarScale.y, BarScale.z };
	}
}

VOID MainUI::Display_ClearBossUI(CONST FLOAT& _DT) {
	if (Enable_BossClearUI) {
		BossClearTimer += _DT;
		FLOAT Delay = 3.f, BackGroundOPC = 180.f;
		SpriteINFO* FadeBG = Component_Sprite->Get_Texture(L"BossClearBG");
		if (BossClearTimer > Delay  && BossClearTimer < Delay + 6.f) {
			if (BossClearTimer <= 5.f) {
				SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM03, 0.5f - BossClearTimer / 10);
			}
			else {}
		}
		if (BossClearTimer > Delay && BossClearTimer < Delay + 1.5f) {
			FadeBG->Set_Opacity(BackGroundOPC * (BossClearTimer - Delay) / 1.5f);
		}
		else if (BossClearTimer > (Delay + 4.3f) && BossClearTimer < (Delay + 5.3f)) {
			FadeBG->Set_Opacity(BackGroundOPC + (255 - BackGroundOPC) * (BossClearTimer - (Delay + 4.3f)));
		}
		else if (BossClearTimer > (Delay + 5.3f) && BossClearTimer < (Delay + 5.8f)) {
			if (BossClear[4]) {
				dynamic_cast<EndingCredit*>(SceneManager::GetInstance()->Get_GameObject(L"EndingCredit"))->Set_CreditStart(TRUE);
				UIManager::GetInstance()->Find_FontObject(L"KeyCountText")->Set_Visible(FALSE);
				UIManager::GetInstance()->Find_FontObject(L"CoinCountText")->Set_Visible(FALSE);
				UIManager::GetInstance()->Find_FontObject(L"CrystalCountText")->Set_Visible(FALSE);
				UIManager::GetInstance()->Find_FontObject(L"ArrowCountText")->Set_Visible(FALSE);
				UIManager::GetInstance()->Find_GlobalFontObject(L"Destroyed")->Set_Visible(FALSE);
				BossClear[4] = FALSE;
			}
			
			FadeBG->Set_Opacity(255 - 254 * 2 * (BossClearTimer - (Delay + 5.3f)));
		}
		if		(BossClearTimer > Delay && BossClear[0]) {
			dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_BREAK"))->Replay_Effect();
			BossClear[0] = FALSE;
		}
		else if (BossClearTimer > Delay + 0.5f && BossClear[1]) {
			dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_CHARGE"))->Replay_Effect();
			BossClear[1] = FALSE;
		}
		else if (BossClearTimer > Delay + 1.0f && BossClear[2]) {
			dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_MARK"))->Replay_Effect();
			BossClear[2] = FALSE;
		}
		else if (BossClearTimer > Delay + 2.0f && BossClear[3]) {
			dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_LINE"))->Replay_Effect();
			BossClear[3] = FALSE;
		}
		else if (BossClearTimer > Delay + 2.3f && BossClearTimer < Delay + 2.8f) {
			UIManager::GetInstance()->Find_FontObject(L"Destroyed")->Set_Color(254 * 2 * (BossClearTimer - (Delay + 2.3f)), 255, 255, 255);
		}
		else if (BossClearTimer > Delay + 3.8f && BossClearTimer < Delay + 4.3f) {
			UIManager::GetInstance()->Find_FontObject(L"Destroyed")->Set_Color(255 - 254 * 2 * (BossClearTimer - (Delay + 3.8f)), 255, 255, 255);
		}

		if (BossClearTimer > Delay && BossClear[9]) {
			PLAY_SOUND_ONCE(L"Docheol/UI_Stage clear.wav", CHANNELID::SOUND_EFFECT10);

			static_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_BREAK"))->Set_All_Visible(TRUE);
			static_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_CHARGE"))->Set_All_Visible(TRUE);
			static_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_MARK"))->Set_All_Visible(TRUE);
			static_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_LINE"))->Set_All_Visible(TRUE);

			BossClear[9] = FALSE;
		}
		else if (BossClearTimer > Delay + 6.f) {
			memset(BossClear, TRUE, sizeof(BossClear));
			BossClearTimer = 0.f;
			Enable_BossClearUI = FALSE;
		}
	}
}

VOID MainUI::Display_Tutorial(CONST FLOAT& _DT) {
	if (Enable_Tutorial) {
		Tutorial_Timer += _DT;

		
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
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Token.png", L"Token3", 170.f, 669.f, 33.f, 29.f, TRUE);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// INTERACT /////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/KEY_E.png", L"KEY_E", 720.f, 590.f, 35, 35, FALSE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Interaction_BG.png", L"Interaction_BG", 700.f, 590.f, 215, 35, FALSE, 155);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// WEAPON /////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/WeaponBG_Arrow.png", L"WeaponBG_Arrow", 1166.f, 580.f, 108, 108, TRUE, 150);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/WeaponBG_ArrowCount.png", L"WeaponBG_ArrowCount", 1167.f, 681.f, 108, 30, TRUE, 150);

	Component_Sprite->Import_Sprite(L"../../UI/Weapon_UI/FairyBow_UI.png", L"FairyBow_IMG", 1173.f, 586.f, 90, 90, TRUE, 150);
	Component_Sprite->Import_Sprite(L"../../UI/Weapon_UI/IceBow_UI.png", L"IceBow_IMG", 1174.f, 589.f, 85, 85, FALSE, 150);
	Component_Sprite->Import_Sprite(L"../../UI/Weapon_UI/EvilHeadBow_UI.png", L"EvilHeadBow_IMG", 1175.f, 589.f, 90, 90, FALSE, 150);
	Component_Sprite->Import_Sprite(L"../../UI/Weapon_UI/IRABow_UI.png", L"IRABow_IMG", 1173.f, 588.f, 95, 95, FALSE, 150);
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
	UIManager::GetInstance()->Add_GlobalObject(FilterOBJ);
	Component_Sprite->Import_Sprite(L"../../UI/Filter_Fade.png", L"BossClearBG", 0, 0, 1280, 720, TRUE, 0);
	UIManager::GetInstance()->Add_GlobalObject(Component_Sprite->Get_Texture(L"BossClearBG"));
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/ItemNoticeBG.png", L"ItemNoticeBG", 1300.f, 320.f, 300, 40, TRUE, 0);


	Component_Sprite->Import_Sprite(L"../../UI/MainUI/DarkBow.png"		, L"DIC_InfoFrame_DarkBow"		, 1300.f, 310.f, 60, 60, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/GreenBow.png"		, L"DIC_InfoFrame_GreenBow"		, 1300.f, 310.f, 60, 60, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/IceBow.png"		, L"DIC_InfoFrame_IceBow"		, 1300.f, 310.f, 60, 60, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/MichaelBow.png"	, L"DIC_InfoFrame_MichaelBow"	, 1300.f, 310.f, 60, 60, FALSE, 0);
								  	 																									
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_Item1.png"	, L"DIC_InfoFrame_Relic_Item1"	, 1300.f, 310.f, 60, 60, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_Horcrux.png", L"DIC_InfoFrame_Relic_Horcrux", 1300.f, 310.f, 60, 60, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_quiver.png"	, L"DIC_InfoFrame_Relic_quiver"	, 1300.f, 310.f, 60, 60, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_Glove.png"	, L"DIC_InfoFrame_Relic_Glove"	, 1300.f, 310.f, 60, 60, FALSE, 0);
								  	  																							
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Token.png"		, L"DIC_InfoFrame_Token"		, 1300.f, 310.f, 60, 60, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/ArrowFill.png"	, L"DIC_InfoFrame_ArrowFill"	, 1300.f, 310.f, 60, 70, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Health.png"		, L"DIC_InfoFrame_Health"		, 1300.f, 310.f, 60, 60, FALSE, 0);

	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_Item1.png", L"Relic_Item1", 1300.f, 290.f, 80, 80, TRUE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_Item2.png", L"Relic_Item2", 1300.f, 290.f, 80, 80, TRUE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Relic_Item3.png", L"Relic_Item3", 1300.f, 290.f, 80, 80, TRUE, 0);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// Relic_BackGround////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_UI_InGame_Perk_GageBar.png", L"Relic_Bar", 600.f, 290.f, 80, 80, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_UI_InGame_Perk_IconBackground.png", L"Relic_BackGround", 600.f, 290.f, 80, 80, FALSE, 0);
	//////////////////////////////////////////////// RelicUI ////////////////////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_PerkIcon_1-15.png", L"Relic_Item1", 720.f, 290.f, 80, 80, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_PerkIcon_1-26.png", L"Relic_Item2", 780.f, 290.f, 80, 80, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_PerkIcon_1-02.png", L"Relic_Item3", 600.f, 290.f, 80, 80, FALSE, 0);
	Component_Sprite->Import_Sprite(L"../../UI/MainUI/Spr_PerkIcon_1-05.png", L"Relic_Item4", 600.f, 290.f, 80, 80, FALSE, 0);

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
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::TOKEN_EFFECT,L"TOKEN_EFFECT3", 138.f, 630.f, 100, 100, 0.6f, 255);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	EffectManager::GetInstance()->Add_GlobalEffect(UIEffect::Create(GRPDEV, L"CLEAR_BREAK", MAIN_UI_EFFECT::CLEAR_BREAK,
		{ WINCX / 2 - 256 / 2, WINCY / 2 - 400 / 2 + 15, 0.f }, { 256, 400, 0.f }, 255, FALSE, 1.25f));
	EffectManager::GetInstance()->Add_GlobalEffect(UIEffect::Create(GRPDEV, L"CLEAR_CHARGE", MAIN_UI_EFFECT::CLEAR_CHARGE,  
		{ WINCX / 2 - 256 / 2,  WINCY / 2 - 256 / 2 - 40, 0.f }, { 256, 256, 0.f }, 255, FALSE, 1.00f));
	EffectManager::GetInstance()->Add_GlobalEffect(UIEffect::Create(GRPDEV, L"CLEAR_MARK", MAIN_UI_EFFECT::CLEAR_MARK,
		{ WINCX / 2 - 350 / 2, WINCY / 2 - 475 / 2 + 20, 0.f }, { 350, 475, 0.f }, 255, FALSE, 1.00f));
	EffectManager::GetInstance()->Add_GlobalEffect(UIEffect::Create(GRPDEV, L"CLEAR_LINE", MAIN_UI_EFFECT::CLEAR_LINE,
		{ WINCX / 2 - 512 / 2, WINCY / 2 - 116 / 2 - 48, 0.f }, { 512, 116, 0.f }, 255, FALSE, 0.75f));

	dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_BREAK"))->Set_All_Visible(FALSE);
	dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_CHARGE"))->Set_All_Visible(FALSE);
	dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_MARK"))->Set_All_Visible(FALSE);
	dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Find_GlobalEffect(L"CLEAR_LINE"))->Set_All_Visible(FALSE);

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
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 1550.f, 340.f }, 12, L"ItemClass",			L"08서울한강체 L",	D3DCOLOR_ARGB(200, 255, 255, 255)));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// SPEECH ///////////////////////////////////////////////////////
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 140.f, 550.f + 30.f }, 13, L"TifNotice_Text",					L"08서울한강체 L",	D3DCOLOR_ARGB(0, 255, 255, 255), 100, TRUE, DT_LEFT);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 615.091,	5.f }, 15, L"Boss_Name",		L"08서울한강체 L",	D3DCOLOR_ARGB(0, 255, 255, 255), 700));
	AllFontOBJ.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 616.967f, 25.f }, 13, L"Boss_Tag",			L"08서울한강체 L",	D3DCOLOR_ARGB(0, 160, 160, 160)));
	Title_Name = UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 30.f, 270.f }, 65, L"Boss_Title_Name",				L"08서울한강체 L",		D3DCOLOR_ARGB(0, 255, 255, 255), 600, TRUE, DT_LEFT);
	Title_Tag = UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 45.f, 370.f }, 32, L"Boss_Title_Tag",				L"08서울한강체 L",	D3DCOLOR_ARGB(0, 255, 255, 255), 600, TRUE, DT_LEFT);
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"근원 파괴", { 643.f, 298.f }, 40, L"Destroyed",						L"08서울한강체 L", D3DCOLOR_ARGB(0, 255, 255, 255));
	UIManager::GetInstance()->Add_GlobalObject(UIManager::GetInstance()->Find_FontObject(L"Destroyed"), L"Destroyed");
	return S_OK;
}
VOID MainUI::PopUp_ItemInfo(ItemINFO* Item, FLOAT _DT) {
	if (Enable_ItemPopUp) {
		if (ItemINFOSetting) {
			BackGround = Component_Sprite->Get_Texture(L"ItemNoticeBG");
			BackGround->Set_Visible(TRUE);
			ITEM = Component_Sprite->Get_Texture(PopUpSpriteTag);

			ITEM->Set_Visible(TRUE);

			InfoFont = UIManager::GetInstance()->Find_FontObject(L"ItemInfo");
			ClassFont = UIManager::GetInstance()->Find_FontObject(L"ItemClass");

			InfoFont->Set_Text(Item->ItemDesc[ItemINFO::ITINFO::NAME]);
			ClassFont->Set_Text(Item->ItemDesc[ItemINFO::ITINFO::CLASS]);

			if		(ClassFont->Text == L"소모품")												{ ClassFont->Set_Color(0, 255, 101, 14);	}
			else if (ClassFont->Text == L"무기/희귀" || ClassFont->Text == L"유물/희귀")		{ ClassFont->Set_Color(0, 63, 72, 205);		}
			else if (ClassFont->Text == L"무기/노말" || ClassFont->Text == L"아티펙트/노말")	{ ClassFont->Set_Color(0, 255, 255, 255);	}
			else																				{ ClassFont->Set_Color(0, 255, 255, 255);	}

			ItemINFOSetting = FALSE;
		}
		
		if (ITEM->KEY != Item->ItemDesc[ItemINFO::ITINFO::INFFRAME]) {
			ItemPopUp_Timer = 0.f;

			Component_Sprite->Get_Texture(ITEM->KEY)->Set_Opacity(0);
			Component_Sprite->Get_Texture(ITEM->KEY)->Set_Pos(1300.f, 320.f);

			PopUpSpriteTag = Item->ItemDesc[ItemINFO::ITINFO::INFFRAME];

			ITEM = nullptr;
			ITEM = Component_Sprite->Get_Texture(Item->ItemDesc[ItemINFO::ITINFO::INFFRAME]);
			ITEM->Set_Visible(TRUE);
			ITEM->Set_Opacity(0);
			ITEM->Set_Pos(1300.f, 310.f);

			InfoFont->Set_Text(Item->ItemDesc[ItemINFO::ITINFO::NAME]);
			InfoFont->Set_Pos(1430.f, 330.f);
			InfoFont->Set_Color(0, 255, 255, 255);

			ClassFont->Set_Text(Item->ItemDesc[ItemINFO::ITINFO::CLASS]);
			ClassFont->Set_Pos(1550.f, 340.f);
			ClassFont->Set_Color(0, 255, 255, 255);

			if		(ClassFont->Text == L"소모품")												{ ClassFont->Set_Color(0, 255, 101, 14);	}
			else if (ClassFont->Text == L"무기/희귀" || ClassFont->Text == L"유물/희귀")		{ ClassFont->Set_Color(0, 63, 72, 205);		}
			else																				{ ClassFont->Set_Color(0, 255, 255, 255);	}
			
			BackGround->Set_Pos(1300.f, 320.f);
			BackGround->Set_Opacity(0);
		}
		
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
			ClassFont->Set_Pos(1550.f, ClassFont->Get_Pos().y);

			BackGround->Set_Opacity(0);
			ITEM->Set_Opacity(0);
			InfoFont->Set_Color(0, 255, 255, 255);
			ClassFont->Set_Color(0, 255, 255, 255);

			Safe_Release(Item->TEXTURE);
			Safe_Delete(Item);
			Item = nullptr;

			ItemPopUp_Timer = 0.f;
			Enable_ItemPopUp = FALSE;
			ItemINFOSetting = TRUE;
		}
	}
}
VOID MainUI::Set_EnableItemPopUP(BOOL _POP, ItemINFO* _IT, wstring _SPR) {
	Enable_ItemPopUp = _POP;
	PopUpItem = _IT;
	PlayerInven* PI = dynamic_cast<PlayerInven*>(SceneManager::GetInstance()->Get_GameObject(L"PlayerInven"));
	PopUpSpriteTag = _SPR;
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
VOID	MainUI::Free() {
	Safe_Delete(Title_Name);
	Safe_Delete(Title_Tag);
	Safe_Delete(HPBarFill);
	Safe_Delete(BossTitleBar);
	Safe_Release(BossHPSprite);
	GameObject::Free();
}