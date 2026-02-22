#include "../Include/PCH.h"
#include "BossEffect.h"

BossEffect::BossEffect(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
BossEffect::BossEffect(CONST GameObject& _RHS)		: GameObject(_RHS)		{}
BossEffect::~BossEffect()													{}

HRESULT BossEffect::Ready_Effect(BOSS_EFFECT _SKILLTYPE, _vec3* _BossPOS, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _Size, wstring _TAG, _vec3 _Rot) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	if (_SKILLTYPE == BOSS_EFFECT::EMBLEM_APPEAR_EFFECT)			{ Make_TextureList(L"Effect_Emblem_Appear");	}
	else if (_SKILLTYPE == BOSS_EFFECT::EMBLEM_DESTROY_EFFECT)		{ Make_TextureList(L"Effect_Emblem");			}
	else if (_SKILLTYPE == BOSS_EFFECT::SPOOL_APPEAR_EFFECT)		{ Make_TextureList(L"Effect_Spool_Appear");		}
	else if (_SKILLTYPE == BOSS_EFFECT::SPOOL_FLOW_EFFECT)			{ Make_TextureList(L"Effect_Spool");			}
	else if (_SKILLTYPE == BOSS_EFFECT::WATER_POPUP_EFFECT)			{ Make_TextureList(L"Effect_Water_PopUp");		}
	else if (_SKILLTYPE == BOSS_EFFECT::FLAME_EFFECT)				{ Make_TextureList(L"Effect_AppearFlame");		}
	else if (_SKILLTYPE == BOSS_EFFECT::CIRCLE_FLAME_EFFECT)		{ Make_TextureList(L"Effect_CircleFlame");		}
	else if (_SKILLTYPE == BOSS_EFFECT::SPIRAL_FLAME_EFFECT)		{ Make_TextureList(L"Effect_Spiral_Flame");		}

	if (_SKILLTYPE == BOSS_EFFECT::SLAM_CIRCLE_EFFET)				{ Make_TextureList(L"Effect_Slam_Circle");		}
	if (_SKILLTYPE == BOSS_EFFECT::SLAM_FLAME_EFFECT)				{ Make_TextureList(L"Effect_SlamFlame");		}
	if (_SKILLTYPE == BOSS_EFFECT::SLAM_GROUND_EXP_EFFECT)			{ Make_TextureList(L"Effect_GroundExp");		}
	if (_SKILLTYPE == BOSS_EFFECT::METEOR_SLAM_FLAME_EFFECT)		{ Make_TextureList(L"Effect_SlamFlame");		}
	if (_SKILLTYPE == BOSS_EFFECT::METEOR_EFFECT)					{ Make_TextureList(L"Effect_Meteor");			}
	if (_SKILLTYPE == BOSS_EFFECT::METEOR_EXP_EFFECT)				{ Make_TextureList(L"Effect_MeteorExp");		}

	if (_SKILLTYPE == BOSS_EFFECT::FSWING_CHARGE_EFFECT)			{ Make_TextureList(L"Effect_FSwing_Charge");	}
	if (_SKILLTYPE == BOSS_EFFECT::FSWING_CHARGE_ELECTRIC_EFFECT)	{ Make_TextureList(L"Effect_FSwing_Charge_Electric"); }
	if (_SKILLTYPE == BOSS_EFFECT::FSWING_CIRCLE_EFFET)				{ Make_TextureList(L"Effect_Slam_Circle");		}
	if (_SKILLTYPE == BOSS_EFFECT::FSWING_ELECTRIC_EFFECT)			{ Make_TextureList(L"Effect_FSwing_Electric");	}
	if (_SKILLTYPE == BOSS_EFFECT::FSWING_EXP_EFFECT)				{ Make_TextureList(L"Effect_Exp");				}

	if (_SKILLTYPE == BOSS_EFFECT::RSWING_EFFECT)					{ Make_TextureList(L"Effect_RSwing");			}
	if (_SKILLTYPE == BOSS_EFFECT::RSWING_PROJ_EFFECT)				{ Make_TextureList(L"Effect_FireBall");			}

	if (_SKILLTYPE == BOSS_EFFECT::DANGER_AREA_EFFECT)				{ Make_TextureList(L"DangerArea");				}
	EffectSize = _Size;
	PlayTime = _PlayTime;

	Component_Transform->Set_Pos(*_BossPOS);
	Repeatable = _Repeatable;

	ObjectTAG = _TAG;

	Transform* BossTransform = dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Docheol")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

	Component_Transform->Rotation(ROT_X, 80.f);
	Component_Transform->Rotation(ROT_X, _Rot.x);
	Component_Transform->Rotation(ROT_Y, _Rot.y);
	Component_Transform->Rotation(ROT_Z, _Rot.z);

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
VOID BossEffect::Set_EffectRotation(FLOAT _X, FLOAT _Y, FLOAT _Z) {
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);

	Component_Transform->Rotation(ROT_X, _X);
	Component_Transform->Rotation(ROT_Z, _Z);
	Component_Transform->Rotation(ROT_Y, _Y);

	Transform* BossTransform = dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Docheol")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

	Component_Transform->Rotation(ROT_X, BossTransform->Get_Rotation()->x);
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
BossEffect* BossEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, wstring _TAG, BOSS_EFFECT _SKILLTYPE, _vec3* _BossPOS, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _Size, _vec3 _Rot) {
	BossEffect* EFT = new BossEffect(_GRPDEV);
	if (FAILED(EFT->Ready_Effect(_SKILLTYPE, _BossPOS, _Repeatable, _PlayTime, _Size, _TAG, _Rot))) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(EFT);
		return nullptr;
	}
	return EFT;
}
VOID		BossEffect::Free() {
	GameObject::Free();
}