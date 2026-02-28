#pragma once
#include "Scene.h"

#define MINIGAMESCENE static_cast<MiniGameScene*>(SceneManager::GetInstance()->Get_CurrentScene())

#define MINIGAMETILEX	16
#define MINIGAMETILEY	1
#define MINIGAMETILEZ	10

class MiniGameScene : public Scene {
protected:
	explicit MiniGameScene(LPDIRECT3DDEVICE9 _GRPDEV);
	virtual ~MiniGameScene();

public:
	virtual	HRESULT		Ready_Scene();
	virtual INT			Update_Scene(CONST FLOAT& _DT);
	virtual VOID		LateUpdate_Scene(CONST FLOAT& _DT);
	virtual VOID		Render_Scene();

private:
	HRESULT		Ready_Enviroment_Layer();
	HRESULT		Ready_GameLogic_Layer();
	HRESULT		Ready_UserInterface_Layer() { return S_OK; };

public:
	static	MiniGameScene* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
	virtual void Free();

public:
	const	Player*			Get_Player() { return m_pPlayer; }
	const	CameraObject*	Get_Camera() { return m_pCamera; }

private:
	Player*			m_pPlayer;
	CameraObject*	m_pCamera;


//public:
//Texture*						Set_Texture(const TCHAR* _Filename, bool &_IsNew) {
//	auto iter = m_mapTexture.find(_Filename);
//	if (iter == m_mapTexture.end())
//	{
//		Texture* pTex = dynamic_cast<Texture*>(ProtoManager::GetInstance()->Clone_Prototype(COMPONENT_TYPE::COMPONENT_TEXTURE));
//		m_mapTexture.emplace(_Filename, pTex);
//		_IsNew = true;
//		return pTex;
//	}
//	else
//	{
//		_IsNew = false;
//		return (*iter).second;
//	}
//}
//Texture*						Get_Texture(const TCHAR* _Filename) {
//	return (*m_mapTexture.find(_Filename)).second;
//}
//map<const TCHAR*, Texture*>*	Get_TextureMap() { return &m_mapTexture; }
	//
//private	:
	//map<const TCHAR*, Texture*>		m_mapTexture;
};