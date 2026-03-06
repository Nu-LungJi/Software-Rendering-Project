#include "../Include/PCH.h"
#include "Augment.h"

Augment::Augment(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Augment::Augment(CONST GameObject& _RHS)		: GameObject(_RHS)		{}
Augment::~Augment()													{}

HRESULT	Augment::Ready_GameObject() {
	if (FAILED(Component_Initialize()))	return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;
	if (FAILED(Perk_Initialize()))			return E_FAIL;
	fDelay = 0;
	bdelay = false;
	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));
	isActive = true;
	return S_OK;
}
INT		Augment::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);

		PlayerObject->Set_PlayerStop(TRUE);
		Perk_Text[2]->Text = L"가호 선택";
		Perk_Text[2]->Visible = TRUE;

		INT iType = IsMouseOnPerk();
		Display_PerkInfo(Perk_Info[iType]);

		if (iType != m_iPrevHoverType) {
			wstring prevAnim = L"Perk_Effect" + to_wstring(m_iPrevHoverType);
			if (iType != INIT)
				Perk_Selected_Effect(iType);

			UIEffect* pPrevEffect = dynamic_cast<UIEffect*>(EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, prevAnim));
			if (pPrevEffect) {
				pPrevEffect->Set_All_Visible(FALSE);
				pPrevEffect->Set_ObjectDead(TRUE);
			}
			m_iPrevHoverType = iType;
		}

		fDelay += _DT;
		
		if (fDelay > 2.f)
			bdelay = true;

		if (bdelay &&GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
			PlayerObject->Set_PlayerStop(FALSE);
			if (iType != INIT) {
				Add_PlayerStatus(iType);
				SoundManager::GetInstance()->Play_Sound_Once(L"UI/Apostle/UI_Menu_ChooseApostle_Select.wav", CHANNELID::SOUND_EFFECT05, 0.6f);
				dynamic_cast<MiniGameCounter*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"DefenseUI"))->Set_EndWave();
				Set_ObjectDead(TRUE);
				ClearAllEffects();
				for (auto& Txt : Perk_Text) Txt->Visible = FALSE;
				//PlayerObject->Set_PlayerStop(FALSE);

				return 0;
			}
		
		}

	if (!isActive) {
		for (auto& Txt : Perk_Text) Txt->Visible = FALSE;
		//if (PlayerObject) PlayerObject->Set_PlayerStop(FALSE);
	}
		return 0;
}
VOID	Augment::LateUpdate_GameObject(CONST FLOAT& _DT) {

}
VOID	Augment::Render_GameObject() {

		Component_Sprite->Render_Sprite();	
}

HRESULT Augment::Component_Initialize() {
	Component_Sprite		= ADD_COMPONENT_SPRITE;
	Component_Texture	  = ADD_COMPONENT_TEXTURE;
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	
	return S_OK;
}
HRESULT Augment::Sprite_Initialize() {
	///////////////////////////////APOSTLE_BACKGROUND///////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_Apostle_BackGroun.png", L"Apostle_BackGround", 0.f, 0.f, 1280, 960, TRUE, 155);
	///////////////////////////////APOSTLE_EFFECT///////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_SelectBress_Title.png", L"Apostle", 300.f, 30.f, 626, 192, TRUE, 255);
	/////////////////////////////////////BACKGROUND_TOP_BOTTOM//////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_ApostleLiberation_Background_Top.png", L"Y", 270.f, 400.f, 700, 80, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_ApostleLiberation_Background_Bottom.png", L"D", 270.f, 480.f, 700, 140, TRUE, 255);
	///////////////////////////////////BACKGROUND///////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Background.png", L"Perk_BackGround1", 300.f, 230.f, 140, 140, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Background.png", L"Perk_BackGround2", 560.f, 230.f, 140, 140, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Background.png", L"Perk_BackGround3", 800.f, 230.f, 140, 140, TRUE, 255);
	///////////////////////////////////////FRAME////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Frame.png", L"Perk_Frame1", 300.f, 230.f, 140, 140, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Frame.png", L"Perk_Frame2", 560.f, 230.f, 140, 140, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_Ui_LevelV2_Frame.png", L"Perk_Frame3", 800.f, 230.f, 140, 140, TRUE, 255);
	//////////////////////////////////////PERK//////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_PerkIcon_1-04.png", L"Perk_01", 333.f, 255.f, 80.f, 80.f, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_PerkIcon_1-05.png", L"Perk_02", 580.f, 247.f, 108.f, 108.f, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_PerkIcon_1-30.png", L"Perk_03", 816.f, 250.f, 108.f, 108.f, TRUE, 255);
	return S_OK;
}

HRESULT Augment::Text_Initialize() {
	///////////////////////////////////////FONT//////////////////////////////////
	Perk_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 615.f, 415.f }, 18, L"PERK_TITLE", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	Perk_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 615.f, 500.f }, 20, L"PERK_INFO", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	Perk_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 615.f, 115.f }, 25, L"PERK_SELECT", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	
	return S_OK;
}


