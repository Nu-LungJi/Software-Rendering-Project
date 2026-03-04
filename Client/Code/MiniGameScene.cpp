#include "../Include/PCH.h"

MiniGameScene::MiniGameScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV), m_pMainScene(nullptr), m_fTimer(0.f) {}
MiniGameScene::~MiniGameScene() {}

HRESULT	MiniGameScene::Ready_Scene(Scene* pScene) {

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

	m_fTimer += _DT;
	

	if (KEY_DOWN(DIK_P)) {
		POS(Monster::Get_Player())->y = 1.5f;
	}

	CollisionManager::GetInstance()->Update_CollisionManager();
	return Scene::Update_Scene(_DT);
}
VOID MiniGameScene::LateUpdate_Scene(CONST FLOAT& _DT) {
	Scene::LateUpdate_Scene(_DT);
	TileManager::GetInstance()->LateUpdate_Tile(_DT);
	CollisionManager::GetInstance()->LateUpdate_CollisionManager();
	CollisionManager::GetInstance()->Render_CollisionManager();

	if (POS(Monster::Get_Player())->y < -2.f) {
		if(m_pMainScene != nullptr)
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
		LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Add_GameObject(pCamera);
		pCamera->AddRef();

		Player* pPlayer = static_cast<Player*>(m_pMainScene->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Get_GameObject(L"Player"));
		Monster::Set_Player(pPlayer);
		pPlayer->MiniGameInit();
		LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Add_GameObject(pPlayer);
		pPlayer->AddRef();

		LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Add_GameObject(Monster::Create<Shadow>(GRPDEV));

		Layer* pUiLayer = m_pMainScene->Get_Layer(LAYER_TYPE::LAYER_USER_INTERFACE);
		for (auto pUI : *(pUiLayer->Get_GameObjectList())) {
			LayerList[(long)LAYER_TYPE::LAYER_USER_INTERFACE]->Add_GameObject(pUI);
			pUI->AddRef();
		}
	}
	else {
		if (nullptr == Monster::Get_Camera()) {
			Add_GameObjectToScene<CameraObject>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
			Monster::Set_Camera(static_cast<CameraObject*>(LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Get_GameObject(L"Camera")));
		}
		if (nullptr == Monster::Get_Player()) {
			Add_GameObjectToScene<Player>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"Player");
			Monster::Set_Player(static_cast<Player*>((LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Get_GameObject(L"Player"))));
		}
	}


	for (auto pLayer : LayerList)
	{
		for (auto pObj : *pLayer->Get_GameObjectList()) {
			CollisionManager::GetInstance()->Delete_ColliderObject(pObj);
			CollisionManager::GetInstance()->Add_ColliderObject(pObj);
		}
	}

	SceneManager::GetInstance()->Set_CurrentScene(this);

	return S_OK;
}

HRESULT MiniGameScene::End_MiniGame()
{
	if(nullptr!= Monster::Get_Player())
		Monster::Get_Player()->MiniGameExit(); 
	if (nullptr != Monster::Get_Camera())
		Monster::Get_Camera()->Exit_MiniGame();

	for (auto pLayer : LayerList)
	{
		for (auto pObj : *pLayer->Get_GameObjectList()) {
			CollisionManager::GetInstance()->Delete_ColliderObject(pObj);
		}
	}


	SceneManager::GetInstance()->Set_CurrentScene(m_pMainScene);
	//SceneManager::GetInstance()->Scene_Transition(m_pMainScene);
	return S_OK;
}

HRESULT MiniGameScene::Ready_Enviroment_Layer() {

	MonsterManager::GetInstance()->Ready_Origin_Buffer();
	for (_float z = 0; z < MINIGAMETILEZ; ++z)
	{
		for (_float x = 0; x < MINIGAMETILEX; ++x)
		{
			CubeFloorTile* pTile = CubeFloorTile::Create(GRPDEV);
			_vec3 vScale = { 1.f,1.f,1.f };
			POS(pTile)->x = 2.f * x * vScale.x;
			POS(pTile)->y = -vScale.y;
			POS(pTile)->z = 2.f * z * vScale.z;

			pTile->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_TERRAIN);
			pTile->Set_ObjectTag(L"Cube");
			pTile->Set_TileNumber(x + z * MINIGAMETILEX);

			pTile->Get_TransCom()->Set_Scale(vScale.x, vScale.y, vScale.z);
			Monster::Staic_Obj(GRPDEV, pTile->Get_TransCom());

			MonsterManager::GetInstance()->Get_Tiles()->push_back(pTile);
			LayerList[(long)LAYER_TYPE::LAYER_STATIC_OBJECT]->Add_GameObject(pTile);
		}
	}
	if (FAILED(MonsterManager::GetInstance()->Ready_Static_Batch(GRPDEV)))
		return E_FAIL;

	return S_OK;
}
HRESULT MiniGameScene::Ready_GameLogic_Layer() {

	for (float i = 20; i < 200; i += 5)
	{
		int		iCount	= RANDOM::Get_int(1, 5);
		bool	bDir	= (RANDOM::Get_int(0,9) < 5);
		_vec3	vSrc	= { i, 1.f, 10.f * bDir };
		_vec3	vDst	= { i, 1.f, 10.f * (!bDir)};
		_float	fSpeed	= RANDOM::Get_float(1.f, 6.f);

		uint8_t Type	= RANDOM::Get_int((uint8_t)MONSTER_TYPE::Bat, (uint8_t)MONSTER_TYPE::Random -1);

		//if (iCount == 1)
		//	if (RANDOM::Get_float(0.f, 10.f) < 5.f) {
		//		Monster::Add_Monster_to_Scene(Monster::Create<Bullet_Chain_Head>(GRPDEV, vSrc, vDst, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
		//		continue;
		//	}

		for (int j = 0; j < iCount; ++j)
		{
			switch (Type)
			{
			default:
			case (uint8_t)Engine::MONSTER_TYPE::Bat:
				Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, vSrc, vDst, fSpeed, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE, this);
				break;
			case (uint8_t)Engine::MONSTER_TYPE::ScorpionEvilSoul:
				Monster::Add_Monster_to_Scene(Monster::Create<ScorpionEvilSoul>(GRPDEV, vSrc, vDst, fSpeed, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE, this);
				break;
			case (uint8_t)Engine::MONSTER_TYPE::ShotGunEvilSoul:
				Monster::Add_Monster_to_Scene(Monster::Create<ShotGunEvilSoul>(GRPDEV, vSrc, vDst, fSpeed, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE, this);
				break;
			case (uint8_t)Engine::MONSTER_TYPE::EvilSlime:
				Monster::Add_Monster_to_Scene(Monster::Create<EvilSlime>(GRPDEV, vSrc, vDst, fSpeed, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE, this);
				break;
			}
		}
	}

	return S_OK;
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

