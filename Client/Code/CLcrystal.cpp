#include "../Include/PCH.h"

CLcrystal::CLcrystal(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV),m_TextureIndex(0), m_FrameTick(0.f) {}
CLcrystal::CLcrystal(const GameObject& _RHS) : GameObject(_RHS), m_TextureIndex(0), m_FrameTick(0.f) {}
CLcrystal::~CLcrystal() {}

HRESULT CLcrystal::Ready_Effect(_vec3 vPos) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	_bool AngleChase = true;

	Make_TextureList(L"Spr_Effect_Cheonlog_AttackMode_Rage02_0");	
	Component_Transform->Set_Pos(vPos);
	Component_Transform->Set_Rotation(50.f, 0.f, 0.f);
	Component_Transform->Set_Scale(1.f, 1.f, 1.f);
	return S_OK;
}

HRESULT CLcrystal::Make_TextureList(wstring _FileName) {
	INT FRAME = 0;

	while (++FRAME) {
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { TEX->AddRef();  TextureList.push_back(TEX); }
	}

	return S_OK;
}

INT  CLcrystal::Update_GameObject(const _float& _DT) {
	GameObject::Update_GameObject(_DT);
	Move_Frame(_DT);
	Chek_CLPos();
	return 0;
}
void CLcrystal::LateUpdate_GameObject(const _float& _DT) {

}
void CLcrystal::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, TextureList[m_TextureIndex]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
void CLcrystal::Move_Frame(const _float& _DT)
{
	m_FrameTick += _DT;

	if (m_FrameTick > 0.1f)
	{
		++m_TextureIndex;
		m_FrameTick = 0.f;
	}
	if (m_TextureIndex > TextureList.size() - 1)
	{
		m_TextureIndex = 0;

	}
}

void CLcrystal::Chek_CLPos()
{

	_vec3 pPos = *dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
	Cheonlog* pCL = dynamic_cast<Cheonlog*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog"));

	if (pCL->Get_Statu() == CL_LJUMP)
	{
		pPos += { -1.4f, 1.3f, 2.9f };
		Component_Transform->Set_Pos(pPos.x, pPos.y, pPos.z);

	}
	else if (pCL->Get_Statu() == CL_RJUMP)
	{
		pPos += { 0.9f, 1.f, 3.1f };
		Component_Transform->Set_Pos(pPos.x, pPos.y, pPos.z);
	}
	else
	{
		pPos += { 0.9f, 1.f, 3.1f };
		Component_Transform->Set_Pos(pPos.x, pPos.y, pPos.z);
	}

}


HRESULT	CLcrystal::Component_Initialize() {
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;			
	return S_OK;
}
CLcrystal* CLcrystal::Create(LPDIRECT3DDEVICE9 _GRPDEV,  _vec3 vPos) {
	CLcrystal* EFT = new CLcrystal(_GRPDEV);
	if (FAILED(EFT->Ready_Effect( vPos))) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(EFT);
		return nullptr;
	}
	return EFT;
}
void CLcrystal::Free() {
	for (auto& iter : TextureList)
		Safe_Release(iter);

	TextureList.clear();
	GameObject::Free();
}
