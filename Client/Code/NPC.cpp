#include "../Include/PCH.h"
#include "NPC.h"

NPC::NPC(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV)	{}
NPC::NPC(const GameObject& _RHS)	: GameObject(_RHS)		{}
NPC::~NPC()													{}

HRESULT NPC::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	Timer_Tif = 0.f;
	Tif_AnimIDX = 1;

	Interaction_Possible = TRUE;

	PlayerObject	= dynamic_cast<Player*>	(SceneManager::GetInstance()->Get_GameObject(L"Player"));


	CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));

	_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
	_vec3 planeDir = { 0.f, 1.f, 0.f };

	_float angle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&cameraDir, &cameraDir), D3DXVec3Normalize(&planeDir, &planeDir)));
	_float _cameraAngle = angle / D3DX_PI * 180.f;

	Component_Transform->Rotation(ROT_X, 90.f - _cameraAngle);

	return S_OK;
}
INT	NPC::Update_GameObject(const _float& _DT) {
	GameObject::Update_GameObject(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}
VOID NPC::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);
	Timer_Tif += _DT;
	if (Timer_Tif > 0.2f) {
		Tif_AnimIDX = Tif_AnimIDX % 7 + 1;
		Timer_Tif = 0.f;
	}
		AlphaZValue = PlayerObject->Get_AlphaZValue() + 1;
}
VOID NPC::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, Tif_TextureList[Tif_AnimIDX]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT NPC::Component_Initialize() {

	Component_Buffer	= ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Texture	= ADD_COMPONENT_TEXTURE;
	Component_Collider	= ADD_COMPONENT_COLLIDER;

	ResourceManager::GetInstance()->PathFinder(GRPDEV, L"../../UI/NPCDialog");

	for (INT PIC = 1; PIC <= 8; ++PIC) {
		wstring Base = L"Tif_Stand" + to_wstring(PIC) + L".png";
		Tif_TextureList.push_back(ResourceManager::GetInstance()->Find_Texture(Base.c_str()));
	}
	
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(2.f, 2.f, 2.f);

	Component_Transform->Set_Pos({ 20.213f , 0.5f, 23.f }); 
	Component_Transform->Set_Scale(0.72f, 1.44f, 1.2f);

	return S_OK;
}
NPC* NPC::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	NPC* NPN = new NPC(_GRPDEV);
	if (FAILED(NPN->Ready_GameObject())) {
		MSG_BOX("Cannot Create NPC.");
		Safe_Release(NPN);
		return nullptr;
	}
	return NPN;
}
VOID NPC::Free() {
	GameObject::Free();
}

BOOL NPC::OnCollisionEnter(GameObject* _Other) {
	return FALSE;
}
BOOL NPC::OnCollisionStay(GameObject* _Other) {
	
	if (_Other->Get_ObjectTag() == L"Player") {
		if (Interaction_Possible) {
			PlayerUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"));
			
			PlayerUI->PopUp_Interaction_Notice(L"대화하기 - 티프", TRUE);
			UIManager::GetInstance()->Find_FontObject(L"Interaction_Text")->Set_Color(255, 255, 255, 255);
		}
		if (Interaction_Possible && KEY_DOWN(DIK_E)) {
			PlayerUI->Set_PlayTutorial(TRUE);
			Interaction_Possible = FALSE;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL NPC::OnCollisionExit(GameObject* _Other) {
	if (_Other->Get_ObjectTag() == L"Player") {
		PlayerUI->PopUp_Interaction_Notice(L"", FALSE);
		return TRUE;
	}
	return FALSE;
}
