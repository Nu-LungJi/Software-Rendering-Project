#include "../Include/pch.h"
#include "DamageFontManager.h"

IMPLEMENT_SINGLETON(DamageFontManager)

DamageFontManager::DamageFontManager() {};
DamageFontManager::~DamageFontManager()
{
	Free();
}

HRESULT DamageFontManager::Ready_DamageFontManager()
{
	GRPDEV = GraphicDevice::GetInstance()->Get_Device();

	FontList.resize(71);

	for (int k = 0; k <= 70; k++) {
		for (int i = 0; i < 20; i++) {
			_vec3 destPos = { 0.f, 0.f, 0.f };
	
			FontObject* FO = UIManager::GetInstance()->Add_FontSprite(GRPDEV, to_wstring(0), { 0.f, 0.f }, k + 30,
				L"DamageFont", L"Bastard", D3DCOLOR_ARGB(0, 247, 230, 0));
			FO->Set_Active(false);
			FontList[k].push_back(FO);
		}
	}

	

	return E_NOTIMPL;
}

VOID DamageFontManager::Update_DamageFontManager(const FLOAT& _DT)
{
	return VOID();
}

VOID DamageFontManager::LateUpdate_DamageFontManager(const FLOAT& _DT)
{
	return VOID();
}

VOID DamageFontManager::Render_DamageFontManager()
{
	return VOID();
}

VOID DamageFontManager::Add_DamageFont(GameObject* obj, int damage)
{
	_vec3 targetPos = *(static_cast<Transform*>(obj->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position());

	DamageFont* DamageFont = nullptr;

	_vec3 destPos = { 0.f, 0.f, 0.f };
	
	DamageFont = DamageFont::Create(GRPDEV, damage, &targetPos);
	DamageFont->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_UI);
	DamageFont->Set_ObjectTag(L"DamageFont");
	DamageFont->Set_Damage(damage);

	SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(DamageFont);

	damageFontCnt += 1;

	return VOID();
}

FontObject* DamageFontManager::Get_Fo(int size)
{
	for (FontObject* FO : FontList[size - 30]) {
		if (!FO->Get_Active()) return FO;
	}

	return nullptr;
}

VOID DamageFontManager::Free()
{
	Safe_Release(GRPDEV);

	return VOID();
}