HRESULT Augment::Perk_Initialize()
{
  ItemINFO* pPerk0 = new ItemINFO;
	pPerk0->ItemDesc = { L"",L"가호를 선택하세요 " };
  Perk_Info.push_back(pPerk0);

	ItemINFO* pPerk1 = new ItemINFO;
	pPerk1->ItemDesc = { L"신속한 사냥",L"공격속도가 50% 빨라집니다." };
  Perk_Info.push_back(pPerk1);

	ItemINFO* pPerk2 = new ItemINFO;
  pPerk2->ItemDesc = { L"강화된 활시위",L"일반 공격이 20% 강화됩니다." };
  Perk_Info.push_back(pPerk2);

	ItemINFO* pPerk3 = new ItemINFO;
	pPerk3->ItemDesc = { L"정확한 초점",L"치명타 확률이 50% 증가됩니다." };
  Perk_Info.push_back(pPerk3);

	return S_OK;
}

HRESULT Augment::Add_PlayerStatus(INT _PerkType)
{
	switch (_PerkType)
	{
		case FIRST:
			PlayerObject->Set_AttackSpeed(*PlayerObject->Get_AttackSpeed() * 1.25f);
      break;
		case SECOND:
			PlayerObject->Set_Atk(*PlayerObject->Get_AttackSpeed() * 1.5f);
      break;
		case THIRD:
			PlayerObject->Set_Critical(*PlayerObject->Get_Critical() * 1.5f);
      break;
	}
	return S_OK;
}

VOID Augment::Display_PerkInfo(ItemINFO* _pPerk)
{
	if (isActive)
	{
		if (nullptr == _pPerk) {
			for (auto& Txt : Perk_Text) Txt->Set_Visible(FALSE);
			for (auto& Comp : PerkInfo_Screen) Comp->Set_Visible(FALSE);
		}
		for (auto& Txt : Perk_Text) Txt->Set_Visible(TRUE);
		for (auto& Comp : PerkInfo_Screen) Comp->Set_Visible(TRUE);
			Perk_Text[0]->Text = _pPerk->ItemDesc[0];
			Perk_Text[1]->Text = _pPerk->ItemDesc[1];
	}
	else
		return;
}

VOID Augment::Perk_Selected_Effect(INT _PerkType)
{  
	if (_PerkType == FIRST){
		PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect1", 285.f, 233.f, 175, 150, 2.0f, 200);
		SoundManager::GetInstance()->Play_Sound_Once(L"UI/Apostle/UI_Bless_Choice_01.wav", CHANNELID::SOUND_EFFECT08, 0.4f);		
	}
	if (_PerkType == SECOND) {
		PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect2", 545.f, 233.f, 175, 150, 2.0f, 200);
		SoundManager::GetInstance()->Play_Sound_Once(L"UI/Apostle/UI_Bless_Choice_02.wav", CHANNELID::SOUND_EFFECT08, 0.4f);
	}
  if (_PerkType == THIRD){
		PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect3", 785.f, 233.f, 175, 150, 2.0f, 200);
		SoundManager::GetInstance()->Play_Sound_Once(L"UI/Apostle/UI_Bless_Choice_03.wav", CHANNELID::SOUND_EFFECT08, 0.4f);
	}
}

VOID Augment::FadeOut(FLOAT Frame)
{
	return VOID();
}

INT Augment::IsMouseOnPerk()
{
	POINT MousePoint{ 0, 0 };
	GetCursorPos(&MousePoint);
	ScreenToClient(hWnd, &MousePoint);

	if (MousePoint.y >= 170 && MousePoint.y <= 370)
	{
		if (MousePoint.x >= 160 && MousePoint.x <= 440)
		{
			return FIRST;
		}
		if(MousePoint.x >= 420 && MousePoint.x <= 700)
		{
			return SECOND;
		}
		if(MousePoint.x >= 620 && MousePoint.x <= 940)
		{
			return THIRD;
		}
	}

	return INIT;
}

Augment* Augment::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Augment* MUI = new Augment(_GRPDEV);
	if (FAILED(MUI->Ready_GameObject())) {
		MSG_BOX("Cannot Create Augments.");
		Safe_Release(MUI);
		return nullptr;
	}
	return MUI;
}

VOID Augment::ClearAllEffects() {
	for (int i = 1; i <= 3; ++i) {
		wstring animName = L"Perk_Effect" + to_wstring(i);
		GameObject* pEffect = EffectManager::GetInstance()->Get_Effect(EFFECT_OWNER::UI, animName);
		if (pEffect) pEffect->Set_ObjectDead(TRUE);
	}
}

VOID  Augment::Free() {
	for (auto& PI : Perk_Info)
	{
		Safe_Release(PI->TEXTURE);
		Safe_Delete(PI);
	}

  isActive = FALSE;

	GameObject::Free();
}