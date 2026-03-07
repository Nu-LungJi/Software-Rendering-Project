#include "../Include/PCH.h"
#include "Tesseract.h"

MiniGameScene::MiniGameScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV), m_bEffect(false), m_pMainScene(nullptr), m_fTimer(0.f) {}
MiniGameScene::~MiniGameScene() {}

HRESULT	MiniGameScene::Ready_Scene(Scene* pScene) {

	Monster::Set_Gravity({ 0.f, -1.f, 0.f });
	Scene::Ready_Scene();
	m_pMainScene = pScene;

	if (m_pMainScene == nullptr)
	{
		ProtoManager::GetInstance()->Ready_Prototype(GRPDEV);
		UIManager::GetInstance()->Ready_UIManager(GRPDEV);
		ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Resource/Effect");
		ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Resource/CubeFloorTile");
		MonsterManager::GetInstance()->Load_Textures_from_Folder(GRPDEV, L"../../MonsterManager");
		KeyManager::GetInstance()->Ready_KeyManager(hInst, hWnd);
	}

	if (FAILED(Ready_Enviroment_Layer()))			return E_FAIL;
	if (FAILED(Ready_GameLogic_Layer()))			return E_FAIL;
	if (FAILED(Ready_UserInterface_Layer()))		return E_FAIL;

	m_fTimer = -5.f;

	return S_OK;
}

INT	 MiniGameScene::Update_Scene(CONST FLOAT& _DT) {

	if (m_pMainUI == nullptr)	m_pMainUI = static_cast<MainUI*>(LayerList[(long)LAYER_TYPE::LAYER_USER_INTERFACE]->Get_GameObject(L"MainUI"));
	if (m_pMainUI != nullptr) {
		m_pMainUI->Set_EnableDisplayHPBar(TRUE);
		m_pMainUI->Set_BossMaxHP(100.f);
	}

	m_fTimer += _DT;

	if (KEY_DOWN(DIK_P)) {

		End_MiniGame();
	}

	CollisionManager::GetInstance()->Update_CollisionManager();
	return Scene::Update_Scene(_DT);
}
VOID MiniGameScene::LateUpdate_Scene(CONST FLOAT& _DT) {
	Scene::LateUpdate_Scene(_DT);
	//TileManager::GetInstance()->LateUpdate_Tile(_DT);
	CollisionManager::GetInstance()->LateUpdate_CollisionManager();
	CollisionManager::GetInstance()->Render_CollisionManager();

	if ((POS(m_pPlayer)->x > 49.f) && (m_iEventTrigger == 0)) {
		m_pPlayer->Set_IsFalling(true);
		m_iEventTrigger = 1;
		Monster::Set_Gravity({ 0.f,0.f,1.f });
		for (auto it : m_vecHurdles[0]) {
			it->Set_ObjectDead(true);
			it = nullptr;
			m_fTimer = 0.f;
		}
	}
	else if (POS(m_pPlayer)->y > 10.f && m_iEventTrigger == 1) {
		m_iEventTrigger = 2;
	}
	else if ((POS(m_pPlayer)->y > 49.f) && (m_iEventTrigger == 3)) {
		m_iEventTrigger = 4;
		m_pPlayer->Set_IsFalling(true);
		Monster::Set_Gravity({ 0.f,1.f,0.f });
		for (auto it : m_vecHurdles[1]) {
			it->Set_ObjectDead(true);
			it = nullptr;
		}
	}
	else if ((POS(m_pPlayer)->z > 15.f) && (m_iEventTrigger == 4)) {
		m_iEventTrigger = 5;
	}

	m_pPlayer->Fall(_DT);

	if (m_iEventTrigger == -1)
		End_MiniGame();
	else if (POS(m_pPlayer)->z > 50.f)
		End_MiniGame();
	else if (Monster::Get_Gravity().y == -1.f) {
	if (POS(m_pPlayer)->y < -3.f)
		End_MiniGame();
}
	else if (Monster::Get_Gravity().z == 1.f) {
		if (POS(m_pPlayer)->z > 13.f)
			End_MiniGame();
	}
	else if (Monster::Get_Gravity().y == 1.f) {
		if (POS(m_pPlayer)->y > 63.f)
			End_MiniGame();
	}

}
VOID MiniGameScene::Render_Scene() {}

