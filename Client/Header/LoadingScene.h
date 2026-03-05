#pragma once
#include "Scene.h"
class CLoading;
class MiniGameScene;
class LoadingScene : public Scene {
protected:
	explicit LoadingScene(LPDIRECT3DDEVICE9 _GRPDEV);
	virtual ~LoadingScene();

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
public:
	static   LoadingScene* Create(LPDIRECT3DDEVICE9 _GRPDEV);
private:
	virtual void Free();
};

