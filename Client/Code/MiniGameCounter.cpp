#include "../Include/PCH.h"

MiniGameCounter::MiniGameCounter(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_iWave(1),m_bEnd(false), m_fDefense(0.f), m_fTime(0), m_fFrame(0), m_bStopFrame(false), m_pBuffer(nullptr), m_pTransform(nullptr), m_pTileInfo(nullptr) {}
MiniGameCounter::MiniGameCounter(const GameObject& _RHS) : GameObject(_RHS) {}
MiniGameCounter::~MiniGameCounter() {}

HRESULT MiniGameCounter::Ready_GameObject() {

	SoundManager::GetInstance()->Stop_AllSound();
    SoundManager::GetInstance()->Play_Sound(L"Stage/MiniGame_VamPire.mp3", CHANNELID::SOUND_BGM03, 0.3f);

	if (FAILED(Component_Initialize())) return E_FAIL;
	m_iLast = 0;
	m_iCnt = 0;
	m_bEndWave = false;
	m_bAugment = false;
	m_iKeyCnt = 0;
	m_StageCnt[0] = 10;
	m_StageCnt[1] = 20;
	m_StageCnt[2] = 30;
	m_StageCnt[3] = 40;
	m_StageCnt[4] = 50;
	m_iMaxWave = 5;
	wstring Font = L"Gowun Dodum";
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"라운드 :", { 423.14f, 10.f }, 35, L"STAGE_NAME", Font.c_str(), D3DCOLOR_ARGB(200, 255, 255, 255));
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"1", { 485.52f, 10.f }, 35, L"STAGE_COUNT", Font.c_str(), D3DCOLOR_ARGB(200, 255, 255, 255));
	//Yoon\u00AE 대한
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"남은 마리 수 : ", { 766.537f, 10.f }, 35, L"MONSTER_NUMBER", Font.c_str(), D3DCOLOR_ARGB(200, 255, 255, 255));
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"0", { 878.10f, 10.f }, 35, L"MONSTER_NAME", Font.c_str(), D3DCOLOR_ARGB(200, 255, 255, 255));
	Make_TextureList(L"../../UI/MiniGame/Clear/CLEAR_");
	wstring Name = L"FontBackUI";
	m_pSprite->Import_Sprite(L"../../UI/MiniGame/Spr_Ui_ChaosFrame.png", Name.c_str(), 332.645, 2.491, 600, 50, TRUE, 255);
	m_vecKeyList.push_back(Name);
	return S_OK;
}
HRESULT MiniGameCounter::Make_TextureList(wstring _FileName)
{
	INT FRAME = 0;

	while (++FRAME) {
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";
		wstring KeyName = _FileName + to_wstring(FRAME);
		
		m_pSprite->Import_Sprite(FileName.c_str(), KeyName.c_str(), WINCX / 6, WINCY /6, 800, 150, TRUE, 255);

		m_vecKeyList.push_back(KeyName);
		if (FRAME == 20)
			return S_OK;
	}


	return S_OK;
}
INT	MiniGameCounter::Update_GameObject(const _float& _DT) {

	GameObject::Update_GameObject(_DT);
	//Imgui();
	if (Get_ObjectDead() == TRUE)
	{
		SoundManager::GetInstance()->Stop_AllSound();
		SoundManager::GetInstance()->Play_Sound(L"Stage/Bgm_Stage1-2_Loop.wav", CHANNELID::SOUND_BGM01, 0.3f);
		SoundManager::GetInstance()->Play_Sound(L"Stage/Ambience_Rain.wav", CHANNELID::SOUND_BGM02, 0.25f);

		return -1;
	}
	if (m_StageCnt[m_iCnt] <= 0)
	{
		++m_iWave;
		++m_iCnt;
	}

	if (m_iCnt >= m_iMaxWave - 1)
		m_iCnt  = m_iMaxWave - 1 ;

	if (m_StageCnt[m_iCnt] <= 0)
		m_StageCnt[m_iCnt] = 0;

	if (m_iWave >= m_iMaxWave)
	{
		m_iWave = m_iMaxWave;
	}
		
	if (!m_bAugment)
	{
		UIManager::GetInstance()->Find_FontObject(L"STAGE_COUNT")->Set_Text(to_wstring(m_iWave));
		UIManager::GetInstance()->Find_FontObject(L"MONSTER_NAME")->Set_Text(to_wstring(m_StageCnt[m_iCnt]));

	}
	RenderManager::GetInstance()->Add_RenderGroup(RENDERID::RENDER_UI, this);

	return 0;

}

