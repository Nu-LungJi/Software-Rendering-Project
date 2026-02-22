#include "../Include/PCH.h"
#include "MonsterEffect.h"

MonsterEffect::MonsterEffect(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV),	TextureIndex(0), FrameTick(0.f)		{}
MonsterEffect::MonsterEffect(CONST GameObject& _RHS)	: GameObject(_RHS),		TextureIndex(0), FrameTick(0.f)		{}
MonsterEffect::~MonsterEffect()																						{}

HRESULT MonsterEffect::Ready_Effect(MONSTER_EFFECT _SKILLTYPE, _vec3 _vPos, _float _fScalemult, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _vDir) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	Notify = 0;

	*Component_Transform->Get_Position() = _vPos;
	*Component_Transform->Get_Scale() *= _fScalemult;

	m_eEffect = _SKILLTYPE;
	Repeatable = _Repeatable;
	PlayTime = _PlayTime;
	m_vDir = _vDir;

	switch (_SKILLTYPE)
	{
	case MONSTER_EFFECT::MONSTER_SUMMONS01:			Make_TextureList(L"Spr_Effect_MonsterSummons01");		break;
	case MONSTER_EFFECT::MONSTER_SUMMONS02:			Make_TextureList(L"Spr_Effect_MonsterSummons02");		break;
	case MONSTER_EFFECT::MONSTER_SUMMONS03:			Make_TextureList(L"Spr_Effect_MonsterSummons03");		break;
	case MONSTER_EFFECT::MONSTER_DEATH:				Make_TextureList(L"Spr_Effect_baseDeathEffect_B");		break;

	case MONSTER_EFFECT::BULLET_STANDARD_BIRTH:		Make_TextureList(L"Spr_Bullet_Standard_Birth");			*MYSCALE *= 2.f;	break;
	case MONSTER_EFFECT::BULLET_STANDARD_BIRTHRAY:	Make_TextureList(L"Spr_Bullet_Standard_BirthRayUp");	*MYSCALE *= 2.f;	break;
	case MONSTER_EFFECT::BULLET_STANDARD_CHARGE:	Make_TextureList(L"Spr_Bullet_Standard_Charge");		*MYSCALE *= 2.f;	break;
	case MONSTER_EFFECT::BULLET_STANDARD_DEATH:		Make_TextureList(L"Spr_Bullet_Standard_Death");			*MYSCALE *= 2.f;	break;

	case MONSTER_EFFECT::SKILL_END:				default:		break;
	}

	return S_OK;
}

HRESULT MonsterEffect::Make_TextureList(CONST TCHAR* _Filename)
{
	ENDFRAME = 0;
	INT _frame = 0;
	IDirect3DTexture9* pTexture = nullptr;

	TCHAR Filename[256] = L"";

	while (++_frame)
	{
		++ENDFRAME;
		swprintf_s(Filename, 256, L"%s_%02d.png", _Filename, ENDFRAME);
		pTexture = ResourceManager::GetInstance()->Find_Texture(Filename);
		if (nullptr == pTexture) break;
		TextureList.push_back(pTexture);
	}

	ENDFRAME = _frame - 1;

	if (ENDFRAME == 0)
		return E_FAIL;

	return S_OK;
}

//HRESULT MonsterEffect::Make_TextureList(wstring _FileName) {
//	INT FRAME = 0;
//	while (++FRAME) {
//		wstring FileName = _FileName + to_wstring(FRAME) + L".png";
//		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
//		if (TEX == nullptr) break;
//		else { TextureList.push_back(TEX); }
//	}
//	ENDFRAME = TextureList.size() + 1;
//
//	return S_OK;
//}

