#include "../Include/PCH.h"
#include "MapScene.h"

MapScene::MapScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV) {}
MapScene::~MapScene() {}
HRESULT	MapScene::Ready_Scene() {
	Scene::Ready_Scene();

	ProtoManager::GetInstance()->Ready_Prototype(GRPDEV);

	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Tile");
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Resource");
	//ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../ReSource/Spr_Monster_EvilFrog");
	MonsterManager::GetInstance()->Load_Textures_from_Folder(GRPDEV, L"../../MonsterManager");

	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss");
	ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../UI");
	Ready_GameLogic_Layer(L"GameLogic_Layer");
	Ready_UserInterface_Layer(L"UI_Layer");

	UIManager::GetInstance()->Ready_UIManager(GRPDEV);


	{
	HANDLE	hFile = CreateFile(L"../../Data/Cheonglock.dat", // 파일 이름이 포함된 경로
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
	_vec3		     Info  = {};
	_vec3			 Scale = {};
	_vec3			 Rotation = {};
	_int		     iTileTextureCnt = 0;
	_vec3			 vNextPos = {};
	_bool		     bAni = false;
	_int				 i = 0;
	TILE_SPAWNER		eSpawn = TILE_SPAWNER::SPAWN_END;
	while (true)
	{
		ReadFile(hFile, &Info,			  sizeof(_vec3),		   &dwByte, NULL);
		ReadFile(hFile, &iTilenum,		  sizeof(_int),			   &dwByte, NULL);
		ReadFile(hFile, &eTileSide,		  sizeof(TILE_SIDE),	   &dwByte, NULL);
		ReadFile(hFile, &eTileState,	  sizeof(TILE_STATE),	   &dwByte, NULL);
		ReadFile(hFile, &eTileMode,		  sizeof(TILEMODE_CHANGE), &dwByte, NULL);
		ReadFile(hFile, &cTileName,		  sizeof(_tchar) * 128,    &dwByte, NULL);
		ReadFile(hFile, &Scale,			  sizeof(_vec3),	       &dwByte, NULL);
		ReadFile(hFile, &Rotation,	      sizeof(_vec3),	       &dwByte, NULL);
		ReadFile(hFile, &eTileStage,	  sizeof(TILE_STAGE),	   &dwByte, NULL);
		ReadFile(hFile, &iTileTextureCnt, sizeof(_int),			   &dwByte, NULL);
		ReadFile(hFile, &vNextPos,		  sizeof(_vec3),		   &dwByte, NULL);
		ReadFile(hFile, &bAni,			  sizeof(_bool),	       &dwByte, NULL);
		ReadFile(hFile, &eSpawn,		  sizeof(TILE_SPAWNER),    &dwByte, NULL);
		ReadFile(hFile, &eNext,			  sizeof(TILE_STAGE),	   &dwByte, NULL);
	
		if (0 == dwByte)
			break;
		
		GameObject* GOBJ = nullptr;
		
	
		if (eTileState == TILE_STATE::STATE_NORMAL && eSpawn != TILE_SPAWNER::SPAWN_END)
		{
			GOBJ = Spawner::Create(GRPDEV, eTileSide, eSpawn, Info);
		}else
			GOBJ = CXZTile::Create(GRPDEV, eTileSide, eTileState);
	
	
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
		
		dynamic_cast<Transform*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Scale(Scale);
		dynamic_cast<Transform*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Rotation(Rotation);
		dynamic_cast<Transform*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(Info);
		if (TILE_STATE::STATE_BOOM == eTileState)
			dynamic_cast<TileInfo*>(GOBJ->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Set_Boom(L"Spr_Object_Explosionjar_Stage01_0");
	
		TileManager::GetInstance()->Load_TilePush(GOBJ, eTileStage, eTileMode);
		
	}
	
	MSG_BOX("로드 성공");
	CloseHandle(hFile);
	}

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
	_float      i = 0.1f;
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
	
		if (eTileStage == TILE_STAGE::TILE_DOCHERBOSS && eTileState == TILE_STATE::STATE_POTALGASI)
		{
			dynamic_cast<CXZTile*>(GOBJ)->Tile_Height_Speed((float)i);
			i += 0.3f;
		}
	
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
	MSG_BOX("로드 성공");
	CloseHandle(hFile);

	TileManager::GetInstance()->Set_StageCnt();
	KeyManager::GetInstance()->Ready_KeyManager(hInst, hWnd);
	CollisionManager::GetInstance()->Get_AllObjectOfScene();
	UIManager::GetInstance()->Ready_UIManager(GRPDEV);
	SoundManager::GetInstance()->Play_Sound(L"Stage/Bgm_Stage1-2_Loop.wav", CHANNELID::SOUND_BGM01,0.1f);
	TileManager::GetInstance()->Set_CurStage(TILE_STAGE::TILE_DEFENSE);
	TileManager::GetInstance()->Set_Stage();
	return S_OK;
}
INT	 MapScene::Update_Scene(CONST FLOAT& _DT) {
	
	CollisionManager::GetInstance()->Update_CollisionManager();

		if (!TileManager::GetInstance()->Get_Loading())
		{
			if (TileManager::GetInstance()->Get_MiniGame())
			{
				pMiniGame = MiniGameScene::Create(GRPDEV, this);
				pMiniGame->Start_MiniGame();
				TileManager::GetInstance()->Set_EndLoading(TRUE);
				TileManager::GetInstance()->Set_PotalBgmStart(TRUE);
				TileManager::GetInstance()->Set_MiniGame(false);
				SoundManager::GetInstance()->Stop_AllSound();
				SceneManager::GetInstance()->Set_CurrentScene(pMiniGame);
			}
			else
			{
				TileManager::GetInstance()->Set_Stage();
				TileManager::GetInstance()->Set_EndLoading(TRUE);
				TileManager::GetInstance()->Set_PotalBgmStart(TRUE);
			}
		}
			
		TileManager::GetInstance()->Stage_Update(_DT);
	return Scene::Update_Scene(_DT);
}
VOID MapScene::LateUpdate_Scene(CONST FLOAT& _DT) {
	Scene::LateUpdate_Scene(_DT);


	TileManager::GetInstance()->Stage_LateUpdate(_DT);
	CollisionManager::GetInstance()->LateUpdate_CollisionManager();
	CollisionManager::GetInstance()->Render_CollisionManager();
}
VOID MapScene::Render_Scene() {
	//Scene::Render_Scene();
}
HRESULT MapScene::Ready_Enviroment_Layer(CONST TCHAR* _LTAG) {
	Layer* LYR = Layer::Create();
	if (nullptr == LYR) return E_FAIL;

	GameObject* GOBJ = nullptr;
	LayerList.push_back(LYR);

	return S_OK;
}
HRESULT MapScene::Ready_GameLogic_Layer(CONST TCHAR* _LTAG) {
	
	Add_GameObjectToScene<CameraObject>(LAYER_TYPE:: LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
	Add_GameObjectToScene<Player>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"Player");

	Add_GameObjectToScene<Terrain>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Terrain");
	Add_GameObjectToScene<Tile>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Tile");
	
	ShopKeeper* pObj = ShopKeeper::Create(GRPDEV,{ 22.420f,0.5f, 117.391f });
	pObj->Set_ObjectTag(L"ShopNPC");
	SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pObj);
			
	
	//Add_GameObjectToScene<Rain>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Rain");
	//Cheonlog* pCL = Cheonlog::Create(GRPDEV, {1,1,1});
	//
	//pCL->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER);
	//pCL->Set_ObjectTag(L"CheonLog");
	//SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pCL);

	return S_OK;
}
HRESULT MapScene::Ready_UserInterface_Layer(CONST TCHAR* _LTAG) {

	Add_GameObjectToScene<PlayerInven>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"PlayerInven");
	Add_GameObjectToScene<MainUI>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"MainUI");
	//Add_GameObjectToScene<Augment>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"Argument");

	//Add_GameObjectToScene<ShopUI>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"ShopUI");
	
	return S_OK;
}
MapScene* MapScene::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	MapScene* LS = new MapScene(_GRPDEV);
	SceneManager::GetInstance()->Set_CurrentScene(LS);
	if (FAILED(LS->Ready_Scene())) {
		MSG_BOX("Cannot Create MapScene.");
		Safe_Release(LS);
		return nullptr;
	}
	return LS;
}
void MapScene::Free() {
	Scene::Free();
}