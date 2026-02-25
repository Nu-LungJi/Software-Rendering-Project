#include "../Include/PCH.h"
#include "StageBlackOut.h"

StageBlackOut::StageBlackOut(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV){}
StageBlackOut::StageBlackOut(CONST GameObject& _RHS) : GameObject(_RHS){}
StageBlackOut::~StageBlackOut() {}

HRESULT StageBlackOut::Ready_Effect(_vec3* vPos) {
	if (FAILED(Component_Initialize())) return E_FAIL;
	D3DXCreateSprite(GRPDEV, &m_pDrawSprite);

	Make_TextureList(L"../../Tile/Frame/frame_black_L_to_R", WINCX, WINCY, 255, 46);

	m_fFrame = 0;
	m_iFrameCnt = 0;
	m_bStop = false;
	m_bRestart = true;
	return S_OK;
}
HRESULT StageBlackOut::Make_TextureList(wstring _FileName, UINT _WIDTH, UINT _HEIGHT, INT _OPACITY, INT iCnt)
{
	INT FRAME = 0;

	while (++FRAME) {
		if (FRAME == iCnt)
			return E_FAIL;
		LPDIRECT3DTEXTURE9 TEX;
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";

		D3DXCreateTextureFromFileExW(GRPDEV, FileName.c_str(), _WIDTH, _HEIGHT,
			1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &TEX);

		if (TEX == nullptr) break;
		 m_pSprtieList.push_back(TEX);
	}

	return S_OK;
}


INT  StageBlackOut::Update_GameObject(CONST FLOAT& _DT) {
	
	if (!m_bRestart)
	{

		GameObject::Update_GameObject(_DT);

		m_fFrame += _DT;
		if (m_bStop && TileManager::GetInstance()->Get_Loading())
		{
			++m_iFrameCnt;

			dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(m_vPos);
			m_bStop = false;
		}

		if (m_iFrameCnt == 20)
			TileManager::GetInstance()->Set_EndLoading(false);
		if (m_iFrameCnt == 20 && !TileManager::GetInstance()->Get_Loading())
		{
			m_bStop = true;
			return 1;
		}

		if (m_fFrame > 0.008)
		{
			m_fFrame = 0;
			++m_iFrameCnt;
		}
		if (m_iFrameCnt > m_pSprtieList.size() - 1)
		{
			m_bRestart = true;
			m_bStop = false;
			m_iFrameCnt = 0;
		}
	}
	return 0;
}
VOID StageBlackOut::LateUpdate_GameObject(CONST FLOAT& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
}
VOID StageBlackOut::Render_GameObject() {
	
	if (!m_bRestart)
	{
		GRPDEV->SetRenderState(D3DRS_ZENABLE, FALSE);
		m_pDrawSprite->Begin(D3DXSPRITE_ALPHABLEND);

		m_pDrawSprite->Draw(m_pSprtieList[m_iFrameCnt], NULL, NULL, 0, D3DCOLOR_ARGB(255, 255, 255, 255));

		m_pDrawSprite->End();
		GRPDEV->SetRenderState(D3DRS_ZENABLE, TRUE);
	}
	
}

HRESULT	StageBlackOut::Component_Initialize() {

	return S_OK;
}
StageBlackOut* StageBlackOut::Create(LPDIRECT3DDEVICE9 _GRPDEV,_vec3* vPos) {
	StageBlackOut* Blackout = new StageBlackOut(_GRPDEV);
	if (FAILED(Blackout->Ready_Effect(vPos))) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(Blackout);
		return nullptr;
	}
	return Blackout;
}
VOID StageBlackOut::Free() {
	Safe_Release(m_pDrawSprite);



	GameObject::Free();
}
