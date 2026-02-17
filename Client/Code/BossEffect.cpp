#include "../Include/PCH.h"
#include "BossEffect.h"

BossEffect::BossEffect(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
BossEffect::BossEffect(CONST GameObject& _RHS)		: GameObject(_RHS)		{}
BossEffect::~BossEffect()													{}

HRESULT BossEffect::Ready_Effect(BOSS_EFFECT _SKILLTYPE, _vec3* _BossPOS, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _Size) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	if (_SKILLTYPE == BOSS_EFFECT::APPEAR_EFFECT_EMBLEM)	{ Make_TextureList(L"Effect_Appear_Emblem");	}
	if (_SKILLTYPE == BOSS_EFFECT::APPEAR_EFFECT_SPOOL)		{ Make_TextureList(L"Effect_Spool");			}
	if (_SKILLTYPE == BOSS_EFFECT::APPEAR_EFFECT_ARM)		{ Make_TextureList(L"Effect_Appear_Arm");		}

	EffectSize = _Size;
	PlayTime = _PlayTime;

	Component_Transform->Set_Pos(*_BossPOS);
	Repeatable = _Repeatable;

	CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));

	_vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
	_vec3 planeDir = { 0.f, 1.f, 0.f };

	_float angle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&cameraDir, &cameraDir), D3DXVec3Normalize(&planeDir, &planeDir)));
	angle = angle / D3DX_PI * 180.f;

	Component_Transform->Rotation(ROT_X, 90.f - angle);

	Component_Transform->Set_Scale(EffectSize.x, EffectSize.y, EffectSize.z);

	return S_OK;
}
HRESULT	BossEffect::Make_TextureList(wstring _FileName) {
	INT FRAME = 0;
	while (++FRAME) {
		wstring FileName = _FileName + to_wstring(FRAME) + L".png";
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { TextureList.push_back(TEX); }
	}

	ENDFRAME = TextureList.size() + 1;

	return S_OK;
}
INT		BossEffect::Update_GameObject(const _float& _DT) {
	if (ObjectDead)	return 0;
	GameObject::Update_GameObject(_DT);

	FrameTick += _DT;
}
VOID BossEffect::LateUpdate_GameObject(CONST FLOAT& _DT) {
	if (ObjectDead)	return;
	if (FrameTick > PlayTime / ENDFRAME) {
		if (TextureIndex++ >= ENDFRAME - 2) {
			if (Repeatable) { TextureIndex = 0; }
			else {
				TextureIndex = ENDFRAME - 2;
				ObjectDead = TRUE;
			}
		}
		FrameTick = 0.f;
	}
}
VOID BossEffect::Render_GameObject() {
	if (ObjectDead)	return;
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, TextureList[TextureIndex]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
BOOL BossEffect::OnCollisionEnter(GameObject* _Other) {
	return TRUE;
}
BOOL BossEffect::OnCollisionStay(GameObject* _Other) {
	return TRUE;
}
BOOL BossEffect::OnCollisionExit(GameObject* _Other) {
	return TRUE;
}
HRESULT	BossEffect::Component_Initialize() {
	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(0.3f, 0.3f, 0.3f);

	Component_Texture = ADD_COMPONENT_TEXTURE;

	return S_OK;
}
BossEffect* BossEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, BOSS_EFFECT _SKILLTYPE, _vec3* _BossPOS, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _Size) {
	BossEffect* EFT = new BossEffect(_GRPDEV);
	if (FAILED(EFT->Ready_Effect(_SKILLTYPE, _BossPOS, _Repeatable, _PlayTime, _Size))) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(EFT);
		return nullptr;
	}
	return EFT;
}
VOID		BossEffect::Free() {
	GameObject::Free();
}