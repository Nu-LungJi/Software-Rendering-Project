#include "../Include/PCH.h"

Spawner::Spawner(LPDIRECT3DDEVICE9 _GRPDEV) :m_SpawnCnt(1), m_SpawnDelay(3.f), GameObject(_GRPDEV), m_fDefense(0.f), m_bTrigger(false), m_bSpawn(false), m_fTime(0), m_fFrame(0), m_bStopFrame(false), m_pBuffer(nullptr), m_pTransform(nullptr), m_pTileInfo(nullptr) {}
Spawner::Spawner(const GameObject& _RHS) : GameObject(_RHS) {}
Spawner::~Spawner() {}

HRESULT Spawner::Ready_GameObject(TILE_SIDE eid, TILE_SPAWNER eSpawn, _vec3 vPos) {

	if (FAILED(Component_Initialize(eid, eSpawn))) return E_FAIL;

	switch (eSpawn)
	{
	  case TILE_SPAWNER::CL_SPAWN:
		m_bSpawn = true;
		break;
	  case TILE_SPAWNER::RANDOM_SPAWNER:
		  
		  for (int i = 0; i < 3; ++i)
		  {
			  if (TileManager::GetInstance()->Get_Defense().size() > 2000)
				  break;
			  _int iRand = rand() % 5;
			  GameObject* pObj=nullptr;
			  switch (iRand)
			  {
			  case 0:

				  pObj = Bat::Create(GRPDEV, vPos, true);
				  break;
			  case 1:
				  pObj = ScorpionEvilSoul::Create(GRPDEV, vPos, true);
				  pObj = ScorpionEvilSoul::Create(GRPDEV, vPos, true);
				  break;
			  case 2:
				  pObj = ShotGunEvilSoul::Create(GRPDEV, vPos, true);
				  break;
			  case 3:
				  pObj = EvilSlime::Create(GRPDEV, vPos, true);
				  break;
			  }
			  if(pObj != nullptr)
			  TileManager::GetInstance()->Get_Defense().push_back(pObj);
			 
		  }
		  
		break;
	}
	
	
	return S_OK;
}
INT	Spawner::Update_GameObject(const _float& _DT) {

	GameObject::Update_GameObject(_DT);
	
	
	Frame_Move(_DT);
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_TILE, this);

	return 0;

}
VOID Spawner::LateUpdate_GameObject(const _float& _DT) {

	_vec3		vPos;
	m_pTransform->Get_Info(INFO_POS, &vPos);
	for (size_t i = 0; i < m_vecMonsterDefense.size(); ++i)
	{
		m_vecMonsterDefense[i]->LateUpdate_GameObject(_DT);
	}
	
	if (m_pTileInfo->Get_Spawner() == TILE_SPAWNER::RANDOM_SPAWNER)
	{
		MiniGameCounter* pObj = dynamic_cast<MiniGameCounter*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"DefenseUI"));
		if (pObj == nullptr)
			return;
		_int i = pObj->Get_Stage();

		switch (i)
		{
		case 1:
			m_SpawnCnt = 2;
			m_SpawnDelay = 1;

			break;
		case 2:
			m_SpawnCnt = 3;
			m_SpawnDelay = 0.2;

			break;
		}
	}
	
	AlphaYSorting(&vPos);

	GameObject::LateUpdate_GameObject(_DT);

}
VOID Spawner::Render_GameObject()
{
	GRPDEV->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());

	GRPDEV->SetTexture(0, m_pTileInfo->Get_Texture());

	m_pBuffer->Render_Buffer();

	GRPDEV->SetTexture(0, NULL);
}

void Spawner::Set_Buffer(TILE_SIDE eid)
{
	switch (eid)
	{
	case TILE_SIDE::TILE_FRONT:
		m_pBuffer = ADD_COMPONENT_TILEFRONT;
		break;
	case TILE_SIDE::TILE_RIGHT:
		m_pBuffer = ADD_COMPONENT_TILERIGHT;
		break;
	case TILE_SIDE::TILE_LEFT:
		m_pBuffer = ADD_COMPONENT_TILELEFT;
		break;
	case TILE_SIDE::TILE_BACK:
		m_pBuffer = ADD_COMPONENT_TILEBACK;
		break;
	case TILE_SIDE::TILE_OTHER:
		m_pBuffer = ADD_COMPONENT_TILE;
		break;
	}
}

