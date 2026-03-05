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
	POSX = -600.f;
	POSY = -600.f;
	Frame = 1;
	FrameTimer = 0.f;
	FrameDelay = 0.1f;
	SizeRatio = 2.f;
	MoveSpeed = 20.f;
	Timer = 0.f;
	Start = false;

	BackGround = nullptr;
	_BlackScreen = nullptr;

	if (FAILED(Component_Initialize()))		return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;

	return S_OK;
}

INT EndingCredit::Update_GameObject(const FLOAT& _DT)
{
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	Player* PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));

	if (KEY_DOWN(DIK_M)) {
		UIManager::GetInstance()->Find_FontObject(L"KeyCountText")->Set_Visible(TRUE);
		UIManager::GetInstance()->Find_FontObject(L"CoinCountText")->Set_Visible(TRUE);
		UIManager::GetInstance()->Find_FontObject(L"CrystalCountText")->Set_Visible(TRUE);
		UIManager::GetInstance()->Find_FontObject(L"ArrowCountText")->Set_Visible(TRUE);
		m_bCreditStart ? m_bCreditStart = FALSE : m_bCreditStart = TRUE;
		m_bCreditStart ? PlayerObject->Set_PlayerStop(TRUE) : PlayerObject->Set_PlayerStop(FALSE);
	}

	if (m_bCreditStart) {
		Timer += _DT;
		FrameTimer += _DT;
		
		if (!Start) Update_Ready();

		// 크기 조절
		if (Timer < 5.f) {
			POSX += MoveSpeed * _DT;
			POSX = min(POSX, -200);
			if (BackGround != nullptr)
				BackGround->Set_Pos(POSX, POSY);
		}

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
	



	return S_OK;
}

HRESULT EndingCredit::Update_Ready()
{
	_BlackScreen = Component_Sprite->Get_Texture(L"BlackScreen");
	_BlackScreen->Set_Visible(TRUE);

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

