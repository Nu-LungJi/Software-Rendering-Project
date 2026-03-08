#pragma once

class GameManager : public Base {
private:
	explicit GameManager();
	virtual ~GameManager();

public:
	HRESULT		Ready_GameManager();
	VOID		Update_GameManager(CONST FLOAT& _DT);
	VOID		LateUpdate_GameManager(CONST FLOAT& _DT);
	VOID		Render_GameManager();

public:
	HRESULT		Ready_DefaultSetting();
	HRESULT		Ready_SceneSetting();

	GraphicDevice*		Get_DeviceClass()	{ return DEVCLASS;	}
	LPDIRECT3DDEVICE9	Get_GraphicDevice() { return GRPDEV;	}
private:
	GameObject*					pBlackOut;
	GraphicDevice*				DEVCLASS;
	LPDIRECT3DDEVICE9			GRPDEV;
	MiniGameScene*				pMiniGame;
	HCURSOR hCustomCursor;
	IDirect3DTexture9* CursorTex;
	IDirect3DSurface9* Surface;
public:
	static	GameManager*		Create();
	virtual	VOID				Free();
};