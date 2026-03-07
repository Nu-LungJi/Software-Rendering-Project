#include "../Include/PCH.h"
#include "Player_Shadow.h"

Player_Shadow::Player_Shadow(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Player_Shadow::Player_Shadow(const GameObject& _RHS) : GameObject(_RHS) {}
Player_Shadow::~Player_Shadow() {}

HRESULT Player_Shadow::Ready_GameObject()
{
	if (FAILED(Component_Initialize())) return E_FAIL;

	lateReady = false;
	AlphaRatio = 1.f;

	return S_OK;
}

INT Player_Shadow::Update_GameObject(const _float& _DT)
{
	GameObject::Update_GameObject(_DT);

	if (!lateReady) {
		PlayerObject = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));

		PlayerPos = static_cast<Transform*>(PlayerObject->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();

		Component_Transform->Set_Scale({ 1.f, 1.f, 1.f });
		Component_Transform->Rotation(ROT_X, 80.f);
		lateReady = true;
	}
	
	Component_Transform->Set_Pos({ PlayerPos->x, PlayerPos->y - 0.3f , PlayerPos->z - 1.35f });

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return S_OK;
}

VOID Player_Shadow::LateUpdate_GameObject(const _float& _DT)
{
	GameObject::LateUpdate_GameObject(_DT);
}

VOID Player_Shadow::Render_GameObject()
{
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	SetGrahpic();

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ÃÊ±âÈ­
	GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}

HRESULT Player_Shadow::Component_Initialize()
{
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Texture = ADD_COMPONENT_TEXTURE;

	Component_Texture->Import_TextureFromFolder(L"../../Resource/Player/Stand");

	return S_OK;
}

VOID Player_Shadow::SetGrahpic()
{
	DWORD tfactor = D3DCOLOR_ARGB(
		(BYTE)(AlphaRatio * 255.f),
		255, 255, 255
	);

	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, tfactor);

	TCHAR FileName[128] = L"";
	wsprintfW(FileName, L"Player_Shadow.png");

	Component_Texture->Set_Texture(FileName);

	GRPDEV->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	GRPDEV->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GRPDEV->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GRPDEV->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

Player_Shadow* Player_Shadow::Create(LPDIRECT3DDEVICE9 _GRPDEV)
{
	Player_Shadow* player_Shadow = new Player_Shadow(_GRPDEV);
	if (FAILED(player_Shadow->Ready_GameObject())) {
		MSG_BOX("Cannot Create Player_Shadow.");
		Safe_Release(player_Shadow);
		return nullptr;
	}
	return player_Shadow;
}

VOID Player_Shadow::Free()
{
	GameObject::Free();
}
