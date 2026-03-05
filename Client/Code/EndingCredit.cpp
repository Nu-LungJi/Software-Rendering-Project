#include "../Include/PCH.h"
#include "EndingCredit.h"

EndingCredit::EndingCredit(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
EndingCredit::EndingCredit(CONST GameObject& _RHS) : GameObject(_RHS) {}
EndingCredit::~EndingCredit() {}

HRESULT EndingCredit::Ready_GameObject()
{
	m_bCreditStart = false;
	WIDTH = 1884.f;
	HEIGHT = 1921.f;
	POSX = -350.f;
	POSY = -600.f;
	Frame = 1;
	FrameTimer = 0.f;
	FrameDelay = 0.1f;
	SizeRatio = 2.f;
	MoveSpeed = 30.f;
	Timer = 0.f;
	Start = false;
	FramePosX = WINCX - 50.f;
	FramePosY = 0;
	FrameSpeed = 30.f;
	WhiteScreenOpacity = 255;

	Credit1_Timer = 0.f;
	Credit2_Timer = 0.f;

	BackGround = nullptr;
	BlackFont_Frame = nullptr;
	_BlackScreen = nullptr;
	_WhiteScreen = nullptr;

	if (FAILED(Component_Initialize()))		return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;

	return S_OK;
}

INT EndingCredit::Update_GameObject(const FLOAT& _DT)
{
	Player* PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));

	if (KEY_DOWN(DIK_M)) {
		m_bCreditStart ? m_bCreditStart = FALSE : m_bCreditStart = TRUE;
		m_bCreditStart ? PlayerObject->Set_PlayerStop(TRUE) : PlayerObject->Set_PlayerStop(FALSE);
	}

	if (m_bCreditStart) {
		{
			PlayerObject->Set_PlayerStop(FALSE);
			UIManager::GetInstance()->Find_FontObject(L"KeyCountText")->Set_Visible(FALSE);
			UIManager::GetInstance()->Find_FontObject(L"CoinCountText")->Set_Visible(FALSE);
			UIManager::GetInstance()->Find_FontObject(L"CrystalCountText")->Set_Visible(FALSE);
			UIManager::GetInstance()->Find_FontObject(L"ArrowCountText")->Set_Visible(FALSE);
		}

		Timer += _DT;
		FrameTimer += _DT;

		GameObject::Update_GameObject(_DT);
		RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);
		
		if (!Start) Update_Ready();

		// 집 위치 조절
		if (Timer < 5.f) {
			POSX += MoveSpeed * _DT;
			POSX = min(POSX, -150);
			if (BackGround != nullptr)
				BackGround->Set_Pos(POSX, POSY);

			// 화이트 스크린
			WhiteScreenOpacity -= _DT * 100.f;
			WhiteScreenOpacity = max(0, WhiteScreenOpacity);
			_WhiteScreen->Set_Opacity(WhiteScreenOpacity);
		}

		if (POSX >= -150) {
			// 블랙 프레임
			FrameSpeed += _DT * 300.f;
			FrameSpeed = min(255, FrameSpeed);
			BlackFont_Frame->Set_Opacity(FrameSpeed);
		}

		if (FrameSpeed >= 255) {
			Credit1->Set_Pos(Credit1->Get_Pos().x, Credit1->Get_Pos().y - _DT * 40.f);
		}
		if (Credit1->Get_Pos().y <= WINCY - 520.f) {
			Credit2->Set_Pos(Credit2->Get_Pos().x, Credit2->Get_Pos().y - _DT * 40.f);
		}

		// 텍스트
		//if (FrameSpeed >= 255) {
		//	FontList[0]->Set_Active(TRUE);
		//	FontList[0]->Set_Pos(FontList[0]->Get_Pos().x, FontList[0]->Get_Pos().y - _DT * 40.f);
		//}

		// 집 애니메이션
		if (FrameTimer > FrameDelay) {
			FrameTimer = 0.f;

			if (BackGround != nullptr)
				BackGround->Set_Visible(FALSE);

			if (Frame > 28) Frame = 1;
			TCHAR FileName[128] = L"";
			wsprintfW(FileName, L"EndingCredet_BackGound%d", Frame++);
			BackGround = Component_Sprite->Get_Texture(FileName);

			BackGround->Set_Pos(POSX, POSY);
			BackGround->Set_Visible(TRUE);

			if (Frame < 6 || (Frame >= 15 && Frame <= 20)) FrameDelay = 0.1f;
			else FrameDelay = 0.3f;
		}


	}
	else {
		if (BackGround != nullptr) {
			BackGround->Set_Visible(FALSE);
			_BlackScreen->Set_Visible(FALSE);
		}
			
	}

	return S_OK;
}

