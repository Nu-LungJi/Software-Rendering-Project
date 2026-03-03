#include "../Include/PCH.h"

MiniGameScene::MiniGameScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV), m_pMainScene(nullptr) {}
MiniGameScene::~MiniGameScene() {}
HRESULT	MiniGameScene::Ready_Scene() {

	Scene::Ready_Scene();
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

	return S_OK;
}
INT	 MiniGameScene::Update_Scene(CONST FLOAT& _DT) {
	TileManager::GetInstance()->Update_TileList(_DT);
	CollisionManager::GetInstance()->Update_CollisionManager();
	return Scene::Update_Scene(_DT);
}
VOID MiniGameScene::LateUpdate_Scene(CONST FLOAT& _DT) {
	Scene::LateUpdate_Scene(_DT);
	TileManager::GetInstance()->LateUpdate_Tile(_DT);
	CollisionManager::GetInstance()->LateUpdate_CollisionManager();
	CollisionManager::GetInstance()->Render_CollisionManager();
}
VOID MiniGameScene::Render_Scene() {}
HRESULT MiniGameScene::Start_MiniGame(LPDIRECT3DDEVICE9 GRPDEV, Scene* pCurrScene)
{
	if (nullptr == pCurrScene)
		return E_POINTER;

	MiniGameScene* pScene = DBG_NEWW MiniGameScene(GRPDEV);

	Monster::Set_Camera(static_cast<CameraObject*>(pCurrScene->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Get_GameObject(L"Camera")));

	if (Monster::Get_Camera() == nullptr)
		return E_POINTER;

	Monster::Set_Player(static_cast<Player*>(pCurrScene->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Get_GameObject(L"Player")));

	if (Monster::Get_Player == nullptr)
		return E_POINTER;

	Monster::Get_Player()->MiniGameInit();
	
	SceneManager::GetInstance()->Set_CurrentScene(pScene);

	return pScene->Ready_Scene();
}
HRESULT MiniGameScene::End_MiniGame()
{
	Monster::Get_Player()->MiniGameExit();
	return	SceneManager::GetInstance()->Scene_Transition(m_pMainScene);
}
HRESULT MiniGameScene::Ready_Enviroment_Layer() {

	LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Add_GameObject(Monster::Create<Shadow>(GRPDEV));

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

	if (nullptr == Monster::Get_Camera()) {
		Add_GameObjectToScene<CameraObject>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
		Monster::Set_Camera(static_cast<CameraObject*>(LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Get_GameObject(L"Camera")));
	}
	if (nullptr == Monster::Get_Player()) {
		Add_GameObjectToScene<Player>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"Player");
		Monster::Set_Player(static_cast<Player*>((LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Get_GameObject(L"Player"))));
	}

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
				Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, vSrc, vDst, fSpeed, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
				break;
			case (uint8_t)Engine::MONSTER_TYPE::ScorpionEvilSoul:
				Monster::Add_Monster_to_Scene(Monster::Create<ScorpionEvilSoul>(GRPDEV, vSrc, vDst, fSpeed, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
				break;
			case (uint8_t)Engine::MONSTER_TYPE::ShotGunEvilSoul:
				Monster::Add_Monster_to_Scene(Monster::Create<ShotGunEvilSoul>(GRPDEV, vSrc, vDst, fSpeed, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
				break;
			case (uint8_t)Engine::MONSTER_TYPE::EvilSlime:
				Monster::Add_Monster_to_Scene(Monster::Create<EvilSlime>(GRPDEV, vSrc, vDst, fSpeed, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
				break;
			}
		}
	}

	return S_OK;
}

MiniGameScene* MiniGameScene::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	MiniGameScene* LS = new MiniGameScene(_GRPDEV);
	SceneManager::GetInstance()->Set_CurrentScene(LS);
	if (FAILED(LS->Ready_Scene())) {
		MSG_BOX("Cannot Create MiniGameScene.");
		Safe_Release(LS);
		return nullptr;
	}
	return LS;
}
void MiniGameScene::Free() {

	Scene::Free();
}

