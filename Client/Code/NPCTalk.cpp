#include "../Include/PCH.h"
#include "NPCTalk.h"

NPCTalk::NPCTalk(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV)	{}
NPCTalk::NPCTalk(CONST GameObject& _RHS) : GameObject(_RHS)			{}
NPCTalk::~NPCTalk()													{}

HRESULT		NPCTalk::Ready_GameObject() {
	if (FAILED(Component_Initialize()))		return E_FAIL;
	if (FAILED(Sprite_Initialize()))		return E_FAIL;
	if (FAILED(Effect_Initialize()))		return E_FAIL;
	if (FAILED(Text_Initialize()))			return E_FAIL;
	TalkTimer = 0.f;
	ContextPassing = (INT)NOT_TALKING;

	Enable_Interaction = FALSE;
	AcceptedQuest = FALSE, CompletedQuest = FALSE;

	BackGround = Component_Sprite->Get_Texture(L"NPCTalk_BackGround");
	GRD_Top = Component_Sprite->Get_Texture(L"FrameGradation_Top");
	GRD_Bottom = Component_Sprite->Get_Texture(L"FrameGradation_Bottom");

	Sprite_Yeon = Component_Sprite->Get_Texture(L"NPCTalk_Yeon");
	Sprite_Tif = Component_Sprite->Get_Texture(L"NPCTalk_Tif");
	Sprite_Shop = Component_Sprite->Get_Texture(L"NPCTalk_Shop");

	NameBar = Component_Sprite->Get_Texture(L"NameBar");
	Square = Component_Sprite->Get_Texture(L"TalkPass_Square");

	PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_GameObject(L"Player"));

	Name = UIManager::GetInstance()->Find_FontObject(L"Personal Name");
	Talk = UIManager::GetInstance()->Find_FontObject(L"TALK");

	FOBJVec = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Get_AllFontObject();

	return S_OK;
}
INT			NPCTalk::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDERID::RENDER_UI, this);

	return 0;
}
VOID		NPCTalk::LateUpdate_GameObject(CONST FLOAT& _DT) {

}
VOID		NPCTalk::Render_GameObject() {
	Component_Sprite->Render_Sprite();
}

