#pragma once
#include "Scene.h"
class CLoading;
class MiniGameScene;
class TempScene : public Scene {
protected:
	explicit TempScene(LPDIRECT3DDEVICE9 _GRPDEV);
	virtual ~TempScene();

public:
	virtual   HRESULT      Ready_Scene();
	virtual INT         Update_Scene(CONST FLOAT& _DT);
	virtual VOID      LateUpdate_Scene(CONST FLOAT& _DT);
	virtual VOID      Render_Scene();

private:
	HRESULT      Ready_Enviroment_Layer();
	HRESULT      Ready_GameLogic_Layer();
	HRESULT      Ready_UserInterface_Layer();

private:
	CLoading* pLoading;
	MiniGameScene* pMiniGame;
public:
	static   TempScene* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual void Free();
};

