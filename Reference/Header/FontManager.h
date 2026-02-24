#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)



class ENGINE_DLL FontManager : public Base {
  DECLARE_SINGLETON(FontManager)
private:
  explicit	FontManager();
  virtual	~FontManager();

public:
	HRESULT		Ready_FontManager(LPDIRECT3DDEVICE9 _GRPDEV);
	INT			Update_FontManager(CONST FLOAT& _DT);
	VOID		LateUpdate_FontManager(CONST FLOAT& _DT);
	VOID		Render_FontManager();

	HRESULT		Add_FontSprite(LPDIRECT3DDEVICE9 _GRPDEV, wstring _Text, _vec2 _Position, _int _TextScale, wstring _FontTag,
		wstring _FontType, D3DCOLOR _Color = D3DCOLOR_ARGB(255, 255, 255, 255), _int TextWeight = 100, BOOL _Visible = TRUE);

	FontObject* Find_FontObject(wstring _Text);

	VOID		Render_FontLayer() {};
private:

private:
	virtual		VOID	Free();
};

END