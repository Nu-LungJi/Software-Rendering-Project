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

	VOID		Render_FontLayer() {};
private:

private:
	virtual		VOID	Free();
};

END