INT  MonsterEffect::Update_GameObject(CONST FLOAT& _DT) {

	if (ObjectDead)
		return -1;
	GameObject::Update_GameObject(_DT);

	FrameTick += _DT;

	return 0;
}
VOID MonsterEffect::LateUpdate_GameObject(CONST FLOAT& _DT) {
	if (ObjectDead)	return;
	if (TextureList.size() == 1)
	{
		ObjectDead = FRAMETICK >= PlayTime;
	}
	else if (FrameTick > PlayTime / ENDFRAME) {
		if (TextureIndex++ >= ENDFRAME - 2) {
			if (Repeatable) { TextureIndex = 0; }
			else {
				TextureIndex = ENDFRAME - 2;
				ObjectDead = TRUE;
			}
		}
		FrameTick = 0.f;
	}

	if (TextureIndex > ENDFRAME / 2)
		Notify = true;

	switch (m_eEffect)
	{
	default:
	case MONSTER_EFFECT::MONSTER_SUMMONS02:
	case MONSTER_EFFECT::MONSTER_SUMMONS03:
	case MONSTER_EFFECT::MONSTER_SUMMONS01:
		AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV, m_vDir);
		break;
	}
}
VOID MonsterEffect::Render_GameObject() {
	if (ObjectDead)	return;
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	GRPDEV->SetTexture(0, TextureList[TextureIndex]);

	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

BOOL MonsterEffect::OnCollisionEnter(GameObject* _Other) {
	return TRUE;
}
BOOL MonsterEffect::OnCollisionStay(GameObject* _Other) {
	return TRUE;
}
BOOL MonsterEffect::OnCollisionExit(GameObject* _Other) {
	return TRUE;
}
HRESULT			MonsterEffect::Component_Initialize() {
	Component_Buffer	= ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;
	Component_Transform->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}
MonsterEffect* MonsterEffect::Create(LPDIRECT3DDEVICE9 _GRPDEV, MONSTER_EFFECT _SKILLTYPE, _vec3 _vPos, _float _fScalemult, FLOAT _PlayTime, BOOL _Repeatable, _vec3 _vDir) {
	MonsterEffect* EFT = new MonsterEffect(_GRPDEV);
	if (FAILED(EFT->Ready_Effect(_SKILLTYPE, _vPos, _fScalemult, _Repeatable, _PlayTime, _vDir))) {
		MSG_BOX("Cannot Create Effect.");
		Safe_Release(EFT);
		return nullptr;
	}
	return EFT;
}
HRESULT MonsterEffect::Monster_SummonEffect_Set(LPDIRECT3DDEVICE9 GRPDEV, Transform* TransCom, BOOL* bTrigger, _float* _fTimer)
{
	if (bTrigger == nullptr || _fTimer == nullptr)
		return E_POINTER;

	if (*bTrigger == 1) {
		if (*_fTimer >= MONSTER_SUMMON03_PLAYTIME)
			*bTrigger = 2;
		return S_OK;
	}

	HRESULT Result = E_FAIL;

	_vec3 vPos = *TransCom->Get_Position();
	_vec3 vScale = *TransCom->Get_Scale();
	_float fScale = (vScale.x > vScale.y) ? vScale.x : vScale.y;
	MonsterEffect* pEffect = nullptr;

	if (*bTrigger == 0) {
		vPos.y = fScale * 0.5f + 0.001f;

		pEffect = MonsterEffect::Create(GRPDEV,	MONSTER_EFFECT::MONSTER_SUMMONS03,
			vPos, fScale, MONSTER_SUMMON03_PLAYTIME, false);
		Result = EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);
	}
	else if (*bTrigger == 2) {
		vPos.y = fScale * 0.5f + 0.001f;

		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV,
			MONSTER_EFFECT::MONSTER_SUMMONS01,
			vPos, fScale, MONSTER_SUMMON01_PLAYTIME, false);

		Result = EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);
	}
	else if (*bTrigger == 3) {
		vPos.y = fScale * 0.5f - 0.001f;

		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV,
			MONSTER_EFFECT::MONSTER_SUMMONS02,
			vPos, fScale, MONSTER_SUMMON02_PLAYTIME, false);

		Result = EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);
	}

	if (SUCCEEDED(Result))
		*bTrigger += 1;

	return Result;
}
VOID MonsterEffect::Free() {
	GameObject::Free();
}