VOID MiniGameCounter::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
	
	if (m_StageCnt[m_iMaxWave - 1] <= 0)
	{
		m_bEnd = true;
		m_fTime += _DT;
		m_fFrame += _DT;
		if (m_fFrame > 0.1f)
		{
			m_fFrame = 0;
			++m_iLast;
			++m_iKeyCnt;
		}

		if (m_iKeyCnt > m_vecKeyList.size() - 2)
			m_iKeyCnt = m_vecKeyList.size() - 2;

		if (m_iKeyCnt == m_vecKeyList.size() - 2 && !m_bAugment)
		{
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"STAGE_NAME"));
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"STAGE_COUNT"));
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"MONSTER_NUMBER"));
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"MONSTER_NAME"));
			Augment* pObj = Augment::Create(GRPDEV);
			pObj->Set_ObjectTag(L"Agument");
			SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pObj);
			m_bAugment = true;
		}
			

		if (m_fTime > 6.f && m_bEndWave)
		{
			
			_vec3 vPos = { 17.862f, 0.5f, 121.045f };
			dynamic_cast<StageBlackOut*>(EffectManager::GetInstance()->Get_Scene())->Set_Pos(vPos, false, 0,false);
			TileManager::GetInstance()->Set_CurStage(TILE_STAGE::TILE_STAGE4);

			Set_ObjectDead(TRUE);
		}
	}
	

}
VOID MiniGameCounter::Render_GameObject()
{

	if (!m_bAugment)
	{

		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		GRPDEV->SetRenderState(D3DRS_ZENABLE, FALSE);
		Sprite->Begin(D3DXSPRITE_ALPHABLEND);
		Sprite->Draw(m_pSprite->Get_Texture(m_vecKeyList[m_iKeyCnt])->TEXTURE, NULL, NULL, &m_pSprite->Get_Texture(m_vecKeyList[m_iKeyCnt])->POS, D3DCOLOR_ARGB(255, 255, 255, 255));
		Sprite->End();
		
	
		Sprite->Begin(D3DXSPRITE_ALPHABLEND);
		Sprite->Draw(m_pSprite->Get_Texture(m_vecKeyList.back())->TEXTURE, NULL, NULL, &m_pSprite->Get_Texture(m_vecKeyList.back())->POS, D3DCOLOR_ARGB(255, 255, 255, 128));
		Sprite->End();
		GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		GRPDEV->SetRenderState(D3DRS_ZENABLE, TRUE);

	}

}
HRESULT MiniGameCounter::Component_Initialize() {

	m_pSprite = ADD_COMPONENT_SPRITE;
	D3DXCreateSprite(GRPDEV, &Sprite);
	return S_OK;
}

MiniGameCounter* MiniGameCounter::Create(LPDIRECT3DDEVICE9 _GRPDEV) {

	MiniGameCounter* pMiniGameCounter = new MiniGameCounter(_GRPDEV);

	if (FAILED(pMiniGameCounter->Ready_GameObject())) {
		MSG_BOX("Cannot Create MiniGameCounter.");
		Safe_Release(pMiniGameCounter);
		return nullptr;
	}

	return pMiniGameCounter;
}

void MiniGameCounter::Imgui_Setting()
{
	static _float fsScale(1);
	static _float sMin(-100), sMax(2000), sMin1(-100), sMax1(2000);
	static _float fPivot1(1);

	_float fMin(0.0f), fMax(100), UMin(0), UMax(0), fMovePosMin(-100), fMovePosMax(129), fMoveScaleMin(-100), fMoveScaleMax(129), fMoveRotMin(-360), fMoveRotMax(360), vMouseMin(0), vMouseMax(0), fHeightMin(0.f), fHeightMax(10.f), fPosMin(0.f), fPosMax(200.f);


	///////////위치
	_vec2 vPos = { m_pSprite->Get_Texture(m_vecKeyList.back())->POS.x, m_pSprite->Get_Texture(m_vecKeyList.back())->POS.y };
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
	m_pSprite->Get_Texture(m_vecKeyList.back())->POS.x = vPos.x;
	m_pSprite->Get_Texture(m_vecKeyList.back())->POS.y = vPos.y;


	///////////스케일
	_vec2 vScale = { m_pSprite->Get_Texture(m_vecKeyList.back())->Get_Scale().x, m_pSprite->Get_Texture(m_vecKeyList.back())->Get_Scale().y };
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
	m_pSprite->Get_Texture(m_vecKeyList.back())->Set_Scale(vScale.x, vScale.y);
}
void MiniGameCounter::Imgui()
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
void MiniGameCounter::Imgui_ButtonStyle()
{
	ImGui::PushStyleColor(ImGuiCol_Button, D3DXCOLOR(0.0f, 0.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.8f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.5f, 0.7f, 0.7f));
}
VOID MiniGameCounter::Free() {

	Safe_Release(Sprite);
	GameObject::Free();
}