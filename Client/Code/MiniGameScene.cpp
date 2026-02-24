#include "../Include/PCH.h"

MiniGameScene::MiniGameScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV) {}
MiniGameScene::~MiniGameScene() {}
HRESULT	MiniGameScene::Ready_Scene() {
	Scene::Ready_Scene();
	ProtoManager::GetInstance()->Ready_Prototype(GRPDEV);
	UIManager::GetInstance()->Ready_UIManager(GRPDEV);
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

	for (_float z = 0; z < 256.f; ++z)
	{
		for (_float x = 0; x < 10.f; ++x)
		{
			Monster::Add_Monster_to_Scene(CubeFloorTile::Create(GRPDEV, { x* 2.f,-1.f,z*2.f }), L"CUBE");
		}
	}

	return S_OK;
}
HRESULT MiniGameScene::Ready_GameLogic_Layer(){
	Add_GameObjectToScene<CameraObject>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
	Monster::Add_Monster_to_Scene(Player::Create(GRPDEV), L"Player", GAMEOBJECT_TYPE::OBJECT_PLAYER);
	//Monster::Add_Monster_to_Scene(CubeFloorTile::Create(GRPDEV), L"Cube", GAMEOBJECT_TYPE::OBJECT_END);

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