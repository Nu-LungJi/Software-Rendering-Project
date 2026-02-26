#include "../Include/pch.h"
#include "StartScene.h"

StartScene::StartScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV) {}
StartScene::~StartScene() {}

HRESULT   StartScene::Ready_Scene() {
    Scene::Ready_Scene();
    ProtoManager::GetInstance()->Ready_Prototype(GRPDEV);
    UIManager::GetInstance()->Ready_UIManager(GRPDEV);
    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Resource");
    if (FAILED(Ready_Enviroment_Layer()))      return E_FAIL;
    if (FAILED(Ready_GameLogic_Layer()))      return E_FAIL;
    if (FAILED(Ready_UserInterface_Layer()))      return E_FAIL;
    ResourceManager::GetInstance()->GlobalImport_Texture(GRPDEV, L"../../Tile");
    //Load Tile
    {
        HANDLE   hFile = CreateFile(L"../../Data/Cheonglock.dat", // ?뚯씪 ?대쫫???ы븿??寃쎈줈
            GENERIC_READ,      // ?뚯씪 ?묎렐 紐⑤뱶(GENERIC_WRITE : ?곌린, GENERIC_READ : ?쎄린)
            NULL,            // 怨듭쑀 諛⑹떇(?뚯씪???대젮 ?덈뒗 ?곹깭?먯꽌 ?ㅻⅨ ?꾨줈?몄뒪媛 ?ㅽ뵂 ?????덇??섎뒗 寃껋뿉 ????ㅼ젙, 吏?뺥븯吏 ?딆쓣 寃쎌슦 NULL)
            NULL,            // 蹂댁븞 ?띿꽦(湲곕낯媛믪씤 寃쎌슦 NULL)
            OPEN_EXISTING,      // ?뚯씪???놁쓣 寃쎌슦 ?뚯씪???앹꽦?섏뿬 ???OPEN_EXISTING : ?뚯씪???덉쓣 寃쎌슦?먮쭔 濡쒕뱶)
            FILE_ATTRIBUTE_NORMAL,   // ?뚯씪 ?띿꽦(?꾨Т???띿꽦???녿뒗 ?쇰컲 ?뚯씪)
            NULL);            // ?앹꽦???뚯씪???띿꽦?뉖Ⅴ ?쒓났???쒗뵆由??뚯씪

        if (hFile == INVALID_HANDLE_VALUE)
        {
            MSG_BOX("濡쒕뱶 ?ㅽ뙣");
            return E_FAIL;
        }

        DWORD   dwByte(0);      // eof ??븷
        _int             iTilenum = 0;
        TILE_SIDE        eTileSide = TILE_SIDE::TILE_END;
        TILE_STATE       eTileState = TILE_STATE::STATE_END;
        TILEMODE_CHANGE  eTileMode = TILEMODE_CHANGE::MODE_END;
        TILE_STAGE        eTileStage = TILE_STAGE::STAGE_END;
        TILE_STAGE       eNext = TILE_STAGE::STAGE_END;
        _tchar          cTileName[128] = {};
        _vec3           Info = {};
        _vec3          Scale = {};
        _vec3          Rotation = {};
        _int           iTileTextureCnt = 0;
        _vec3          vNextPos = {};
        _bool           bAni = false;
        _int      i = 0;
        TILE_SPAWNER      eSpawn = TILE_SPAWNER::SPAWN_END;
        //TileManager::GetInstance()->Render_TileList();
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
            ReadFile(hFile, &eNext, sizeof(TILE_SPAWNER), &dwByte, NULL);

            if (0 == dwByte)
                break;

            GameObject* GOBJ = nullptr;
            //GRPDEV->AddRef();
            if (eTileState == TILE_STATE::STATE_NORMAL && eSpawn != TILE_SPAWNER::SPAWN_END)
            {
                GOBJ = Spawner::Create(GRPDEV, eTileSide, eSpawn);
            }
            else
                GOBJ = CXZTile::Create(GRPDEV, eTileSide, eTileState);



            if (eNext == TILE_STAGE::TILE_STAGE2)
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
        TileManager::GetInstance()->Set_StageCnt();
        MSG_BOX("로드 성공");
        CloseHandle(hFile);
    }
    KeyManager::GetInstance()->Ready_KeyManager(hInst, hWnd);
    CollisionManager::GetInstance()->Get_AllObjectOfScene();
    pLoading = CLoading::Create(GRPDEV, CLoading::LOADING_STAGE);

    return S_OK;
}
INT    StartScene::Update_Scene(CONST FLOAT& _DT) {

    if (KeyManager::GetInstance()->KEY_STATE_DOWN(DIK_TAB) == TRUE) {
        int  a = 0;
    }
    TileManager::GetInstance()->Stage_Update(_DT);

    if (!TileManager::GetInstance()->Get_Loading())
    {
        if (pLoading->Get_Finish())
        {
            TileManager::GetInstance()->Set_Stage();
            TileManager::GetInstance()->Set_EndLoading(TRUE);

        }
    }
    else
        TileManager::GetInstance()->Stage_Update(_DT);
    CollisionManager::GetInstance()->Update_CollisionManager();
    return Scene::Update_Scene(_DT);
}
VOID StartScene::LateUpdate_Scene(CONST FLOAT& _DT) {

    TileManager::GetInstance()->Stage_LateUpdate(_DT);
    Scene::LateUpdate_Scene(_DT);
    CollisionManager::GetInstance()->LateUpdate_CollisionManager();
    CollisionManager::GetInstance()->Render_CollisionManager();

}
VOID StartScene::Render_Scene() {

    //GOBJ = Tile::Create(GRPDEV);
    //GOBJ->Set_ObjectTag(L"Tile");
    //
    //if (nullptr == GOBJ)               return E_FAIL;
    //if (FAILED(LYR->Add_GameObject(GOBJ)))   return E_FAIL;
}
HRESULT StartScene::Ready_Enviroment_Layer() {
    Add_GameObjectToScene<Terrain>(LAYER_TYPE::LAYER_STATIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_TERRAIN, L"Terrain");
    return S_OK;
}
HRESULT StartScene::Ready_GameLogic_Layer() {
    Add_GameObjectToScene<CameraObject>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_CAMERA, L"Camera");
    Add_GameObjectToScene<Player>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_PLAYER, L"Player");
    Add_GameObjectToScene<Bat>              (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"Bat");
    Add_GameObjectToScene<ScorpionEvilSoul>   (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"ScorpoinEvilSoul");
   // Add_GameObjectToScene<Bat>            (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"Bat");
    //Add_GameObjectToScene<ScorpoinEvilSoul>   (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"ScorpoinEvilSoul");
    Add_GameObjectToScene<EvilFrog>           (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"EvilFrog");


    //Add_GameObjectToScene<FinalBoss>         (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"Docheol");
    //Add_GameObjectToScene<Fireball>         (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_MONSTER, L"Fireball");
    //Add_GameObjectToScene<NPC>            (LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_NPC     , L"NPC_Tif"      );
    return S_OK;
}
HRESULT StartScene::Ready_UserInterface_Layer() {
    //Add_GameObjectToScene<MainMenuButton>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"MainButton");
    //Add_GameObjectToScene<MainMenu>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"MainMenu");
    Add_GameObjectToScene<MainUI>         (LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI     , L"MainUI"      );

    //Add_GameObjectToScene<PlayerInven>      (LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI     , L"PlayerInven"   );
    Add_GameObjectToScene<Augment>         (LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI     , L"Augment"      );    
    //Add_GameObjectToScene<NPCTalk>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"NPCTalk");
    //Add_GameObjectToScene<SpeechBubble>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"NpcField");
    //Add_GameObjectToScene<PlayerInven>(LAYER_TYPE::LAYER_USER_INTERFACE, GAMEOBJECT_TYPE::OBJECT_UI, L"Player_Inven");

    return S_OK;
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
    Safe_Release(pLoading);
    Scene::Free();
}