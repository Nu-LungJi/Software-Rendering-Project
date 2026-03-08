#include "../Include/PCH.h"
#include "MiniGameDis.h"

MiniGameDis::MiniGameDis(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
MiniGameDis::MiniGameDis(const GameObject& _RHS) : GameObject(_RHS) {}
MiniGameDis::~MiniGameDis() {}

HRESULT MiniGameDis::Ready_GameObject()
{
	m_fElapsed = 0.f;
	m_fTimer = 0.f;
	m_fDuration = 1.f;
	m_bSound[0] = 0;
	m_bSound[1] = 0;

	D3DXCreateSprite(GRPDEV, &m_pDrawSprite);

	AddFontResourceEx(L"../../Font/Griun_PolSensibility-Rg.ttf", FR_PRIVATE, 0);

	if (FAILED(D3DXCreateFont(GRPDEV, 55, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"Griun_PolSensibility-Rg", &m_pFont)))
	{
		MSG_BOX("Font Create Failed!");
		return E_FAIL;
	}

	m_vStartPos = { WINCX * 0.5f, WINCY * 0.5 };
	m_vEndPos = { WINCX - 250.f, WINCY * 0.25f };

	return S_OK;
}

INT MiniGameDis::Update_GameObject(const FLOAT& _DT)
{
	wstring DisStr = L"";

	m_fTimer += _DT;
	if (m_fTimer > 2.f) {
		if (!m_bSound[0]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"MiniGameDis.wav", CHANNELID::SOUND_EFFECT08, 1.f);
			m_bSound[0] = 1;
		}
		DisStr += L"남은 거리 :";
	}
	if (m_fTimer < 3.f) {}
	else if (m_fTimer < 5.f) {
		if (!m_bSound[1]) {
			SoundManager::GetInstance()->Play_Sound_Once(L"MiniGameDis.wav", CHANNELID::SOUND_EFFECT08, 1.f);
			m_bSound[1] = 1;
		}
		DisStr += L"200 m";

		if (m_fTimer > 4.f) {
			if (m_fElapsed < m_fDuration) {
				m_fElapsed += _DT;
			}
		}
	}
	else {
		_vec3 vPlayerPos = *POS(Monster::Get_Player());
		_int LeftDis = 200 - (int)((vPlayerPos.x + vPlayerPos.y) * 2.f);
		if (LeftDis < 0) LeftDis = 0;
		DisStr += to_wstring(LeftDis) + L" m";
	}

	m_wstrText = DisStr;

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return S_OK;
}

VOID MiniGameDis::LateUpdate_GameObject(const FLOAT& _DT)
{
}

VOID MiniGameDis::Render_GameObject()
{
	_float t = min(1.f, m_fElapsed / m_fDuration);

	_float fCurrScale = 1.5f - 1.f * t;

	_vec2 vCurrPos;
	D3DXVec2Lerp(&vCurrPos, &m_vStartPos, &m_vEndPos, t);

	_matrix matWorld;
	_vec2 vScale = { fCurrScale, fCurrScale };

	D3DXMatrixTransformation2D(&matWorld, NULL, 0.f, &vScale, NULL, 0.f, &vCurrPos);
	m_pDrawSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	m_pDrawSprite->SetTransform(&matWorld);

	RECT rcShadow = { 2, 2, 0, 0 };
	m_pFont->DrawTextW(m_pDrawSprite, m_wstrText.c_str(), -1, &rcShadow,
		DT_NOCLIP | DT_LEFT | DT_VCENTER, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));

	RECT rc = { 0, 0, 0, 0 };
	m_pFont->DrawTextW(m_pDrawSprite, m_wstrText.c_str(), -1, &rc, DT_NOCLIP | DT_LEFT | DT_VCENTER, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	m_pDrawSprite->End();
}

MiniGameDis* MiniGameDis::Create(LPDIRECT3DDEVICE9 _GRPDEV)
{
	MiniGameDis* damage_Font = new MiniGameDis(_GRPDEV);

	if (FAILED(damage_Font->Ready_GameObject())) {
		MSG_BOX("Cannot Create MiniGameCounter.");
		Safe_Release(damage_Font);
		return nullptr;
	}

	return damage_Font;
}

void MiniGameDis::Free()
{
	Safe_Release(m_pDrawSprite);
	Safe_Release(m_pFont); 
	RemoveFontResourceEx(L"../../Font/Griun_PolSensibility-Rg.ttf", FR_PRIVATE, 0);
	GameObject::Free();
}
