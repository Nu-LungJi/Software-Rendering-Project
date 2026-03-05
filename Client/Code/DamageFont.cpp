#include "../Include/PCH.h"
#include "DamageFont.h"

DamageFont::DamageFont(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV){}
DamageFont::DamageFont(const GameObject& _RHS) : GameObject(_RHS) {}
DamageFont::~DamageFont() {}

HRESULT DamageFont::Ready_GameObject(int Damage, _vec3* vec3Pos)
{
	std::random_device rd;
	std::uniform_int_distribution<int> distribution(0, 1000);
	int rand = distribution(rd) % 100;

	offsetX = rand - 30.f;

	if (damageFontCnt % 2 == 0) offsetY = rand * 0.5 + 30.f;
	else offsetY = rand * 0.5 - 30.f;

	
	targetPos = *vec3Pos;
	_Pos = Matrix_Calc(targetPos);
	_Pos.x += 20.f + offsetX;
	_Pos.y -= 30.f + offsetY;
	_Size = 110.f;
	
	_lifeTimer = 0.f;
	return S_OK;
}

INT DamageFont::Update_GameObject(const FLOAT& _DT)
{
	if (_FO != nullptr) {
		_FO->Set_Active(false);
		_FO->Set_Color(0, 247, 230, 0);
		_FO = nullptr;
	}

	_lifeTimer += _DT;
	if (_lifeTimer < 0.25f) {
		if (!_isCritical) {
			_Size -= 600.f * _DT;
			_Size = max(_Size, 40.f);
		}
		else {
			_Size -= 600.f * _DT;
			_Size = max(_Size, 50.f);
		}
	}
	
	if (_lifeTimer < 0.25f) {
		_alpha += 20;
		_alpha = min(_alpha, 255);
	}
	else if (_lifeTimer > 0.25f && _lifeTimer < 0.3f) {
		_alpha = 255;
	}
	else if (_lifeTimer > 0.3f) {
		_alpha -= 20;
		_alpha = max(_alpha, 0);
	}

	_FO = DamageFontManager::GetInstance()->Get_Fo(_Size);

	if (_FO == nullptr) return -1;

	_Pos = Matrix_Calc(targetPos);
	_Pos.x += 40.f + offsetX;
	_Pos.y -= 50.f + offsetY;
	_FO->Set_Pos(_Pos.x, _Pos.y);
	_FO->Set_Text(to_wstring(_Damage));
	_FO->Set_Active(true);

	if (!_isCritical) _FO->Set_Color(_alpha, 255, 255, 255);
	else _FO->Set_Color(_alpha, 247, 230, 0);

	if (_lifeTimer > 0.75f) {
		_FO->Set_Active(false);
		return -1;
	}



	return S_OK;
}

VOID DamageFont::LateUpdate_GameObject(const FLOAT& _DT)
{
	return VOID();
}

VOID DamageFont::Render_GameObject()
{
	return VOID();
}

DamageFont* DamageFont::Create(LPDIRECT3DDEVICE9 _GRPDEV, int Damage, _vec3* vec3Pos)
{
	DamageFont* damage_Font = new DamageFont(_GRPDEV);

	if (FAILED(damage_Font->Ready_GameObject(Damage, vec3Pos))) {
		MSG_BOX("Cannot Create MiniGameCounter.");
		Safe_Release(damage_Font);
		return nullptr;
	}

	return damage_Font;
}

void DamageFont::Free()
{
	GameObject::Free();
}

_vec2 DamageFont::Matrix_Calc(_vec3 vec3Pos)
{
	D3DXVECTOR3 vWorldPos = vec3Pos;
	D3DXVECTOR3 vScreenPos;              
	D3DXMATRIX mWorld, mView, mProj;
	D3DVIEWPORT9 vp;

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);

	GRPDEV->GetTransform(D3DTS_WORLD, &mWorld);
	GRPDEV->GetTransform(D3DTS_VIEW, &mView);
	GRPDEV->GetTransform(D3DTS_PROJECTION, &mProj);
	GRPDEV->GetViewport(&vp);

	D3DXVec3Project(&vScreenPos, &vWorldPos, &vp, &mProj, &mView, &identity);

	_vec2 ret = { vScreenPos.x, vScreenPos.y };

	return ret;
}