void Spawner::Frame_Move(const FLOAT& _DT)
{


	switch (m_pTileInfo->Get_Spawner())
	{
	case TILE_SPAWNER::NPC1:
		if (TileManager::GetInstance()->Get_CurrentStage() == TILE_STAGE::TILE_STAGE4)
		{
			if (!m_bSpawn)
			{
				ShopKeeper* pObj = ShopKeeper::Create(GRPDEV, *m_pTransform->Get_Position());
				pObj->Set_ObjectTag(L"ShopNPC");
				SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pObj);
				m_bSpawn = true;
			}
		}
		break;

	case TILE_SPAWNER::NPC2:
		break;
	
	case TILE_SPAWNER::MONSTER_SPAWN1:
		Monster_Spawn();
		break;
	case TILE_SPAWNER::MONSTER_SPAWN2:
		Monster_Spawn2();
		break;
	case TILE_SPAWNER::MONSTER_SPAWN3:
		Monster_Spawn3();
		break;
	case TILE_SPAWNER::MONSTER_SPAWN4:
		Monster_Spawn4();
		break;
	case TILE_SPAWNER::ITEM_SPAWN1:
	
		break;
	case TILE_SPAWNER::ITEM_SPAWN2:
		break;

	case TILE_SPAWNER::ITEM_SPAWN3:
		break;

	case TILE_SPAWNER::ITEM_SPAWN4:
		break;

	case TILE_SPAWNER::ITEM_SPAWN5:
		break;

	case TILE_SPAWNER::ITEM_SPAWN6:
		break;
	case TILE_SPAWNER::CL_SPAWN:
	    CL_Spawn();
		break;
	case TILE_SPAWNER::BOSS_SPAWN:
		Boss();
		break;
	case TILE_SPAWNER::RANDOM_SPAWNER:
		Defense_Spawn(_DT);
		break;
	case TILE_SPAWNER::UI_SPAWNER:
		if (!m_bSpawn)
		{
			MiniGameCounter* pObj = MiniGameCounter::Create(GRPDEV);
			pObj->Set_ObjectTag(L"DefenseUI");
			SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_USER_INTERFACE)->Add_GameObject(pObj);
			m_bSpawn = true;
		}
		break;
	}

}

void Spawner::Monster_Spawn()
{
	if (!m_bSpawn)
	{
		_vec3 vPos;
		m_pTransform->Get_Info(INFO_POS, &vPos);
		Monster::Add_Monster_to_Scene(Monster::Create<Bat>(GRPDEV, vPos),L"Monster",GAMEOBJECT_TYPE::OBJECT_MONSTER);
		m_bSpawn = true;
	}
}
void Spawner::Monster_Spawn2()
{
	if (!m_bSpawn)
	{
		_vec3 vPos;
		m_pTransform->Get_Info(INFO_POS, &vPos);

		Monster::Add_Monster_to_Scene(Monster::Create<ScorpionEvilSoul>(GRPDEV, vPos, 2.f), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
		m_bSpawn = true;
	}
}
void Spawner::Monster_Spawn3()
{
	if (!m_bSpawn)
	{
		_vec3 vPos;
		m_pTransform->Get_Info(INFO_POS, &vPos);
		Monster::Add_Monster_to_Scene(Monster::Create<ShotGunEvilSoul>(GRPDEV,vPos), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
		m_bSpawn = true;
	}
}
void Spawner::Monster_Spawn4()
{
	if (!m_bSpawn)
	{
		_vec3 vPos;
		m_pTransform->Get_Info(INFO_POS, &vPos);
	  Monster::Add_Monster_to_Scene(Monster::Create<EvilSlime>(GRPDEV, vPos), L"Monster", GAMEOBJECT_TYPE::OBJECT_MONSTER);
		m_bSpawn = true;
	}
}

void Spawner::CL_Spawn()
{
	_vec3 vPos;
	vPos = *m_pTransform->Get_Position();
	
	if (!m_bTrigger)
	{
		dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"))->Set_CameraMove(true);
		dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->CheonLog_Respawn(0);
		dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->Set_Obj(this, vPos);
		dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->Set_Move(false);
		m_bTrigger = true;
	}
	
	if (!m_bSpawn)
	{
		Cheonlog* pCL = Cheonlog::Create(GRPDEV, vPos);
		pCL->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER);
		pCL->Set_ObjectTag(L"CheonLog");
		SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pCL);
		m_bSpawn = true;
	}
}
void Spawner::Defense_Spawn(const _float& _DT)
{
	m_fDefense += _DT;

	if (dynamic_cast<MiniGameCounter*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"DefenseUI")) != nullptr)
	{
		if (dynamic_cast<MiniGameCounter*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"DefenseUI"))->Get_End())
		{
			for (auto& iter : m_vecMonsterDefense)
			{
				CollisionManager::GetInstance()->Delete_ColliderObject(iter);
				Safe_Release(iter);
			}
				
			m_vecMonsterDefense.clear();

			for (auto& iterer : TileManager::GetInstance()->Get_Defense())
			{

				CollisionManager::GetInstance()->Delete_ColliderObject(iterer);

				Safe_Release(iterer);
			}


			return;
		}
	}
	
	if (m_fDefense > m_SpawnDelay)
	{
		m_fDefense = 0;
		if (m_vecMonsterDefense.size() < m_SpawnCnt)
		{
			if (TileManager::GetInstance()->Get_Defense().size() == 0)
				return;

			m_vecMonsterDefense.push_back(TileManager::GetInstance()->Get_Defense()[TileManager::GetInstance()->Get_Defense().size() - 1]);
			CollisionManager::GetInstance()->Add_ColliderObject(TileManager::GetInstance()->Get_Defense()[TileManager::GetInstance()->Get_Defense().size() - 1]);
			TileManager::GetInstance()->Get_Defense().pop_back();
		}
	}

	for (auto iter = m_vecMonsterDefense.begin(); iter != m_vecMonsterDefense.end();)
	{

		(*iter)->Update_GameObject(_DT);
		_int iResult = dynamic_cast<Collider*>((*iter)->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER))->Get_Hp();
		if (iResult <= 0)
		{
			dynamic_cast<Transform*>((*iter)->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_Pos(*m_pTransform->Get_Position());
			dynamic_cast<Collider*>(((*iter)->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER)))->Set_Hp(50);
			dynamic_cast<MiniGameCounter*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"DefenseUI"))->Set_Count();
			CollisionManager::GetInstance()->Delete_ColliderObject((*iter));
			TileManager::GetInstance()->Get_Defense().push_back(*iter);
			iter = m_vecMonsterDefense.erase(iter);
			continue;

		}

		if (iter != m_vecMonsterDefense.end())
			++iter;
	}


	
}

