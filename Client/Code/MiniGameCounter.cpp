#include "../Include/PCH.h"

MiniGameCounter::MiniGameCounter(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_bEnd(false), m_fDefense(0.f), m_fTime(0), m_fFrame(0), m_bStopFrame(false), m_pBuffer(nullptr), m_pTransform(nullptr), m_pTileInfo(nullptr) {}
MiniGameCounter::MiniGameCounter(const GameObject& _RHS) : GameObject(_RHS) {}
MiniGameCounter::~MiniGameCounter() {}

HRESULT MiniGameCounter::Ready_GameObject() {

	if (FAILED(Component_Initialize())) return E_FAIL;

	m_iCnt = 0;
	m_iKeyCnt = 0;
	m_StageCnt[0] = 5;
	m_StageCnt[1] = 10;
	m_StageCnt[2] = 20;

	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"STAGE :", { 423.14f, 10.f }, 30, L"STAGE_NAME", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255));
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"0", { 495.52f, 10.f }, 30, L"STAGE_COUNT", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255));

	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"남은 마리 수 : ", { 766.537f, 10.f }, 30, L"MONSTER_NUMBER", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255));
	UIManager::GetInstance()->Add_FontSprite(GRPDEV, L"0", { 878.10f, 10.f }, 30, L"MONSTER_NAME", L"Yoon\u00AE 대한", D3DCOLOR_ARGB(200, 255, 255, 255));
	Make_TextureList(L"../../Resource/Clear/CLEAR_");
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
		return -1;
	if (m_StageCnt[m_iCnt] <= 0)
		++m_iCnt;

	if (m_iCnt > 2)
		m_iCnt = 2;
	if (m_StageCnt[m_iCnt] <= 0)
		m_StageCnt[m_iCnt] = 0;

	UIManager::GetInstance()->Find_FontObject(L"STAGE_COUNT")->Set_Text(to_wstring(m_iCnt));
	UIManager::GetInstance()->Find_FontObject(L"MONSTER_NAME")->Set_Text(to_wstring(m_StageCnt[m_iCnt]));
	RenderManager::GetInstance()->Add_RenderGroup(RENDERID::RENDER_UI, this);

	return 0;

}

VOID MiniGameCounter::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
	
	if (m_StageCnt[2] <= 0)
	{
		m_bEnd = true;
		m_fTime += _DT;
		m_fFrame += _DT;
		if (m_fFrame > 0.1f)
			++m_iKeyCnt;
		if (m_iKeyCnt > m_vecKeyList.size() - 1)
			m_iKeyCnt = m_vecKeyList.size() - 1;

		if (m_fTime > 6.f)
		{
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"STAGE_NAME"));
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"STAGE_COUNT"));
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"MONSTER_NUMBER"));
			UIManager::GetInstance()->Delete_FontObject(UIManager::GetInstance()->Find_FontObject(L"MONSTER_NAME"));
			_vec3 vPos = { 17.862f, 0.5f, 121.045f };
			dynamic_cast<StageBlackOut*>(EffectManager::GetInstance()->Get_Scene())->Set_Pos(vPos, false, 0);
			TileManager::GetInstance()->Set_CurStage(TILE_STAGE::TILE_STAGE4);

			Set_ObjectDead(TRUE);
		}
	}
	

}
VOID MiniGameCounter::Render_GameObject()
{

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetRenderState(D3DRS_ZENABLE, FALSE);
	Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	Sprite->Draw(m_pSprite->Get_Texture(m_vecKeyList[m_iKeyCnt])->TEXTURE, NULL, NULL, &m_pSprite->Get_Texture(m_vecKeyList[m_iKeyCnt])->POS, D3DCOLOR_ARGB(255, 255, 255, 255));
	Sprite->End();
	GRPDEV->SetRenderState(D3DRS_ZENABLE, TRUE);
	

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}