VOID EndingCredit::LateUpdate_GameObject(const FLOAT& _DT)
{
}

VOID EndingCredit::Render_GameObject()
{
	if (m_bCreditStart) {
		Component_Sprite->Render_Sprite();
	}
	else {
	}
}


HRESULT EndingCredit::Component_Initialize()
{
	UIManager::GetInstance()->Set_Active(FALSE);
	Component_Sprite = ADD_COMPONENT_SPRITE;

	return S_OK;
}

HRESULT EndingCredit::Text_Initialize()
{
	//FontObject* FO = UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"최광윤", {WINCX - 260.f, WINCY}, 40.f,
	//	L"Tile", L"Bastard", D3DCOLOR_ARGB(255, 255, 255, 255));
	//FO->Set_Active(false);
	//FontList.push_back(FO);
	//
	//FO = UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"최광윤", { WINCX - 260.f, WINCY }, 40.f,
	//	L"EndingName", L"Bastard", D3DCOLOR_ARGB(255, 255, 255, 255));

	return S_OK;
}

HRESULT EndingCredit::Sprite_Initialize()
{
	// ***************BACKGROUND*******************
	Component_Sprite->Import_Sprite(L"../../UI/EndingCredit/BlackScreen.png", L"BlackScreen", 0, 0, WINCX, WINCY, FALSE, 255);
	for (int i = 1; i <= 28; i++) {
		TCHAR FileName1[128] = L"";
		TCHAR FileName2[128] = L"";
		wsprintfW(FileName1, L"../../UI/EndingCredit/EndingCredet_BackGound%d.dds", i);
		wsprintfW(FileName2, L"EndingCredet_BackGound%d", i);
		Component_Sprite->Import_Sprite(FileName1, FileName2, POSX, POSY, WIDTH, HEIGHT, FALSE, 255);
	}
	Component_Sprite->Import_Sprite(L"../../UI/EndingCredit/Ending_BlackFrame.png", L"Black_FontFrame", 0, 0, WINCX, WINCY, FALSE, 0);

	Component_Sprite->Import_Sprite(L"../../UI/EndingCredit/WhiteScreen.png", L"WhiteScreen", 0, 0, WINCX, WINCY, FALSE, 255);

	Component_Sprite->Import_Sprite(L"../../UI/EndingCredit/Credit1.png", L"Credit1", WINCX - 520.f, WINCY, 520.f, 520.f, FALSE, 255);
	Component_Sprite->Import_Sprite(L"../../UI/EndingCredit/Credit2.png", L"Credit2", WINCX - 520.f, WINCY, 520.f, 520.f, FALSE, 255);

	return S_OK;
}

HRESULT EndingCredit::Update_Ready()
{
	_BlackScreen = Component_Sprite->Get_Texture(L"BlackScreen");
	_BlackScreen->Set_Visible(TRUE);
	_WhiteScreen = Component_Sprite->Get_Texture(L"WhiteScreen");
	_WhiteScreen->Set_Visible(TRUE);
	BlackFont_Frame = Component_Sprite->Get_Texture(L"Black_FontFrame");
	BlackFont_Frame->Set_Visible(TRUE);
	Credit1 = Component_Sprite->Get_Texture(L"Credit1");
	Credit1->Set_Visible(TRUE);
	Credit2 = Component_Sprite->Get_Texture(L"Credit2");
	Credit2->Set_Visible(TRUE);

	return S_OK;
}

EndingCredit* EndingCredit::Create(LPDIRECT3DDEVICE9 _GRPDEV)
{
	EndingCredit* MUI = new EndingCredit(_GRPDEV);
	if (FAILED(MUI->Ready_GameObject())) {
		MSG_BOX("Cannot Create EndingCredit.");
		Safe_Release(MUI);
		return nullptr;
	}
	return MUI;
}

VOID EndingCredit::Free()
{
	return VOID();
}

