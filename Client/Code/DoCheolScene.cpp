#include "../Include/PCH.h"
#include "DoCheolScene.h"

DoCheolScene::DoCheolScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV) {}
DoCheolScene::~DoCheolScene() {}
HRESULT   DoCheolScene::Ready_Scene() {
    Scene::Ready_Scene();

    UIManager::GetInstance()->Ready_UIManager(GRPDEV);
    
    //ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Appear");
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Stand");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/NoneAnimation");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/RageUp");
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/RightSwing");
    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Charge");
    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/FullSwing");
    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/TwoHandSlam");
    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Death");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/TwoHandSlamShake");
    //ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Supporter");

    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss/Effect");

    ProtoManager::GetInstance()->Ready_Prototype(GRPDEV);
	MonsterManager::GetInstance()->Load_Textures_from_Folder(GRPDEV, L"../../MonsterManager");

    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Tile");
    if (FAILED(Ready_Enviroment_Layer()))           return E_FAIL;
    if (FAILED(Ready_GameLogic_Layer()))            return E_FAIL;
    if (FAILED(Ready_UserInterface_Layer()))        return E_FAIL;

	HANDLE	hFile = CreateFile(L"../../Data/Docheol.dat", // 파일 이름이 포함된 경로
		GENERIC_READ,		// 파일 접근 모드(GENERIC_WRITE : 쓰기, GENERIC_READ : 읽기)
		NULL,				// 공유 방식(파일이 열려 있는 상태에서 다른 프로세스가 오픈 할 때 허가하는 것에 대해 설정, 지정하지 않을 경우 NULL)
		NULL,				// 보안 속성(기본값인 경우 NULL)
		OPEN_EXISTING,		// 파일이 없을 경우 파일을 생성하여 저장(OPEN_EXISTING : 파일이 있을 경우에만 로드)
		FILE_ATTRIBUTE_NORMAL,	// 파일 속성(아무런 속성이 없는 일반 파일)
		NULL);				// 생성될 파일의 속성ㅇ르 제공할 템플릿 파일

	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("로드 실패");
		return E_FAIL;
	}

	DWORD	dwByte(0);		// eof 역할
	_int             iTilenum = 0;
	TILE_SIDE        eTileSide = TILE_SIDE::TILE_END;
	TILE_STATE       eTileState = TILE_STATE::STATE_END;
	TILEMODE_CHANGE  eTileMode = TILEMODE_CHANGE::MODE_END;
	TILE_STAGE	     eTileStage = TILE_STAGE::STAGE_END;
	TILE_STAGE		 eNext = TILE_STAGE::STAGE_END;
	_tchar			 cTileName[128] = {};
	_vec3		     Info = {};
	_vec3			 Scale = {};
	_vec3			 Rotation = {};
	_int		     iTileTextureCnt = 0;
	_vec3			 vNextPos = {};
	_bool		     bAni = false;
	_int      i = 0;
	TILE_SPAWNER		eSpawn = TILE_SPAWNER::SPAWN_END;
	while (true)
	{
		ReadFile(hFile, &Info, sizeof(_vec3), &dwByte, NULL);
		ReadFile(hFile, &iTilenum, sizeof(_int), &dwByte, NULL);
		ReadFile(hFile, &eTileSide, sizeof(TILE_SIDE), &dwByte, NULL);
		ReadFile(hFile, &eTileState, sizeof(TILE_STATE), &dwByte, NULL);
		ReadFile(hFile, &eTileMode, sizeof(TILEMODE_CHANGE), &dwByte, NULL);
		ReadFile(hFile, &cTileName, sizeof(_tchar) * 128, &dwByte, NULL);
		ReadFile(hFile, &Scale, sizeof(_vec3), &dwByte, NULL);
		ReadFile(hFile, &Rotation, sizeof(_vec3), &dwByte, NULL);
		ReadFile(hFile, &eTileStage, sizeof(TILE_STAGE), &dwByte, NULL);
		ReadFile(hFile, &iTileTextureCnt, sizeof(_int), &dwByte, NULL);
		ReadFile(hFile, &vNextPos, sizeof(_vec3), &dwByte, NULL);
		ReadFile(hFile, &bAni, sizeof(_bool), &dwByte, NULL);
		ReadFile(hFile, &eSpawn, sizeof(TILE_SPAWNER), &dwByte, NULL);
		ReadFile(hFile, &eNext, sizeof(TILE_STAGE), &dwByte, NULL);

		if (0 == dwByte)
			break;

		GameObject* GOBJ = nullptr;
		//GRPDEV->AddRef();
		if (eTileState == TILE_STATE::STATE_NORMAL && eSpawn != TILE_SPAWNER::SPAWN_END)
		{
			GOBJ = Spawner::Create(GRPDEV, eTileSide, eSpawn, Info);
		}
		else
			GOBJ = CXZTile::Create(GRPDEV, eTileSide, eTileState);

		

		if (eTileState == TILE_STATE::STATE_UNDERTILE)
			++i;

		GOBJ->Set_ObjectTag(L"CXZTile");
		dynamic_cast<TileInfo*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Set_TileStage(eTileStage);

		if (eTileState == TILE_STATE::STATE_DESTORY || eTileState == TILE_STATE::STATE_ANIMATION || eTileState == TILE_STATE::STATE_POTALEFFECT)
			dynamic_cast<TileInfo*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Set_TileAnimaiton(cTileName, iTileTextureCnt, eTileSide, eTileState, eTileMode, iTilenum, vNextPos, bAni);
		else
		{
			dynamic_cast<TileInfo*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Set_TileAll(nullptr, cTileName, eTileSide, eTileState, eTileMode, iTilenum, vNextPos, eNext);
			dynamic_cast<TileInfo*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))
				->Set_TextureID(ResourceManager::GetInstance()->Find_Texture(dynamic_cast<TileInfo*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_TileTextureName().c_str()));
		}

		dynamic_cast<TileInfo*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Set_TileSpawner(eSpawn);
		dynamic_cast<Transform*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Scale(Scale);
		dynamic_cast<Transform*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Rotation(Rotation);
		dynamic_cast<Transform*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Info);
		if (TILE_STATE::STATE_BOOM == eTileState)
			dynamic_cast<TileInfo*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Set_Boom(L"Spr_Object_Explosionjar_Stage01_0");

		TileManager::GetInstance()->Load_TilePush(GOBJ, eTileStage, eTileMode);

	}
	//MSG_BOX("로드 성공");
	CloseHandle(hFile);

	
    TileManager::GetInstance()->Set_StageCnt();

    TileManager::GetInstance()->Set_CurStage(TILE_STAGE::TILE_DOCHERBOSS);
    TileManager::GetInstance()->Set_Stage();

    KeyManager::GetInstance()->Ready_KeyManager(hInst, hWnd);
    CollisionManager::GetInstance()->Get_AllObjectOfScene();
    return S_OK;
}
INT    DoCheolScene::Update_Scene(CONST FLOAT& _DT) {
	TileManager::GetInstance()->Stage_Update(_DT);
    CollisionManager::GetInstance()->Update_CollisionManager();
    return Scene::Update_Scene(_DT);
}
VOID DoCheolScene::LateUpdate_Scene(CONST FLOAT& _DT) {
    Scene::LateUpdate_Scene(_DT);
    TileManager::GetInstance()->Stage_LateUpdate(_DT);
    CollisionManager::GetInstance()->LateUpdate_CollisionManager();
    CollisionManager::GetInstance()->Render_CollisionManager();
}
VOID DoCheolScene::Render_Scene() { }
HRESULT DoCheolScene::Ready_Enviroment_Layer() {
    Layer* LYR = Layer::Create();
    if (nullptr == LYR) return E_FAIL;
    GameObject* GOBJ = nullptr;
    LayerList.push_back(LYR);
    return S_OK;
}
HRESULT DoCheolScene::Ready_GameLogic_Layer() {
    Add_GameObjectToScene<CameraObject>	(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
    Add_GameObjectToScene<Player>		(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"Player");
	dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos({63.929,0.5f,90.981f});
    Add_GameObjectToScene<Terrain>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Terrain");
	Add_GameObjectToScene<Tile>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Tile");
    return S_OK;
}
HRESULT DoCheolScene::Ready_UserInterface_Layer() {
    Add_GameObjectToScene<MainUI>		(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"MainUI");
    return S_OK;
}
DoCheolScene* DoCheolScene::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
    DoCheolScene* LS = new DoCheolScene(_GRPDEV);
    SceneManager::GetInstance()->Set_CurrentScene(LS);
    if (FAILED(LS->Ready_Scene())) {
        MSG_BOX("Cannot Create DoCheolScene.");
        Safe_Release(LS);
        return nullptr;
    }
    return LS;
}
void DoCheolScene::Free() {
    Scene::Free();
}