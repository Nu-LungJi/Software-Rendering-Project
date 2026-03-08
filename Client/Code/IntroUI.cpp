#include "../Include/PCH.h"
#include "IntroUI.h"

IntroUI:: IntroUI(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
IntroUI:: IntroUI(CONST GameObject& _RHS)		: GameObject(_RHS)		{}
IntroUI::~IntroUI()														{}

HRESULT		IntroUI::Ready_GameObject(){
	if (FAILED(Component_Initialize()))		return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;

	D3DXCreateSprite(GRPDEV, &IntroSprite);

	dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"))->Set_PlayerStop(TRUE);
	dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"))->Set_VelocityLock(TRUE);

	//SoundManager::GetInstance()->Play_Sound(L"UI/Intro/Intro_BGM.wav", CHANNELID::SOUND_BGM01, 0.5f);

	Filter = UIManager::GetInstance()->Find_GlobalObject(L"FadeFilter");
	Filter->Set_Opacity(255);
	Filter->Set_Visible(TRUE);

	Enable_FadeIn		= FALSE;
	Enable_FadeOut		= FALSE;
	Enable_MenuBar		= FALSE;
	Enable_GameStart	= FALSE;
	Enable_ClickToStart = FALSE;
	Enable_PlayerSummon = FALSE;

	return S_OK;
}
INT			IntroUI::Update_GameObject(CONST FLOAT& _DT){
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);


	if (Enable_FadeIn == TRUE && Filter->OPACITY > 2) {
		Filter->Set_Opacity(Filter->OPACITY - 2);
		if (Filter->OPACITY < 50) {
			Enable_MenuBar = TRUE;
			Enable_ClickToStart = TRUE;
		}
	}
	else if (Enable_FadeOut == TRUE && Filter->OPACITY < 253) {
		Filter->Set_Opacity(Filter->OPACITY + 2);
	}
	if (Enable_MenuBar && Component_Sprite->Get_Texture(L"MenuBar")->Get_Opacity() < 200) {
		Component_Sprite->Get_Texture(L"MenuBar")->Set_Opacity(Component_Sprite->Get_Texture(L"MenuBar")->Get_Opacity() + 1);
		StartBTN->Set_Color(Component_Sprite->Get_Texture(L"MenuBar")->Get_Opacity(), 255, 255, 255);
	}
	
	if (KEY_DOWN(DIK_2)) {			// 시작 버튼
		if (Enable_FadeIn == FALSE) {
			static_cast<StartScene*>(SceneManager::GetInstance()->Get_CurrentScene())->Set_BGMPlayer(2);
			UIManager::GetInstance()->Find_FontObject(L"KeyCountText")->Set_Visible(FALSE);
			UIManager::GetInstance()->Find_FontObject(L"CoinCountText")->Set_Visible(FALSE);
			UIManager::GetInstance()->Find_FontObject(L"CrystalCountText")->Set_Visible(FALSE);
			UIManager::GetInstance()->Find_FontObject(L"ArrowCountText")->Set_Visible(FALSE);
			Enable_FadeIn = TRUE;
			Enable_FadeOut = FALSE;
		}
		return 0;
	}
	if (Enable_ClickToStart && MOUSE_LBUTTON) {
		Enable_FadeOut = TRUE;
		Enable_FadeIn = FALSE;
		Enable_GameStart = TRUE;
	}
	if (Enable_GameStart) {
		FadeTimer += _DT;
		SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM01, 0.5f - FadeTimer / 10);
		if (FadeTimer > 5.f) {
			Enable_GameStart = FALSE;
			Enable_PlayerSummon = TRUE;
			FadeTimer = 0.f;
			dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"))->Set_PlayerStop(FALSE);
			dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"))->Set_VelocityLock(TRUE);

			UIManager::GetInstance()->Find_FontObject(L"KeyCountText")->Set_Visible(TRUE);
			UIManager::GetInstance()->Find_FontObject(L"CoinCountText")->Set_Visible(TRUE);
			UIManager::GetInstance()->Find_FontObject(L"CrystalCountText")->Set_Visible(TRUE);
			UIManager::GetInstance()->Find_FontObject(L"ArrowCountText")->Set_Visible(TRUE);
			UIManager::GetInstance()->Delete_FontObject(StartBTN);

			static_cast<StartScene*>(SceneManager::GetInstance()->Get_CurrentScene())->Set_BGMPlayer(TRUE);
			dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Set_FadeOption(TRUE, 1.f);

			Component_Sprite->Get_Texture(L"BackGround")->Set_Visible(FALSE);
			Component_Sprite->Get_Texture(L"Logo")->Set_Visible(FALSE);
			Component_Sprite->Get_Texture(L"MenuBar")->Set_Visible(FALSE);
		}
	}
	if (Enable_PlayerSummon) {
		FadeTimer += _DT;
		if (FadeTimer > 7.f) {
			ObjectDead = TRUE;
			dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"))->Set_PlayerSummon(TRUE);
		}
	}
	
	return 0;
}
VOID		IntroUI::LateUpdate_GameObject(CONST FLOAT& _DT){
	GameObject::LateUpdate_GameObject(_DT);
}
VOID		IntroUI::Render_GameObject(){
	Component_Sprite->Render_Sprite();

}

HRESULT IntroUI::Component_Initialize()	{
	Component_Sprite = ADD_COMPONENT_SPRITE;
	
	return S_OK;
}
HRESULT IntroUI::Sprite_Initialize()	{
	wstring BaseFolder = L"../../UI/IntroUI/";
	Component_Sprite->Import_SpriteEX(BaseFolder, L"BackGround.png"	, L"BackGround"	, 0.f, 0.f, 1280, 720, TRUE, 255);
	Component_Sprite->Import_SpriteEX(BaseFolder, L"Logo.png"		, L"Logo"		, 437.f, 335.f, 420, 290, TRUE, 255);
	Component_Sprite->Import_SpriteEX(BaseFolder, L"MenuBar.png"	, L"MenuBar"	, 365.f, 615.f, 557, 43, TRUE, 0);
	return S_OK;
}
HRESULT IntroUI::Text_Initialize()		{
	StartBTN = UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"시작", { 639.f, 627.f }, 22, L"StartBTN", L"08서울한강체 L", D3DCOLOR_ARGB(0, 255, 255, 255), 100, TRUE);
	return S_OK;
}

IntroUI* IntroUI::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	IntroUI* IUI = new IntroUI(_GRPDEV);
	if (FAILED(IUI->Ready_GameObject())) {
		MSG_BOX("Cannot Create IntroUI.");
		Safe_Release(IUI);
		return nullptr;
	}
	return IUI;
}
VOID IntroUI::Start_FadeIn(const FLOAT& _DT) {
	if (Enable_FadeIn == TRUE) {

	}
}
VOID IntroUI::Start_FadeOut(const FLOAT& _DT) {
	if (Enable_FadeOut == TRUE) {

	}
}
VOID	 IntroUI::Free() {
	GameObject::Free();
}