#include "../Include/PCH.h"

MiniGameScene::MiniGameScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV) {}
MiniGameScene::~MiniGameScene() {}
HRESULT	MiniGameScene::Ready_Scene() {
	Scene::Ready_Scene();
	ProtoManager::GetInstance()->Ready_Prototype(GRPDEV);
	UIManager::GetInstance()->Ready_UIManager(GRPDEV);
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Resource/Monster");
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

	//for (_float z = 0; z < 10.f; ++z)
	//{
	//	for (_float x = 0; x < 10.f; ++x)
	//	{
	//		GameObject* pTile = CubeFloorTile::Create(GRPDEV);
	//		POS(pTile)->x = 2.f* x * SCALE(pTile)->x;
	//		POS(pTile)->y = -1.f * SCALE(pTile)->y;
	//		POS(pTile)->z = 2.f* z * SCALE(pTile)->z;
	//		pTile->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_TERRAIN);
	//		pTile->Set_ObjectTag(L"Cube");
	//		LayerList[(long)LAYER_TYPE::LAYER_STATIC_OBJECT]->Add_GameObject(pTile);
	//		CollisionManager::GetInstance()->Add_ColliderObject(pTile);
	//		//Monster::Add_Monster_to_Scene(CubeFloorTile::Create(GRPDEV, { x * 2.f,-1.f,z * 2.f }), L"CUBE", GAMEOBJECT_TYPE::OBJECT_TERRAIN);
	//	}
	//}

	return S_OK;
}
HRESULT MiniGameScene::Ready_GameLogic_Layer(){
	Add_GameObjectToScene<CameraObject>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
	Monster::Add_Monster_to_Scene(Player::Create(GRPDEV), L"Player", GAMEOBJECT_TYPE::OBJECT_PLAYER);
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV,{0.f,0.f,0.f},3.f), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
	//Monster::Add_Monster_to_Scene(Monster::Create<ScorpionEvilSoul>(GRPDEV,{20.f,0.f,0.f},3.f), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
	//Monster::Add_Monster_to_Scene(Monster::Create<ShotGunEvilSoul>(GRPDEV,{0.f,0.f,30.f},3.f), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
	//Monster::Add_Monster_to_Scene(Monster::Create<EvilSlime>(GRPDEV,{0.f,0.f,0.f},3.f), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
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

