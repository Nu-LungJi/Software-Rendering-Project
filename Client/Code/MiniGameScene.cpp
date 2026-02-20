#include "../Include/PCH.h"

MiniGameScene::MiniGameScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV) {}
MiniGameScene::~MiniGameScene() {}
HRESULT	MiniGameScene::Ready_Scene() {
	Scene::Ready_Scene();

	ProtoManager::GetInstance()->Ready_Prototype(GRPDEV);

	UIManager::GetInstance()->Ready_UIManager(GRPDEV);

	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Tile");

	if (FAILED(Ready_Enviroment_Layer()))			return E_FAIL;
	if (FAILED(Ready_GameLogic_Layer()))			return E_FAIL;
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
VOID MiniGameScene::Render_Scene() {
	//Scene::Render_Scene();
}
HRESULT MiniGameScene::Ready_Enviroment_Layer() {
	Layer* LYR = Layer::Create();
	if (nullptr == LYR) return E_FAIL;

	GameObject* GOBJ = nullptr;

	LayerList.push_back(LYR);

	return S_OK;
}
HRESULT MiniGameScene::Ready_GameLogic_Layer(){
	Add_GameObjectToScene<CameraObject>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
	Add_GameObjectToScene<Player>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"Player");

	//Add_GameObjectToScene<Terrain>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Terrain");
	Add_GameObjectToScene<Tile>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Tile");
	Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, { 0.f,0.f,0.f }));
	//Monster::Add_Monster_to_Scene(CubeFloorTile::Create(GRPDEV),GAMEOBJECT_TYPE::OBJECT_TERRAIN);
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