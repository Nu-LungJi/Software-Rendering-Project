#include "../Include/pch.h"
#include "StartScene.h"

StartScene::StartScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV) {}
StartScene::~StartScene() {}

HRESULT   StartScene::Ready_Scene() {
    Scene::Ready_Scene();
    ProtoManager::GetInstance()->Ready_Prototype(GRPDEV);
    UIManager::GetInstance()->Ready_UIManager(GRPDEV);

	MonsterManager::GetInstance()->Load_Textures_from_Folder(GRPDEV, L"../../MonsterManager");
    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Boss");
    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Tile");

    //ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../UI");
    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Resource");
	

    if (FAILED(Ready_Enviroment_Layer()))		return E_FAIL;
    if (FAILED(Ready_GameLogic_Layer()))		return E_FAIL;
    if (FAILED(Ready_UserInterface_Layer()))    return E_FAIL;

    //Load Tile 천록
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
		_vec3		     Info = {};
		_vec3			 Scale = {};
		_vec3			 Rotation = {};
		_int		     iTileTextureCnt = 0;
		_vec3			 vNextPos = {};
		_bool		     bAni = false;
		_int				 i = 0;
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
	
			if (eTileState == TILE_STATE::STATE_NORMAL && eSpawn != TILE_SPAWNER::SPAWN_END)
			{
				GOBJ = Spawner::Create(GRPDEV, eTileSide, eSpawn, Info);
			}
			else
				GOBJ = CXZTile::Create(GRPDEV, eTileSide, eTileState);
	
			if (eTileStage == TILE_STAGE::TILE_DOCHERBOSS || eTileStage == TILE_STAGE::TILE_DOCHER1)
				eTileStage = TILE_STAGE::TILE_DOCHER2;

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
	


			if(GOBJ->Get_ObjectTag().front() = !nullptr)
				TileManager::GetInstance()->Load_TilePush(GOBJ, eTileStage, eTileMode);
	
		}
		CloseHandle(hFile);
	}

	//Load Tile 도철
	{
		HANDLE	LFile = CreateFile(L"../../Data/Docheol.dat", // 파일 이름이 포함된 경로
			GENERIC_READ,		// 파일 접근 모드(GENERIC_WRITE : 쓰기, GENERIC_READ : 읽기)
			NULL,				// 공유 방식(파일이 열려 있는 상태에서 다른 프로세스가 오픈 할 때 허가하는 것에 대해 설정, 지정하지 않을 경우 NULL)
			NULL,				// 보안 속성(기본값인 경우 NULL)
			OPEN_EXISTING,		// 파일이 없을 경우 파일을 생성하여 저장(OPEN_EXISTING : 파일이 있을 경우에만 로드)
			FILE_ATTRIBUTE_NORMAL,	// 파일 속성(아무런 속성이 없는 일반 파일)
			NULL);				// 생성될 파일의 속성ㅇ르 제공할 템플릿 파일
	
		if (LFile == INVALID_HANDLE_VALUE)
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
			ReadFile(LFile, &Info, sizeof(_vec3), &dwByte, NULL);
			ReadFile(LFile, &iTilenum, sizeof(_int), &dwByte, NULL);
			ReadFile(LFile, &eTileSide, sizeof(TILE_SIDE), &dwByte, NULL);
			ReadFile(LFile, &eTileState, sizeof(TILE_STATE), &dwByte, NULL);
			ReadFile(LFile, &eTileMode, sizeof(TILEMODE_CHANGE), &dwByte, NULL);
			ReadFile(LFile, &cTileName, sizeof(_tchar) * 128, &dwByte, NULL);
			ReadFile(LFile, &Scale, sizeof(_vec3), &dwByte, NULL);
			ReadFile(LFile, &Rotation, sizeof(_vec3), &dwByte, NULL);
			ReadFile(LFile, &eTileStage, sizeof(TILE_STAGE), &dwByte, NULL);
			ReadFile(LFile, &iTileTextureCnt, sizeof(_int), &dwByte, NULL);
			ReadFile(LFile, &vNextPos, sizeof(_vec3), &dwByte, NULL);
			ReadFile(LFile, &bAni, sizeof(_bool), &dwByte, NULL);
			ReadFile(LFile, &eSpawn, sizeof(TILE_SPAWNER), &dwByte, NULL);
			ReadFile(LFile, &eNext, sizeof(TILE_STAGE), &dwByte, NULL);
	
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
		//MSG_BOX("로드 성공");
		CloseHandle(LFile);
	}
	PlayingSound = FALSE;
	TileManager::GetInstance()->Set_StageCnt();
  KeyManager::GetInstance()->Ready_KeyManager(hInst, hWnd);
  CollisionManager::GetInstance()->Get_AllObjectOfScene();

	return S_OK;
}
INT    StartScene::Update_Scene(CONST FLOAT& _DT) {
    
	
		if (!TileManager::GetInstance()->Get_Loading())
		{
			if (TileManager::GetInstance()->Get_MiniGame())
			{
				pMiniGame = MiniGameScene::Create(GRPDEV, this);
				pMiniGame->Start_MiniGame();

				TileManager::GetInstance()->Set_BeforeStage();
				TileManager::GetInstance()->Set_CurStage(TILE_STAGE::STAGE_ROLARUN);
				TileManager::GetInstance()->Set_Stage();
				TileManager::GetInstance()->Set_EndLoading(TRUE);
				TileManager::GetInstance()->Set_PotalBgmStart(TRUE);
				TileManager::GetInstance()->Set_MiniGame(false);
				SoundManager::GetInstance()->Stop_AllSound();
				SoundManager::GetInstance()->Play_Sound(L"Stage/BGM_CrossyRoad.wav", CHANNELID::SOUND_BGM01, 1.f);
				SceneManager::GetInstance()->Set_CurrentScene(pMiniGame);
			
			}
			else
			{
				TileManager::GetInstance()->Set_BeforeStage();
				TileManager::GetInstance()->Set_Stage();
				TileManager::GetInstance()->Set_EndLoading(TRUE);
				TileManager::GetInstance()->Set_PotalBgmStart(TRUE);
			}
		}
		IntroToStage(_DT);

        TileManager::GetInstance()->Stage_Update(_DT);
    CollisionManager::GetInstance()->Update_CollisionManager();

    return Scene::Update_Scene(_DT);
}
VOID StartScene::LateUpdate_Scene(CONST FLOAT& _DT) {

    TileManager::GetInstance()->Stage_LateUpdate(_DT);
    Scene::LateUpdate_Scene(_DT);
    CollisionManager::GetInstance()->LateUpdate_CollisionManager();
    CollisionManager::GetInstance()->Render_CollisionManager();


   if (KEY_DOWN(DIK_P)) {
       pMiniGame = MiniGameScene::Create(GRPDEV, this);
       pMiniGame->Start_MiniGame();
       return;
   }
}
VOID StartScene::Render_Scene() {

    //GOBJ = Tile::Create(GRPDEV);
    //GOBJ->Set_ObjectTag(L"Tile");
    //
    //if (nullptr == GOBJ)               return E_FAIL;
    //if (FAILED(LYR->Add_GameObject(GOBJ)))   return E_FAIL;
}
HRESULT StartScene::Ready_Enviroment_Layer() {
    //Add_GameObjectToScene<Terrain>(LAYER_TYPE::LAYER_STATIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Terrain");
    return S_OK;
}
HRESULT StartScene::Ready_GameLogic_Layer() {
    Add_GameObjectToScene<CameraObject>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
    Add_GameObjectToScene<Player>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"Player");
    Add_GameObjectToScene<Rain>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Rain");
    //Add_GameObjectToScene<FinalBoss>         (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"Docheol");
    //Add_GameObjectToScene<Fireball>         (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"Fireball");
    Add_GameObjectToScene<NPC>            (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_NPC     , L"NPC_Tif"      );
    return S_OK;
}
HRESULT StartScene::Ready_UserInterface_Layer() {
    //Add_GameObjectToScene<MainMenuButton>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"MainButton");
    //Add_GameObjectToScene<MainMenu>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"MainMenu");
    Add_GameObjectToScene<MainUI>         (LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI     , L"MainUI"      );
    ShopKeeper* pObj = ShopKeeper::Create(GRPDEV, { 22.420f,0.5f, 117.391f });
    pObj->Set_ObjectTag(L"ShopNPC");
    SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pObj);

    Add_GameObjectToScene<PlayerInven>      (LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI     , L"PlayerInven"   );
    Add_GameObjectToScene<ShopUI>           (LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI,      L"Shop");
    Add_GameObjectToScene<EndingCredit>     (LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI,      L"EndingCredit");
 
    Add_GameObjectToScene<NPCTalk>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"NPCTalk");
	//Cheonlog* pCL = Cheonlog::Create(GRPDEV, { 1,1,1 });
	//
	//pCL->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER);
	//pCL->Set_ObjectTag(L"CheonLog");
	//SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pCL);
    Add_GameObjectToScene<IntroUI>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"IntroUI");
    //Add_GameObjectToScene<SpeechBubble>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"NpcField");
    //Add_GameObjectToScene<PlayerInven>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"Player_Inven");

    return S_OK;
}
VOID StartScene::IntroToStage(CONST FLOAT& _DT) {
	if		(PlayingSound == 0) return;
	if		(PlayingSound == 1) {
		SoundManager::GetInstance()->Play_Sound(L"Stage/Bgm_Stage1-2_Loop.wav", CHANNELID::SOUND_BGM01, 0.f);
		SoundManager::GetInstance()->Play_Sound(L"Stage/Ambience_Rain.wav"	  , CHANNELID::SOUND_BGM02, 0.f);
		PlayingSound = 3;
	}
	if		(PlayingSound == 2) {
		SoundManager::GetInstance()->Play_Sound(L"UI/Intro/Intro_BGM.wav", CHANNELID::SOUND_BGM01, 0.5f);
		PlayingSound = 0;
	}

	else if (PlayingSound == 3) {
		if (Volume01 <= 0.3f) {
			Volume01 += (_DT / 6);
			SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM02, Volume01);
		}
		else {
			static_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Set_EnableFade(FALSE);
			PlayingSound = 4;
		}
	}
	else if (PlayingSound == 4){

		if (Volume02 <= 0.3f) {
			Volume02 += (_DT / 6);
			SoundManager::GetInstance()->Set_ChannelVolume(CHANNELID::SOUND_BGM01, Volume02);
		}
		else { PlayingSound = 0; }
	}
}
StartScene* StartScene::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
    StartScene* LS = new StartScene(_GRPDEV);

    SceneManager::GetInstance()->Set_CurrentScene(LS);

    if (FAILED(LS->Ready_Scene())) {
        MSG_BOX("Cannot Create StartScene.");
        Safe_Release(LS);
        return nullptr;
    }
    return LS;
}
void StartScene::Free() {
    //if (pMiniGame != nullptr)   Safe_Release(pMiniGame);
    Scene::Free();
}