HRESULT MiniGameScene::Start_MiniGame()
{

	if (m_pMainScene == nullptr) {
		m_pMainScene = SceneManager::GetInstance()->Get_CurrentScene();
	}

	if (m_pMainScene != nullptr) {
		CameraObject* pCamera = static_cast<CameraObject*>(m_pMainScene->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Get_GameObject(L"Camera"));
		Monster::Set_Camera(pCamera);
		pCamera->Start_MiniGame();
		pCamera->Set_EventTrigger(&m_iEventTrigger);
		LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Add_GameObject(pCamera);
		pCamera->AddRef();

		m_pPlayer = static_cast<Player*>(m_pMainScene->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Get_GameObject(L"Player"));
		Monster::Set_Player(m_pPlayer);
		m_pPlayer->MiniGameInit();
		LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Add_GameObject(m_pPlayer);
		m_pPlayer->AddRef();

		LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Add_GameObject(Monster::Create<Shadow>(GRPDEV));

		Layer* pUiLayer = m_pMainScene->Get_Layer(LAYER_TYPE::LAYER_USER_INTERFACE);
		LayerList[(long)LAYER_TYPE::LAYER_USER_INTERFACE] = pUiLayer;
		pUiLayer->AddRef();
	}
	else {
		if (nullptr == Monster::Get_Camera()) {
			Add_GameObjectToScene<CameraObject>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
			Monster::Set_Camera(static_cast<CameraObject*>(LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Get_GameObject(L"Camera")));
			Monster::Set_Camera(static_cast<CameraObject*>(Monster::Set_Target(L"Camera")));
		}
		if (nullptr == Monster::Get_Player()) {
			Add_GameObjectToScene<Player>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"Player");
			Monster::Set_Player(static_cast<Player*>((LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Get_GameObject(L"Player"))));
			m_pPlayer = static_cast<Player*>(Monster::Set_Target(L"Player"));
		}
	}


	for (auto pLayer : LayerList)
	{
		for (auto pObj : *pLayer->Get_GameObjectList()) {
			CollisionManager::GetInstance()->Delete_ColliderObject(pObj);
			CollisionManager::GetInstance()->Add_ColliderObject(pObj);
		}
	}

	m_pMainUI = nullptr;
	
	// 디버그 키 진입 하고싶을때 킬것
	//SceneManager::GetInstance()->Set_CurrentScene(this);

	return S_OK;
}

HRESULT MiniGameScene::End_MiniGame()
{
	if (!m_bEffect)
	{
		dynamic_cast<StageBlackOut*>(EffectManager::GetInstance()->Get_Scene())->Set_Pos({ 60.671,0.5f,43.405 }, false, 0, false);
		TileManager::GetInstance()->Set_CurStage(TILE_STAGE::TILE_DOCHER1);
		m_bEffect = true;
	}
	if (!TileManager::GetInstance()->Get_Loading())
	{
		TileManager::GetInstance()->Set_Stage();
		TileManager::GetInstance()->Stage_Update(0.1);
		TileManager::GetInstance()->Set_EndLoading(TRUE);
		TileManager::GetInstance()->Set_PotalBgmStart(TRUE);
	}

	if (nullptr != Monster::Get_Player())
		Monster::Get_Player()->MiniGameExit();
	if (nullptr != Monster::Get_Camera())
		Monster::Get_Camera()->Exit_MiniGame();

	for (auto& pLayer : LayerList)
	{
		for (auto pObj : *pLayer->Get_GameObjectList()) {
			for (auto& pObj : *pLayer->Get_GameObjectList()) {
				CollisionManager::GetInstance()->Delete_ColliderObject(pObj);
			}
		}
	}
	CollisionManager::GetInstance()->Add_ColliderObject(Monster::Get_Player());
	SoundManager::GetInstance()->Stop_AllSound();
	m_pMainUI->Set_EnableDisplayHPBar(false);
	m_pMainUI = nullptr;
	MonsterManager::GetInstance()->Release_Static_Batich();
	SceneManager::GetInstance()->Scene_Transition(m_pMainScene);
	return S_OK;
}
HRESULT MiniGameScene::Ready_Enviroment_Layer() {

	MonsterManager::GetInstance()->Ready_Origin_Buffer();

	int iHurdle[5] = {};
	for (int i = 0; i < 5; ++i) {
		iHurdle[i] = RANDOM::Get_int(0, 4);
	}

	for (_float z = 0; z < MINIGAMETILEZ; ++z)
	{
		for (_float x = 0; x < MINIGAMETILEX; ++x)
		{
			CubeFloorTile* pTile = CubeFloorTile::Create(GRPDEV);
			_vec3 vScale = { 1.f,1.f,1.f };
			POS(pTile)->x = 2.f * x * vScale.x;
			POS(pTile)->y = -vScale.y;
			POS(pTile)->z = 2.f * z * vScale.z;


			if ((x > 5) && (x< 20)) {
				if ((int)x % 3 == 0) {
					if (iHurdle[(int)(x - 5) / 3] == (int)z) {
						pTile->Set_Hurdle(false);
					}
					else
						pTile->Set_Hurdle(true);
				}
			}

			pTile->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_TERRAIN);
			pTile->Set_ObjectTag(L"Cube");
			pTile->Set_TileNumber(x + z * MINIGAMETILEX);
			pTile->Set_OriginPos(*POS(pTile));
			pTile->Get_TransCom()->Set_Scale(vScale.x, vScale.y, vScale.z);
			Monster::Staic_Obj(GRPDEV, pTile->Get_TransCom());

			MonsterManager::GetInstance()->Get_Tiles()->push_back(pTile);
			LayerList[(long)LAYER_TYPE::LAYER_STATIC_OBJECT]->Add_GameObject(pTile);
		}
	}
	if (FAILED(MonsterManager::GetInstance()->Ready_Static_Batch(GRPDEV)))
		return E_FAIL;

	SkyBox* pSkybox = SkyBox::Create(GRPDEV);
	LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Add_GameObject(pSkybox);

	m_pChaser = Tesseract::Create(GRPDEV);
	m_pChaser->Set_SceneEventTrigger(&m_iEventTrigger);
	Monster::Add_Monster_to_Scene(m_pChaser, L"Chaser", GAMEOBJECT_TYPE::OBJECT_MONSTER, this);
	return S_OK;
}
HRESULT MiniGameScene::Ready_GameLogic_Layer() {

	return S_OK;
}

