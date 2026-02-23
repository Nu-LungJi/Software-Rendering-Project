#include "../Include/PCH.h"
#include "Augment.h"

Augment::Augment(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Augment::Augment(CONST GameObject& _RHS)		: GameObject(_RHS)		{}
Augment::~Augment()													{}

HRESULT	Augment::Ready_GameObject() {
	
	Frame = 0;
  Timer = 0.f;

	if (FAILED(Component_Initialize()))		return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;
	if (FAILED(Perk_Initialize()))			return E_FAIL;
	if (FAILED(Effect_Initialize()))		return E_FAIL;
	
	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));
	
	return S_OK;
}
INT		Augment::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);

//	if (KEY_DOWN(DIK_LCONTROL) && KEY_DOWN(DIK_Q))
//	{
//		isActive = !isActive;
//		if (!isActive)
//		{
//			UIManager::GetInstance()->Find_FontObject(L"PERK_TITLE")->Visible = FALSE;
//			UIManager::GetInstance()->Find_FontObject(L"PERK_INFO")->Visible = FALSE;
//    }
//		if (isActive)
//		{
//			switch (IsMouseOnPerk())
//			{
//			case INIT:
//        Perk_Text[0]->Text = Perk_Info[1]->ItemDesc[1];
//				break;
//			case FIRST:
//        Perk_Text[1]->Text = Perk_Info[1]->ItemDesc[0];
//        Perk_Text[1]->Text = Perk_Info[1]->ItemDesc[1];
//			case SECOND:
//				Perk_Text[2]->Text = Perk_Info[2]->ItemDesc[0];
//				Perk_Text[2]->Text = Perk_Info[2]->ItemDesc[1];
//        break;
//			case THIRD:
//        Perk_Text[3]->Text = Perk_Info[3]->ItemDesc[0];
//        Perk_Text[3]->Text = Perk_Info[3]->ItemDesc[1];
//				break;
//			default:
//				break;
//			}
//		}

		if (KEY_DOWN(DIK_LCONTROL) && KEY_DOWN(DIK_Q)) {
			isActive = !isActive;
			if (!isActive) {
				for (auto& Txt : Perk_Text) Txt->Visible = FALSE;
				PlayerObject->Set_PlayerStop(FALSE);
			}
		}
		if (isActive) {
			PlayerObject->Set_PlayerStop(TRUE);
			INT iType = IsMouseOnPerk();

			Display_PerkInfo(Perk_Info[iType]);

			if(iType == FIRST || iType == SECOND || iType == THIRD)
				Perk_Selected_Effect(iType);
			else
        Perk_Selected_Effect(INIT);
		}
		
		return 0;
}
VOID	Augment::LateUpdate_GameObject(CONST FLOAT& _DT) {

}
VOID	Augment::Render_GameObject() {
	if(isActive)
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
  //////////////////////////////////////EFFECT//////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Flash1.png", L"Perk_Effect1", 0.f, 230.f, 140, 140, TRUE, 225);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Flash2.png", L"Perk_Effect2", 0.f, 230.f, 140, 140, TRUE, 225);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Flash3.png", L"Perk_Effect3", 0.f, 230.f, 140, 140, TRUE, 225);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Flash4.png", L"Perk_Effect4", 0.f, 230.f, 140, 140, TRUE, 225);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Flash5.png", L"Perk_Effect5", 0.f, 230.f, 140, 140, TRUE, 225);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Flash6.png", L"Perk_Effect6", 0.f, 230.f, 140, 140, TRUE, 225);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Flash7.png", L"Perk_Effect7", 0.f, 230.f, 140, 140, TRUE, 225);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Flash8.png", L"Perk_Effect8", 0.f, 230.f, 140, 140, TRUE, 225);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Flash9.png", L"Perk_Effect9", 0.f, 230.f, 140, 140, TRUE, 225);
	//////////////////////////////////////PERK//////////////////////////////////////
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_PerkIcon_1-04.png", L"Perk_01", 340.f, 270.f, 80.f, 80.f, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_PerkIcon_1-05.png", L"Perk_02", 590.f, 260.f, 108.f, 108.f, TRUE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/Augments/Spr_PerkIcon_1-30.png", L"Perk_03", 820.f, 260.f, 108.f, 108.f, TRUE, 255);
	return S_OK;
}

HRESULT Augment::Text_Initialize() {
	///////////////////////////////////////FONT//////////////////////////////////
	Perk_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 615.f, 415.f }, 18, L"PERK_TITLE", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	Perk_Text.push_back(UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 615.f, 500.f }, 20, L"PERK_INFO", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255)));
	
	return S_OK;
}

HRESULT Augment::Effect_Initialize()
{
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect1", 38.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect2", 38.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect3", 38.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect4", 38.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect5", 38.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect6", 38.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect7", 38.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect8", 38.f, -30.f, 75, 75, 0.75f, 255);
	PLAY_UI_EFFECT_ONCE(MAIN_UI_EFFECT::AUGMENT_EFFECT, L"Perk_Effect9", 38.f, -30.f, 75, 75, 0.75f, 255);
	
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
	pPerk3->ItemDesc = { L"정확한 초점",L"차지 공격력이 15% 강화됩니다." };
  Perk_Info.push_back(pPerk3);

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
	switch (_PerkType)
	{
	case FIRST: 
		Anim_PerkSelect[Frame]->Set_Pos(340.f, 270.f);
    Effect_Animation(Anim_PerkSelect[Frame], 0.1f, 9, false);
		break;
  case SECOND:
		Anim_PerkSelect[Frame]->Set_Pos(590.f, 270.f);
    Effect_Animation(Anim_PerkSelect[Frame], 0.1f, 9, false);
		break;
  case THIRD:
		Anim_PerkSelect[Frame]->Set_Pos(820.f, 270.f);
    Effect_Animation(Anim_PerkSelect[Frame], 0.1f, 9, false);
		break;
	}
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

void Augment::Effect_Animation(SpriteINFO* _Sprite, float delay, int maxIdx, bool reverse)
{

	if (!reverse)
	{
		if (FrameTick > delay)
		{
			if (++Frame > maxIdx )
				Frame = 1;
			FrameTick = 0.f;
		}
	}
	else
	{
		if (FrameTick > delay)
		{
			if (--Frame < 1)
				Frame = maxIdx;
			FrameTick = 0.f;
		}
	}

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
VOID  Augment::Free() {
	for (auto& PI : Perk_Info)
		Safe_Delete(PI);
	GameObject::Free();
}