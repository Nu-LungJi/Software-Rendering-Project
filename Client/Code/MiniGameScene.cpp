#include "../Include/PCH.h"

MiniGameScene::MiniGameScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV) {}
MiniGameScene::~MiniGameScene() {}
HRESULT	MiniGameScene::Ready_Scene() {
	Scene::Ready_Scene();
	ProtoManager::GetInstance()->Ready_Prototype(GRPDEV);
	UIManager::GetInstance()->Ready_UIManager(GRPDEV);
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Resource/Effect");
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Resource/CubeFloorTile");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Resource");
	MonsterManager::GetInstance()->Load_Textures_from_Folder(GRPDEV, L"../../MonsterManager");
	MonsterManager::GetInstance();
	if (FAILED(Ready_Enviroment_Layer()))		return E_FAIL;
	if (FAILED(Ready_GameLogic_Layer()))		return E_FAIL;
	if (FAILED(Ready_UserInterface_Layer()))		return E_FAIL;
	KeyManager::GetInstance()->Ready_KeyManager(hInst, hWnd);
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
HRESULT MiniGameScene::Ready_Enviroment_Layer() {

	//Add_GameObjectToScene<Terrain>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Terrain");
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
	Add_GameObjectToScene<CameraObject>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");

	Monster::Set_Camera(static_cast<CameraObject*>(LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Get_GameObject(L"Camera")));
	Add_GameObjectToScene<Player>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"Player");
	Monster::Set_Player(static_cast<Player*>((LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Get_GameObject(L"Player"))));


	// 랜덤으로 2중 for 문 돌려서 소환할 예정
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 10.f,1.f,0.f }, { 10.f,1.f,10.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 10.f,1.f,0.f }, { 10.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 10.f,1.f,0.f }, { 10.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 15.f,1.f,0.f }, { 15.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 15.f,1.f,0.f }, { 15.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 15.f,1.f,0.f }, { 15.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 20.f,1.f,0.f }, { 20.f,1.f,10.f }, 5.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 20.f,1.f,0.f }, { 20.f,1.f,10.f }, 5.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<ShotGunEvilSoul>(GRPDEV, { 25.f,1.f,10.f }, { 25.f,1.f,0.f }, 5.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 30.f,1.f,10.f }, { 30.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 30.f,1.f,10.f }, { 30.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 30.f,1.f,10.f }, { 30.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 35.f,1.f,10.f }, { 35.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 35.f,1.f,10.f }, { 35.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 35.f,1.f,10.f }, { 35.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 40.f,1.f,0.f }, { 40.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 40.f,1.f,0.f }, { 40.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 40.f,1.f,10.f }, { 40.f,1.f,0.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 40.f,1.f,10.f }, { 40.f,1.f,0.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 45.f,1.f,0.f }, { 45.f,1.f,10.f }, 5.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 45.f,1.f,0.f }, { 45.f,1.f,10.f }, 5.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 45.f,1.f,0.f }, { 45.f,1.f,10.f }, 5.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<ShotGunEvilSoul>(GRPDEV, { 50.f,1.f,5.f }, { 50.f,1.f,5.f }, 6.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 55.f,1.f,10.f }, { 55.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 55.f,1.f,10.f }, { 55.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<ShotGunEvilSoul>(GRPDEV, { 65.f,1.f,0.f }, { 60.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 70.f,1.f,10.f }, { 70.f,1.f,0.f }, 5.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 70.f,1.f,10.f }, { 70.f,1.f,0.f }, 5.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 70.f,1.f,10.f }, { 70.f,1.f,0.f }, 5.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 75.f,1.f,0.f }, { 75.f,1.f,10.f }, 6.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 75.f,1.f,0.f }, { 75.f,1.f,10.f }, 6.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 75.f,1.f,0.f }, { 75.f,1.f,10.f }, 6.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 75.f,1.f,0.f }, { 75.f,1.f,10.f }, 6.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 80.f,1.f,0.f }, { 80.f,1.f,10.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 80.f,1.f,0.f }, { 80.f,1.f,10.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 85.f,1.f,10.f }, { 85.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 85.f,1.f,10.f }, { 85.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 85.f,1.f,10.f }, { 85.f,1.f,0.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 90.f,1.f,0.f }, { 90.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 90.f,1.f,0.f }, { 90.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 90.f,1.f,0.f }, { 90.f,1.f,10.f }, 4.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);

	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 95.f,1.f,0.f }, { 95.f,1.f,10.f }, 3.f, 2.f), L"Hurdle", GAMEOBJECT_TYPE::OBJECT_HURDLE);


	//Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV,{15.f,0.f,0.f},3.f), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
	//m_pCamera->Set_Target(LayerList[(long)LAYER_TYPE::LAYER_DYNAMIC_OBJECT]->Get_GameObject(L"Monster"));

	//Monster::Add_Monster_to_Scene(Monster::Create<ScorpionEvilSoul>(GRPDEV,{3.f,0.f,3.f},3.f), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
	//Monster::Add_Monster_to_Scene(Monster::Create<ShotGunEvilSoul>(GRPDEV,{0.f,0.f,3.f},3.f), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
	//Monster::Add_Monster_to_Scene(Monster::Create<EvilSlime>(GRPDEV,{3.f,0.f,0.f},3.f), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
	//Monster::Add_Monster_to_Scene(CubeFloorTile::Create(GRPDEV), L"Cube", GAMEOBJECT_TYPE::OBJECT_END);
	//Monster::Add_Monster_to_Scene(Hurdle::Create(GRPDEV, { 20.f,0.f,0.f }, { 0.f,0.f,0.f }), L"Hurdle");

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