VOID MiniGameScene::Ready_MonsterHurdle(_vec3 _Src, _vec3 _Dst)
{
	bool bx = _Src.x == _Dst.x;
	bool by = _Src.y == _Dst.y;
	bool bz = _Src.z == _Dst.z;

	if ((bx + by + bz) != 1) return;

	_vec3 vDir = _Src - _Dst;
	float fDis = D3DXVec3Length(&vDir);

	if (fDis < 5.f) return;

	D3DXVec3Normalize(&vDir, &vDir);

	for (float i = 0.f; i < fDis; i += 5)
	{
		int		iCount = RANDOM::Get_int(1, 5);
		bool	bDir = (RANDOM::Get_int(0, 9) < 5);

		_vec3	vSrc = (bDir ? _Src : _Dst) + vDir * i;
		_vec3	vDst = (bDir ? _Dst : _Src) + vDir * i;

		_float	fSpeed = RANDOM::Get_float(1.f, 6.f);

		uint8_t Type = RANDOM::Get_int((uint8_t)MONSTER_TYPE::Bat, (uint8_t)MONSTER_TYPE::Random - 1);

		for (int j = 0; j < iCount; ++j)
		{
			GameObject* pHurdle = nullptr;
			switch (Type)
			{
			default:
				break;
			case (uint8_t)Engine::MONSTER_TYPE::Bat:
				pHurdle = Monster::Create<Bat>(GRPDEV, vSrc, vDst, fSpeed, 2.f);
				Monster::Add_Monster_to_Scene(pHurdle, L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE, this);
				break;
			case (uint8_t)Engine::MONSTER_TYPE::ScorpionEvilSoul:
				pHurdle = Monster::Create<ScorpionEvilSoul>(GRPDEV, vSrc, vDst, fSpeed, 2.f);
				Monster::Add_Monster_to_Scene(pHurdle, L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE, this);
				break;
			case (uint8_t)Engine::MONSTER_TYPE::ShotGunEvilSoul:
				pHurdle = Monster::Create<ShotGunEvilSoul>(GRPDEV, vSrc, vDst, fSpeed, 2.f);
				Monster::Add_Monster_to_Scene(pHurdle, L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE, this);
				break;
			case (uint8_t)Engine::MONSTER_TYPE::EvilSlime:
				pHurdle = Monster::Create<EvilSlime>(GRPDEV, vSrc, vDst, fSpeed, 2.f);
				Monster::Add_Monster_to_Scene(pHurdle, L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE, this);
				break;
			}
			m_vecHurdles[0].push_back(pHurdle);
		}
	}
}

MiniGameScene* MiniGameScene::Create(LPDIRECT3DDEVICE9 _GRPDEV, Scene* pCurrScene) {
	MiniGameScene* LS = new MiniGameScene(_GRPDEV);
	if (FAILED(LS->Ready_Scene(pCurrScene))) {
		MSG_BOX("Cannot Create MiniGameScene.");
		Safe_Release(LS);
		return nullptr;
	}
	return LS;
}
void MiniGameScene::Free() {
	Scene::Free();
}