void MiniGameCounter::Imgui()
{
	ImGui::SetNextWindowSize({ 800,600 });

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
void MiniGameCounter::Imgui_Setting()
{
	static _float fsScale(1);
	static _float sMin(-1000), sMax(2000), sMin1(-1000), sMax1(2000), sMin2(-1000), sMax2(2000), sMin3(-3000), sMax3(3000);
	static _float fPivot1(1);

	_float fMin(0.0f), fMax(100), UMin(0), UMax(0), fMovePosMin(-100), fMovePosMax(129), fMoveScaleMin(-100), fMoveScaleMax(129), fMoveRotMin(-360), fMoveRotMax(360), vMouseMin(0), vMouseMax(0), fHeightMin(0.f), fHeightMax(10.f), fPosMin(0.f), fPosMax(200.f);


	
		_vec2 vPos = UIManager::GetInstance()->Find_FontObject(L"STAGE_NAME")->Get_Pos();
			ImGui::Text("STAGE_NAME");
			ImGui::SameLine(200.f, 0.f);
			ImGui::SliderFloat2("##99", vPos, sMin, sMax);
			Imgui_ButtonStyle();
			if (ImGui::Button("STAGE_NAME+"))
			{
				vPos.x += fPivot1;
			}
			ImGui::SameLine(150, 0.f);
			ImGui::PopStyleColor(3);
			Imgui_ButtonStyle();
			if (ImGui::Button("STAGE_NAME-"))
			{
				vPos.x -= fPivot1;
			}
			ImGui::PopStyleColor(3);
			UIManager::GetInstance()->Find_FontObject(L"STAGE_NAME")->Set_Pos(vPos.x, vPos.y);


		_vec2 vPos1= UIManager::GetInstance()->Find_FontObject(L"STAGE_COUNT")->Get_Pos();
			ImGui::Text("STAGE_COUNT");
			ImGui::SameLine(200.f, 0.f);
			ImGui::SliderFloat2("##98", vPos1, sMin1, sMax1);
			Imgui_ButtonStyle();
			if (ImGui::Button("STAGE_COUNT+"))
			{
				vPos1.x += fPivot1;
			}

			ImGui::SameLine(150, 0.f);
			ImGui::PopStyleColor(3);
			Imgui_ButtonStyle();
			if (ImGui::Button("STAGE_COUNT-"))
			{
				vPos1.x -= fPivot1;
			}
			ImGui::PopStyleColor(3);
			UIManager::GetInstance()->Find_FontObject(L"STAGE_COUNT")->Set_Pos(vPos1.x, vPos1.y);


		_vec2 vPos2 = UIManager::GetInstance()->Find_FontObject(L"MONSTER_NUMBER")->Get_Pos();
			ImGui::Text("MONSTER_NUMBER");
			ImGui::SameLine(200.f, 0.f);
			ImGui::SliderFloat2("##97", vPos2, sMin2, sMax2);
			Imgui_ButtonStyle();
			if (ImGui::Button("MONSTER_NUMBER+"))
			{
				vPos2.x += fPivot1;
			}

			ImGui::SameLine(150, 0.f);
			ImGui::PopStyleColor(3);
			Imgui_ButtonStyle();
			if (ImGui::Button("MONSTER_NUMBER-"))
			{
				vPos2.x -= fPivot1;
			}
			ImGui::PopStyleColor(3);
			UIManager::GetInstance()->Find_FontObject(L"MONSTER_NUMBER")->Set_Pos(vPos2.x, vPos2.y);

		_vec2 vPos3 = UIManager::GetInstance()->Find_FontObject(L"MONSTER_NAME")->Get_Pos();
			ImGui::Text("MONSTER_NAME");
			ImGui::SameLine(200.f, 0.f);
			ImGui::SliderFloat2("##96", vPos3, sMin3, sMax3);
			Imgui_ButtonStyle();
			if (ImGui::Button("MONSTER_NAME+"))
			{
				vPos3.x += fPivot1;
			}
			ImGui::SameLine(150, 0.f);
			ImGui::PopStyleColor(3);
			Imgui_ButtonStyle();
			if (ImGui::Button("MONSTER_NAME-"))
			{
				vPos3.x -= fPivot1;
			}
			ImGui::PopStyleColor(3);
			UIManager::GetInstance()->Find_FontObject(L"MONSTER_NAME")->Set_Pos(vPos3.x, vPos3.y);


			//Imgui_ButtonStyle();
			//if (ImGui::Button("PivotScale+"))
			//{
			//
			//}
			//ImGui::PopStyleColor(3);
			//
			//ImGui::SameLine(50.f, 0.f);
			//Imgui_ButtonStyle();
			//if (ImGui::Button("PivotScale"))
			//{
			//
			//	m_fPivot -= 0.1f;
			//}
			//ImGui::PopStyleColor(3);

		
	
}
void MiniGameCounter::Imgui_ButtonStyle()
{
	ImGui::PushStyleColor(ImGuiCol_Button, D3DXCOLOR(0.0f, 0.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.8f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.5f, 0.7f, 0.7f));
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
VOID MiniGameCounter::Free() {

	Safe_Release(Sprite);
	GameObject::Free();
}