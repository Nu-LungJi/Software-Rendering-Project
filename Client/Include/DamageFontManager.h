#pragma once

class DamageFontManager
{
	DECLARE_SINGLETON(DamageFontManager)

private:
	explicit DamageFontManager();
	~DamageFontManager();

public:
	HRESULT		Ready_DamageFontManager();
	VOID		Update_DamageFontManager(CONST FLOAT& _DT);
	VOID		LateUpdate_DamageFontManager(CONST FLOAT& _DT);
	VOID		Render_DamageFontManager();

	VOID		Add_DamageFont(GameObject* obj, int damage, bool isCritical = false);
	FontObject* Get_Fo(int size);

private:
	LPDIRECT3DDEVICE9		GRPDEV;

	vector<vector<FontObject*>>  FontList;


public:
	virtual	VOID				Free();

};