Transform* Spawner::Crash_Player()
{
	_vec3 vPos{}, vTilePos{};
	Transform* pPlayer = dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	Player* OriginPlayer = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));

	pPlayer->Get_Info(INFO_POS, &vPos);

	m_pTransform->Get_Info(INFO_POS, &vTilePos);

	if (vPos.x > vTilePos.x - 1 && vPos.x < vTilePos.x + 1 && vPos.z > vTilePos.z - 1 && vPos.z < vTilePos.z + 1 && vTilePos.y < 2)
	{
		return pPlayer;
	}


	return nullptr;
}

void Spawner::Boss()
{
	if (!m_bSpawn)
	{
		_vec3 vPos;
		SceneManager::GetInstance()->Get_CurrentScene()->Add_GameObjectToScene<FinalBoss>(LAYER_TYPE::LAYER_DYNAMIC_OBJECT, GAMEOBJECT_TYPE::OBJECT_FINALBOSS, L"Docheol");
		GameObject* Docheol = SceneManager::GetInstance()->Get_GameObject(L"Docheol");
		CollisionManager::GetInstance()->Add_ColliderObject(Docheol);

		vPos = *m_pTransform->Get_Position();
		Transform* DCTransform = dynamic_cast<Transform*>(Docheol->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		DCTransform->Set_Pos(DCTransform->Get_Position()->x + vPos.x, DCTransform->Get_Position()->y + vPos.y, DCTransform->Get_Position()->z + vPos.z);
		m_bSpawn = true;
	}
}

HRESULT Spawner::Component_Initialize(TILE_SIDE eid, TILE_SPAWNER eSpawn) {

	m_pTransform = ADD_COMPONENT_TRANSFORM;
	m_pTileInfo = ADD_COMPONENT_TILEINFO;

	switch (eid)
	{
	case TILE_SIDE::TILE_FRONT:
		m_pBuffer = ADD_COMPONENT_TILEFRONT;
		break;
	case TILE_SIDE::TILE_RIGHT:
		m_pBuffer = ADD_COMPONENT_TILERIGHT;
		break;
	case TILE_SIDE::TILE_LEFT:
		m_pBuffer = ADD_COMPONENT_TILELEFT;
		break;
	case TILE_SIDE::TILE_OTHER:
		m_pBuffer = ADD_COMPONENT_TILE;
		break;
	}

	return S_OK;
}

Spawner* Spawner::Create(LPDIRECT3DDEVICE9 _GRPDEV, TILE_SIDE eid, TILE_SPAWNER eSpawn, _vec3 vPos) {

	Spawner* pSpawner = new Spawner(_GRPDEV);

	if (FAILED(pSpawner->Ready_GameObject(eid, eSpawn, vPos))) {
		MSG_BOX("Cannot Create Spawner.");
		Safe_Release(pSpawner);
		return nullptr;
	}

	return pSpawner;
}
VOID Spawner::Free() {

	for (auto& iter : m_vecMonsterDefense)
		Safe_Release(iter);

	m_vecMonsterDefense.clear();

	GameObject::Free();
}