BOOL NPCTalk::Activate_NPCTalk(NPC_CHARACTER _NPCC, FLOAT _DT) {
	if		(!Enable_Interaction) {
		FadeState = 0;
		PlayerObject->Set_PlayerStop(TRUE);
		static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"))->Set_VelocityLock(TRUE);
	}
	if		(_NPCC == NPC_CHARACTER::NPC_TIF) {
		if (KEY_DOWN(DIK_E) && Enable_Interaction) { ContextPassing++; }
		
		if (ContextPassing == 0){
			Name->Text = L"시간의 사도";
			Talk->Text = L"포근한 기운이 느껴진다..";
		}
		else if (ContextPassing == 1) {
			Name->Text = L"시간의 사도";
			Talk->Text = L"모두 함께 기도합시다...";
		}
		else if (ContextPassing == 2) {
			Name->Text = L"이라";
			Talk->Text = L"...";
		}
		else if (ContextPassing == 3) {
			ContextPassing = 999;
		}
	}
	else if (_NPCC == NPC_CHARACTER::NPC_SHOP) {
		if (ContextPassing == (INT)NOT_TALKING) {
			Name->Text = L"상점 주인";
			Talk->Text = L"어서 오세요 연님! 무엇을 도와드릴까요?";
			ContextPassing = (INT)TALKING_ACTIVE;
		}
		if (ContextPassing == (INT)QUIT_TALK) {
			FadeState = 1;
			PlayerObject->Set_PlayerStop(FALSE);
			static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"))->Set_VelocityLock(FALSE);
			static_cast<CameraObject*>(SceneManager::GetInstance()->Get_GameObject(L"Camera"))->Ready_SmoothCamera(FALSE);

			ContextPassing = (INT)QUEST_END;
		}
		if (ContextPassing == (INT)QUEST_TALK01) {
			Name->Text = L"상점 주인";
			Talk->Text = L"실은 방금 막 들어온 긴급 의뢰가 하나 있어요. 북쪽 숲 근처 농가에서 몬스터 무리가 나타나서  \n식량 창고를 털어갔대요. 마을 사람들에겐 일 년 농사가 걸린 아주 중요한 문제거든요";
		}
		if (ContextPassing == (INT)QUEST_TALK02) {
			Name->Text = L"연";
			Talk->Text = L"에린 씨가 부탁한다면 거절할 수가 없네요.";
		}
		if (ContextPassing == (INT)QUEST_TALK03) {
			Name->Text = L"상점 주인";
			Talk->Text = L"이번 건은 마을 촌장님이 특별히 사비까지 털어서 보상 경험치를 올렸으니까요. \n섭섭지 않게 챙겨드릴게요!";
		}
		if (ContextPassing == (INT)QUEST_TALK_ACCEPT) {
			Name->Text = L"연";
			Talk->Text = L"좋습니다. 보상만 확실하다면 물불 안 가리는 게 모험가죠! \n걱정 말고 기다리세요!";
			dynamic_cast<ShopKeeper*>(SceneManager::GetInstance()->Get_GameObject(L"ShopNPC"))->Set_QuestState(QUESTSTATE::ACCEPTED);
		}
		if (ContextPassing == (INT)QUEST_TALK_DENY) {
			Name->Text = L"연";
			Talk->Text = L"제안은 감사하지만, 지금은 다른 급한 용무가 있어서요. \n준비를 좀 더 갖춘 뒤에 다시 찾아오겠습니다....!";
		}
		if (KEY_DOWN(DIK_E)) {
			if (ContextPassing == (INT)QUEST_TALK01) { ContextPassing = (INT)QUEST_TALK02; }
			if (ContextPassing == (INT)QUEST_TALK02) { ContextPassing = (INT)QUEST_TALK03; }
			if (ContextPassing == (INT)QUEST_TALK_ACCEPT) { ContextPassing = (INT)QUIT_TALK; }
			if (ContextPassing == (INT)QUEST_TALK_DENY) { ContextPassing = (INT)QUIT_TALK; }
		}
		if (KEY_DOWN(DIK_1)) {
			if (ContextPassing == (INT)TALKING_ACTIVE) { ContextPassing = (INT)QUEST_TALK01; }
			if (ContextPassing == (INT)QUEST_TALK03) { ContextPassing = (INT)QUEST_TALK_ACCEPT; }
		}
		if (KEY_DOWN(DIK_2)) {
			if (ContextPassing == (INT)TALKING_ACTIVE) { ContextPassing = (INT)QUIT_TALK; }
			if (ContextPassing == (INT)QUEST_TALK03) { ContextPassing = (INT)QUEST_TALK_DENY; }
		}
	}
	return Shadow_Fade(_DT);
}	

