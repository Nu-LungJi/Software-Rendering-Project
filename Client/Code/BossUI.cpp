#include "../Include/PCH.h"
#include "BossUI.h"

BossUI::BossUI(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), vScale{ 1,1,1 } {}
BossUI::BossUI(CONST GameObject& _RHS) : GameObject(_RHS) {}
BossUI::~BossUI() {}

HRESULT	BossUI::Ready_GameObject(BOSSUI_INFO eid) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	switch (eid)
	{
	case BOSSUI_INFO::DOCHEOL:
	
		break;
	case BOSSUI_INFO::CHLG:
		Set_HpInfo(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog"));
		break;
	}
	SpriteINFO* pInfo = new SpriteINFO(L"Bar", 600, 30, WINCX/4, 20, FALSE);
	SpriteInfo[0]= pInfo;

	pInfo = new SpriteINFO(L"HP", 590, 28, WINCX / 4+5, 25, FALSE);
	SpriteInfo[1] = pInfo;

	D3DXCreateTextureFromFileExW(GRPDEV, L"../../UI/Boss_UI/Spr_Ui_Boss_HPFrame.png", SpriteInfo[0]->WIDTH, SpriteInfo[0]->HEIGHT,
		1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&SpriteInfo[0]->TEXTURE);
	D3DXCreateTextureFromFileExW(GRPDEV, L"../../UI/Boss_UI/Spr_Ui_Boss_HP.png", SpriteInfo[1]->WIDTH, SpriteInfo[1]->HEIGHT,
		1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&SpriteInfo[1]->TEXTURE);


	return S_OK;
	
}
INT		BossUI::Update_GameObject(CONST FLOAT& _DT) {
	GameObject::Update_GameObject(_DT);
	_vec3 vScaleOrigin{ 1.f,1.f,1.f };
	if (Component_Collider == nullptr)
		return -1;

	CurHp = 1.f - ((float)Component_Collider->Get_Hp()/(float)MaxHp);
	vScale = {1.f - CurHp,vScale.y,vScale.z };

	if (vScale.x <= 0)
		vScale.x = 0;
	RenderManager::GetInstance()->Add_RenderGroup(RENDERID::RENDER_UI, this);
	
	return 0;
}
VOID	BossUI::LateUpdate_GameObject(CONST FLOAT& _DT) {

}
VOID	BossUI::Render_GameObject() {
	_matrix matWorld, matScale, matTrans;

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	Sprite->Draw(SpriteInfo[0]->TEXTURE, NULL, NULL, &SpriteInfo[0]->POS, D3DCOLOR_ARGB(255, 255, 255, 255));
	Sprite->End();


	matWorld = *Component_Transform->Get_World();
	D3DXMatrixTranslation(&matTrans, SpriteInfo[1]->POS.x, SpriteInfo[1]->POS.y, 0);
	D3DXMatrixScaling(&matScale, vScale.x, 0.7f, vScale.z);

	matWorld = matScale * matTrans;
	Sprite->SetTransform(&matWorld);
	Sprite->Begin(D3DXSPRITE_ALPHABLEND);

	
	Sprite->Draw(SpriteInfo[1]->TEXTURE, NULL, NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
	Sprite->End();
	D3DXMatrixIdentity(&matWorld);
	Sprite->SetTransform(&matWorld);

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}
HRESULT BossUI::Component_Initialize() {
	Component_Transform = ADD_COMPONENT_TRANSFORM;
    Componenet_Buffer	= ADD_COMPONENT_RECTTEX;
	D3DXCreateSprite(GRPDEV, &Sprite);

	return S_OK;
}
BossUI* BossUI::Create(LPDIRECT3DDEVICE9 _GRPDEV, BOSSUI_INFO eid) {
	BossUI* Bui = new BossUI(_GRPDEV );
	if (FAILED(Bui->Ready_GameObject(eid))) {
		MSG_BOX("Cannot Create BossUI.");
		Safe_Release(Bui);
		return nullptr;
	}
	return Bui;
}
VOID	BossUI::Free() {
	Safe_Release(Sprite);
	Safe_Release(SpriteInfo[0]->TEXTURE);
	Safe_Release(SpriteInfo[1]->TEXTURE);

	Safe_Delete(SpriteInfo[0]);
	Safe_Delete(SpriteInfo[1]);
	GameObject::Free();
}