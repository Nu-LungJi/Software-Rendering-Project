#pragma once
#include "Scene.h"

#define MINIGAMESCENE static_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene())

#define MINIGAMETILEX	25
#define MINIGAMETILEY	1
#define MINIGAMETILEZ	5

class Tesseract;
class MiniGameDis;

class MiniGameScene : public Scene {
protected:
	explicit MiniGameScene(LPDIRECT3DDEVICE9 _GRPDEV);
	virtual ~MiniGameScene();

public:
	virtual	HRESULT		Ready_Scene(Scene* pScene);
	virtual INT			Update_Scene(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_Scene(CONST FLOAT& _DT);
	virtual VOID		Render_Scene();

	HRESULT		Start_MiniGame();
	HRESULT		End_MiniGame();

private:
	HRESULT		Ready_Enviroment_Layer();
	HRESULT		Ready_GameLogic_Layer();
	HRESULT		Ready_UserInterface_Layer() { return S_OK; };
	VOID		Ready_MonsterHurdle(_vec3 _Src, _vec3 _Dst);

public:
	static	MiniGameScene* Create(LPDIRECT3DDEVICE9 _GRPDEV, Scene* pCurrScene = nullptr);

	_float* Get_Timer() { return &m_fTimer; }

public:
	virtual void Free();
	Tesseract* Get_Chaser() { return m_pChaser; }
	int*		Get_EventTrigger() { return &m_iEventTrigger; }
	Scene* Get_MainScene() { return m_pMainScene; }

private:
	Player* m_pPlayer = nullptr;
	_bool	m_bEffect ;
	Scene* m_pMainScene;
	_float m_fTimer = 0.f;

	vector<GameObject*> m_vecHurdles[3];
	Tesseract* m_pChaser;
	MainUI* m_pMainUI;
	MiniGameDis* m_pDis;

	int	m_iEventTrigger = 0;
};