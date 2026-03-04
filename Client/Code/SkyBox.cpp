#include "../Include/PCH.h"

SkyBox::SkyBox(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_pBuffer(nullptr), m_pTransform(nullptr), m_pTexture(nullptr){}
SkyBox::SkyBox(const GameObject& _RHS) : GameObject(_RHS) {}
SkyBox::~SkyBox() {}

HRESULT SkyBox::Ready_GameObject() {

	if (FAILED(Component_Initialize())) return E_FAIL;

	return S_OK;
}
INT	SkyBox::Update_GameObject(const _float& _DT) {

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this	);
	return 0;
}
VOID SkyBox::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);
	Monster::BillBoard(m_pTransform,GRPDEV, { 1.f,0.f,0.f }, 0);
}

VOID SkyBox::Render_GameObject()
{
	GRPDEV->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	GRPDEV->SetTexture(0, m_pTexture);
	m_pBuffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

}

HRESULT SkyBox::Component_Initialize() {

	m_pBuffer		= ADD_COMPONENT_CUBE;
	m_pTransform	= ADD_COMPONENT_TRANSFORM;
	m_pTransform->Set_Scale(40.f, 40.f, 40.f);
	m_pTransform->Set_Pos(25.f, 25.f, 25.f);

	m_pTexture = ResourceManager::GetInstance()->Find_Texture(L"MiniGameBack.dds");
	if (m_pTexture == nullptr) return E_FAIL;

	Monster::Staic_Obj(GRPDEV, m_pTransform);
	return S_OK;
}

SkyBox* SkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	SkyBox* pSkyBox = DBG_NEWW SkyBox(pGraphicDev);

	if (FAILED(pSkyBox->Ready_GameObject())) {
		MSG_BOX("Cannot Create SkyBox.");
		Safe_Release(pSkyBox);
		return nullptr;
	}

	return pSkyBox;
}

void SkyBox::Free() {
	GameObject::Free();
}