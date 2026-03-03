#include "../Include/PCH.h"

Shadow::Shadow(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Shadow::Shadow(const GameObject& _RHS) : GameObject(_RHS) {}
Shadow::~Shadow() {}

HRESULT Shadow::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;

	return S_OK;
}
INT	Shadow::Update_GameObject(const _float& _DT)
{
	Component_Buffer->Update_Component(_DT);

	if (m_tInfo.pGameObj[0] == nullptr) {
		m_tInfo.pGameObj[0] = Monster::Get_Player();
		_float fScale = SCALE(m_tInfo.pGameObj[0])->x;
		Component_Transform->Set_Scale(fScale, fScale, fScale);
	}

	if (m_tInfo.pGameObj[0])
	{
		*MYPOS = *POS(m_tInfo.pGameObj[0]);
		MYPOS->z -= SCALE(m_tInfo.pGameObj[0])->y * 0.5f;
	}

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}
VOID Shadow::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);

	AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV, { 1.f,0.f,0.f }, 1);
}
VOID Shadow::Render_GameObject() {
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	// 1. 알파 블렌딩 및 테스트 설정
	GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// 2. 투명도 설정 (120: 적당히 연함)
	GRPDEV->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(120, 255, 255, 255));

	// 3. 컬러 연산: 텍스처 색상 사용
	GRPDEV->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	// 4. 알파 연산: 텍스처 알파 * Factor 알파
	// 중요: D3DTA_TEXTURE와 D3DTA_TFACTOR를 곱함
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	GRPDEV->SetTexture(0, ResourceManager::GetInstance()->Find_Texture(L"spr_Effect_Shadow_01.png"));
	Component_Buffer->Render_Buffer();

	// 5. 복구 (SelectArg1이 기본값)
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}
HRESULT Shadow::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(10.f, 0.5f, 0.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}
Shadow* Shadow::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Shadow* MST = new Shadow(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create Shadow.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}

VOID Shadow::Free() {

	GameObject::Free();
}