BOOL NPCTalk::Shadow_Fade(CONST FLOAT& _DT) {
	if		(FadeState == 2)		return FALSE;
	if		(FadeState == 0) {			//	FADE IN
		TalkTimer += _DT;
		if (TalkTimer < 1.f) {
			for (auto& FOBJ : FOBJVec)
				FOBJ->Set_Color(255 - TalkTimer * 255 * 2, 255, 255, 255);

			BackGround->Set_Opacity(TalkTimer * 150);
			GRD_Top->Set_Opacity(TalkTimer * 255);
			GRD_Bottom->Set_Opacity(TalkTimer * 255);
			Sprite_Yeon->Set_Opacity(TalkTimer * 255);
			Sprite_Shop->Set_Opacity(TalkTimer * 255);
		}
		else if (TalkTimer > 1.f && TalkTimer < 2.f) {
			NameBar->Set_Visible(TRUE);
			Square->Set_Visible(TRUE);
			Talk->Set_Visible(TRUE);
			Name->Set_Color(255 * (TalkTimer - 1), 255, 255, 255);
			Talk->Set_Color(150 * (TalkTimer - 1), 255, 255, 255);
		}
		else if (TalkTimer > 2.f) {
			Enable_Interaction = TRUE;
			TalkTimer = 0.f;
			FadeState = 2;
			return FALSE;
		}
	}
	else if (FadeState == 1) {		// FADE OUT
		TalkTimer += _DT;
		if (TalkTimer < 1.f) {
			for (auto& FOBJ : FOBJVec) {
				FOBJ->Set_Color(TalkTimer * 255 * 2, 255, 255, 255);
			}
			BackGround	->Set_Opacity(150 - (TalkTimer * 150));
			GRD_Top		->Set_Opacity(255 - (TalkTimer * 255));
			GRD_Bottom	->Set_Opacity(255 - (TalkTimer * 255));
			Sprite_Yeon	->Set_Opacity(255 - (TalkTimer * 255));
			Sprite_Shop	->Set_Opacity(255 - (TalkTimer * 255));
			Name		->Set_Color(255 - 255 * (TalkTimer), 255, 255, 255);
			Talk		->Set_Color(150 - 150 * (TalkTimer), 255, 255, 255);
		}
		else if (TalkTimer > 1.f && TalkTimer < 2.f) {
			NameBar	->Set_Visible(FALSE);
			Square	->Set_Visible(FALSE);
			Talk	->Set_Visible(FALSE);
		}
		else if (TalkTimer > 2.f) {
			Enable_Interaction = FALSE;
			TalkTimer = 0.f;
			FadeState = 2;
			ContextPassing = (INT)NOT_TALKING;
			return TRUE;
		}
	}
}
HRESULT  NPCTalk::Component_Initialize() {
	Component_Sprite = ADD_COMPONENT_SPRITE;
	return S_OK;
}
HRESULT  NPCTalk::Sprite_Initialize() {
	wstring BaseFolder = L"../../UI/NPCDialog/";

	Component_Sprite->Import_SpriteEX(BaseFolder, L"NPCTalk_BackGround.png", L"NPCTalk_BackGround", 0.f, 0.f, 1280, 720, TRUE, 0);
	Component_Sprite->Import_SpriteEX(BaseFolder, L"NPCTalk_Yeon.png", L"NPCTalk_Yeon", -60.f, 100.f, 617, 1221, TRUE, 0);
	Component_Sprite->Import_SpriteEX(BaseFolder, L"FrameGradation.png", L"FrameGradation_Top", -150.f, -1150.f, 1620, 1280, TRUE, 0);
	Component_Sprite->Import_SpriteEX(BaseFolder, L"NPCTalk_Tif.png", L"NPCTalk_Tif", 797.f, 105.f, 520, 1221, TRUE, 0);
	Component_Sprite->Import_SpriteEX(BaseFolder, L"NPCTalk_Shop.png", L"NPCTalk_Shop", 797.f, 105.f, 450, 990, TRUE, 0);
	Component_Sprite->Import_SpriteEX(BaseFolder, L"FrameGradation.png", L"FrameGradation_Bottom", -150.f, 450.f, 1620, 1620, TRUE, 0);
	Component_Sprite->Import_SpriteEX(BaseFolder, L"NameBar.png", L"NameBar", 565.f, 535.f, 150, 10, FALSE, 200);
	Component_Sprite->Import_SpriteEX(BaseFolder, L"TalkPass_Square.png", L"TalkPass_Square", 628.f, 660.f, 25, 25, FALSE, 200);

	return S_OK;
}
HRESULT  NPCTalk::Effect_Initialize() {
	return S_OK;
}
HRESULT  NPCTalk::Text_Initialize() {
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 643, 502.f }, 28, L"Personal Name", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(0, 255, 255, 255));
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"", { 640, 560.f }, 20, L"TALK", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(0, 255, 255, 255));
	return S_OK;
}

NPCTalk* NPCTalk::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	NPCTalk* NPT = new NPCTalk(_GRPDEV);
	if (FAILED(NPT->Ready_GameObject())) {
		MSG_BOX("Cannot Create NPCTalk.");
		Safe_Release(NPT);
		return nullptr;
	}
	return NPT;
}
VOID	 NPCTalk::Free() {
	GameObject::Free();
}