#pragma once
#include "Base.h"
#include "Scene.h"
#include "RenderManager.h"
#include "CollisionManager.h"
#include "EffectManager.h"

BEGIN(Engine)

class ENGINE_DLL SceneManager : public Base {
	DECLARE_SINGLETON(SceneManager)
private:
	explicit SceneManager();
	virtual ~SceneManager();

public:
	HRESULT			Ready_SceneManager();
	INT				Update_SceneManager(CONST FLOAT& _DT);
	VOID			LateUpdate_SceneManager(CONST FLOAT& _DT);
	VOID			Render_SceneManager(LPDIRECT3DDEVICE9 _GRPDEV);

public:
	HRESULT			Scene_Transition(Scene* _SCENE);
	Scene*			Get_CurrentScene() { 
		return  CurrentScene;
	}
	GameObject*		Get_GameObject(CONST TCHAR* _TAG);
	void			Set_CurrentScene(Scene* scene) { CurrentScene = scene; }

	void			Set_MiniGameScene(Scene* scene) { LoadScene = scene; }
	Scene*			Get_MiniGameScene() { return LoadScene; }
	// ½ºÅ³¿ë
	void Set_TimeSlow(bool isTimeSlow) { CurrentScene->Set_TimeSlow(isTimeSlow); }
private:
	Scene*			CurrentScene;
	Scene* LoadScene;
public:
	virtual			VOID	Free();
};

END