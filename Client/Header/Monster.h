#include"GameObject.h"
#include"SceneManager.h"
#include"MonsterManager.h"

#define FRAMETICK 0.1f

#pragma region Macros
#define POS(GAMEOBJECT)			static_cast<Transform*>(GAMEOBJECT->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position()
#define SCALE(GAMEOBJECT)		static_cast<Transform*>(GAMEOBJECT->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale()
#define ROTATION(GAMEOBJECT)	static_cast<Transform*>(GAMEOBJECT->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Rotation()

#define MYPOS					Component_Transform->Get_Position()
#define MYSCALE					Component_Transform->Get_Scale()
#define MYROTATION				Component_Transform->Get_Rotation()

#define COLLIDER(GAMEOBJECT)	static_cast<Collider*>(GAMEOBJECT->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER))
#pragma endregion

#pragma region MonsterSummon&MonsterDeath
#define MONSTER_SUMMON01_PLAYTIME	1.f
#define MONSTER_SUMMON02_PLAYTIME	1.1f
#define MONSTER_SUMMON03_PLAYTIME	1.f
#define MONSTER_DEATH_PLAYTTIME		1.f
#pragma endregion

// 1.	씬 시작시 배치된 오브젝트의 경우 Create 도중 이미지를 가져올 수 없음	( RESOURCEMANAGER 가 LAYER 배치보다 후순위)
// 2.	렌더링 리스트에 추가된 후 삭제되지 않도록 조건을 미리 걸어야함		( 오브젝트 삭제가 Update 이후, Late_Update 전에 이루어짐)

#pragma region Bat
#define BATIMGX						289
#define BATIMGY						223
#define BATIMG_ASPECTRATIO			((FLOAT)BATIMGY / (FLOAT)BATIMGX)

#define BAT_WIDTH					1.f
#define BAT_HEIGHT					BAT_WIDTH * BATIMG_ASPECTRATIO

#define BAT_SPEED					3.5f					//	이속
#define BAT_HP						30.f				//	체력

#define BAT_TRACKINGDIS				20.f				//	추적&이동	시작거리
#define BAT_TRACKINGMIN				10.f					//  이동			종료거리
#define	BAT_TRACKING_TIME			1.f					//	공격 쿨타임
#define BAT_LOST_TIME				1.f					//	추적 실패 시간

#define BAT_CASTING_TIME			1.f					//	선딜				CASTING		-> CHANNELING

#define BAT_CHANNELING_TIME			1.f					//	후딜		CHANNELING	-> IDLE
#define BAT_RUSHSPEED				4.f					//	돌진속도

#define BAT_BULLET_TYPE				Bullet_Standard		//	총알 종류
#define BAT_BULLET_SCALEMULT		1.f					//	총알 크기 배수
#define BAT_BULLET_NUM				5					//	총알갯수
#define BAT_BULLET_SPEEDMULT		1.f					//	총알 속도 배수

#define BAT_HORIZONTALFLIP_BUFFER	0.1f				//	좌우반전 버퍼
#pragma endregion

#pragma region Bullet_Standard
#define BULLET_STANDARD_IMGX					146
#define BULLET_STANDARD_IMGY					146
#define BULLET_STANDARD_IMG_ASPECTRATIO			((FLOAT)BULLET_STANDARD_IMGY / (FLOAT)BULLET_STANDARD_IMGX)

#define BULLET_STANDARD_WIDTH					0.5f	// 조정
#define BULLET_STANDARD_HEIGHT					BULLET_STANDARD_WIDTH * BULLET_STANDARD_IMG_ASPECTRATIO

#define	BULLET_STANDARD_SPEED	5.f
#pragma endregion

#pragma region 

#define SCORPIONEVILSOULIMGX					159
#define SCORPIONEVILSOULIMGY					254
#define SCORPIONEVILSOULIMG_ASPECTRATIO			((FLOAT)SCORPIONEVILSOULIMGY / (FLOAT)SCORPIONEVILSOULIMGX)

#define SCORPIONEVILSOUL_WIDTH					0.7f
#define SCORPIONEVILSOUL_HEIGHT					SCORPIONEVILSOUL_WIDTH * SCORPIONEVILSOULIMG_ASPECTRATIO

#define SCORPIONEVILSOUL_SPEED					2.f			//	이속
#define SCORPIONEVILSOUL_HP						100.f		//	체력

#define SCORPIONEVILSOUL_TRACKINGDIS			20.f		//	추적&이동	시작거리
#define SCORPIONEVILSOUL_TRACKINGMIN			10.f			//  이동			종료거리
#define SCORPIONEVILSOUL_TRACKING_TIME			1.f			//	공격 쿨타임
#define SCORPIONEVILSOUL_LOST_TIME				1.f			//	추격 실패 시간				TRACKING	-> IDLE

#define SCORPIONEVILSOUL_CASTING_TIME			1.5f		//	선딜				CASTING		-> CHANNELING
#define SCORPIONEVILSOUL_CHANNELING_TIME		0.5f		//	후딜

#define SCORPIONEVILSOUL_BULLET_TYPE			ScorpionBullet			//	총알 종류
#define SCORPIONEVILSOUL_BULLET_SPEEDMULT		1.f						//	총알 속도 배수
#define SCORPIONEVILSOUL_BULLET_SCALEMULT		2.f						//	총알 크기 배수

#define SCORPIONEVILSOUL_HORIZONTALFLIP_BUFFER	0.1f
#pragma endregion

#pragma region ScorpionBullet
#define SCORPIONBULLET_IMGX						120
#define SCORPIONBULLET_IMGY						120
#define SCORPIONBULLET_IMG_ASPECTRATIO			((FLOAT)SCORPIONBULLET_IMGY / (FLOAT)SCORPIONBULLET_IMGX)

#define SCORPIONBULLET_WIDTH					2.f
#define SCORPIONBULLET_HEIGHT					SCORPIONBULLET_WIDTH * SCORPIONBULLET_IMG_ASPECTRATIO

#define SCORPIONBULLET_SPEED					3.f
#define SCORPIONBULLET_CHAINBULLET_NUM			6
#pragma endregion

#pragma region EvilFrog
#define EVILFROGIMGX									126
#define EVILFROGIMGY									254
#define EVILFROGIMG_ASPECTRATIO						((FLOAT)EVILFROGIMGY / (FLOAT)EVILFROGIMGX)

#define EVILFROG_WIDTH									1.f
#define EVILFROG_HEIGHT								EVILFROG_WIDTH * EVILFROGIMG_ASPECTRATIO

#define EVILFROG_SPEED									4.f
#define EVILFROG_HP											20.f

#define EVILFROG_TRACKINGDIS							20.f
#define EVILFROG_TRACKINGMIN							10.f
#define EVILFROG_TRACKING_TIME 					3.f
#define EVILFROG_LOST_TIME							3.f

#define EVILFROG_CASTING_TIME						2.f
#define EVILFROG_CHANNELING_TIME						0.3f

#pragma region EvilFrogBullet
#define EVILFROG_Bullet_TYPE								Frog_Bullet
#define EVILFROG_BULLET_SCALEMULT						1.f
#define EVILFROG_BULLET_SPEEDMULT						1.f
//#define EVILFROG_BULLET_NUM									8

#define EVILFROG_BULLET_WIDTH								0.5f
#define EVILFROG_BULLET_HEIGHT							EVILFROG_WIDTH * EVILFROGIMG_ASPECTRATIO

#define EVILFROG_BULLET_SPEED                3.f	


#define BAT_HORIZONTALFLIP_BUFFER	0.1f				//	좌우반전 버퍼
#pragma endregion

#pragma region BULLET_CHAIN
#define BULLET_CHAIN_IMGX						120
#define BULLET_CHAIN_IMGY						120
#define BULLET_CHAIN_IMG_ASPECTRATIO			((FLOAT)BULLET_CHAIN_IMGY / (FLOAT)BULLET_CHAIN_IMGX)

#define BULLET_CHAIN_WIDTH						0.5f
#define BULLET_CHAIN_HEIGHT						BULLET_CHAIN_WIDTH * BULLET_CHAIN_IMG_ASPECTRATIO

#define BULLET_CHAIN_SPEED						3.f
#pragma endregion

#pragma region ShotGunEvilSoul
#define SHOTGUNEVILSOULIMGX						169
#define SHOTGUNEVILSOULIMGY						284
#define SHOTGUNEVILSOULIMG_ASPECTRATIO			((FLOAT	)SHOTGUNEVILSOULIMGY / (FLOAT)SHOTGUNEVILSOULIMGX)

#define SHOTGUNEVILSOUL_WIDTH					0.7f
#define SHOTGUNEVILSOUL_HEIGHT					SHOTGUNEVILSOUL_WIDTH * SHOTGUNEVILSOULIMG_ASPECTRATIO

#define SHOTGUNEVILSOUL_SPEED					1.f	
#define SHOTGUNEVILSOUL_HP						100.f

#define SHOTGUNEVILSOUL_TRACKINGDIS				20.f
#define SHOTGUNEVILSOUL_TRACKINGMIN				10.f
#define SHOTGUNEVILSOUL_TRACKING_TIME			1.f	
#define SHOTGUNEVILSOUL_LOST_TIME				1.f	

#define SHOTGUNEVILSOUL_CASTING_TIME			0.5f			//	½ÃÀü ½Ã°£(¼±µô)				CASTING		-> CHANNELING

#define SHOTGUNEVILSOUL_CHANNELING_TIME			1.f		//	°ø°Ý ÆÐÅÏ À¯Áö ½Ã°£(ÈÄµô)		CHANNELING	-> IDLE
#define SHOTGUNEVILSOUL_BULLET_TYPE				Fireball
#define SHOTGUNEVILSOUL_BULLET_SCALEMULT		1.f
#define SHOTGUNEVILSOUL_BULLET_SPEEDMULT		1.f
#define SHOTGUNEVILSOUL_BULLET_NUM				6			//	ÃÑ¾Ë °¹¼ö
#define SHOTGUNEVILSOUL_SPREAD					60.f		//	ºÐ»ê ¹üÀ§

#define SHOTGUNEVILSOUL_HORIZONTALFLIP_BUFFER	0.1f		//	ÀÌ¹ÌÁö ÁÂ¿ì ¹ÝÀü ¹öÆÛ
#pragma endregion

#pragma region Fireball
#define FIREBALLIMGX						479
#define FIREBALLIMGY						190
#define FIREBALLIMG_ASPECTRATIO				((FLOAT)FIREBALLIMGY / (FLOAT)FIREBALLIMGX)

#define FIREBALL_WIDTH						1.f
#define FIREBALL_HEIGHT						FIREBALL_WIDTH * FIREBALLIMG_ASPECTRATIO

#define FIREBALL_SPEED						5.f
#pragma endregion

#pragma region EvilSlime
#define EVILSLIMEIMGX						256
#define EVILSLIMEIMGY						207
#define EVILSLIMEIMG_ASPECTRATIO			((FLOAT)EVILSLIMEIMGY / (FLOAT)EVILSLIMEIMGX)

#define EVILSLIME_WIDTH						1.5f
#define EVILSLIME_HEIGHT					(EVILSLIME_WIDTH * EVILSLIMEIMG_ASPECTRATIO)

#define EVILSLIME_SPEED						2.f
#define EVILSLIME_HP						100.f

#define EVILSLIME_TRACKINGDIS				10.f
#define EVILSLIME_TRACKING_TIME				1.f	
#define EVILSLIME_LOST_TIME					1.f	

#define EVILSLIME_CASTING_TIME				2.f	
#define EVILSIME_JUMP_HEIGHT				10.f

#define EVILSLIME_CHANNELING_TIME			0.3f
#define EVILSLIME_BULLET_TYPE				EvilSlimeGroundIce
#define EVILSLIME_BULLET_NUM				9			//	MAX : 9
#define EVILSLIME_BULLET_SCALEMULT			1.f

#pragma endregion

#pragma region EVILSLIMEGROUNDICE
#define EVILSLIMEGROUNDICE_IMGX						411
#define EVILSLIMEGROUNDICE_IMGY						290
#define EVILSLIMEGROUNDICE_IMG_ASPECTRATIO			(FLOAT)EVILSLIMEGROUNDICE_IMGY / (FLOAT)EVILSLIMEGROUNDICE_IMGX

#define EVILSLIMEGROUNDICE_WIDTH					1.f
#define EVILSLIMEGROUNDICE_HEIGHT					EVILSLIMEGROUNDICE_WIDTH * EVILSLIMEGROUNDICE_IMG_ASPECTRATIO

#pragma endregion

#pragma region Docheol
//#define DOCHEOLAPPEARMARK_FLOATDIS		0.5f
//#define DOCHEOLAPPEARMARK_FLOATTIME		1.f
//#define DOCHEOL_COOLDOWN				1.f
//#define DOCHEOL_PUNCH_FIREBALL_NUM		5
//#define DOCHEOL_PUNCH_FIREBALL_ANGLE	60
//#define DOCHEOL_METEOR_NUM				5
//
#pragma endregion

enum MONSTER_STATE
{
	MONSTER_STATE_SUMMON,
	MONSTER_STATE_APPEAR,
	MONSTER_STATE_IDLE,
	MONSTER_STATE_TRACKING,
	MONSTER_STATE_CASTING,
	MONSTER_STATE_CHANNELING,
	MONSTER_STATE_HIT,
	MONSTER_STATE_DISAPPEAR,
	MONSTER_STATE_DEAD,

	EVILSLIME_FISSION,
	MONSTER_STATE_MINIGAME_MOVE,
	MONSTER_STATE_MINIGAME_IDLE,

	MONSTER_STATE_END
};


typedef struct tagTextureInfo
{
	tagTextureInfo() :_frame(0), _Endframe(0), _frameTick(0.f) { _vecTexture.reserve(32); }
	~tagTextureInfo() { _vecTexture.clear(); }

	vector<IDirect3DTexture9*>			_vecTexture;
	const	vector<IDirect3DTexture9*>*	pTexture;
	TCHAR						_Filename[256];
	_uint						_frame;
	_uint						_Endframe;
	_float						_frameTick;

}TEXINFO;

typedef struct tagMonsterInfo {
	tagMonsterInfo() :
		bTrigger{}, eState{}, fTimer{}, pGameObj{}, bMiniGame(false),_pHurdle(nullptr),
		vDirection{-1.f,0.f,-1.f}, fSpeed(0.f){} 

	~tagMonsterInfo() {}

	VOID	Change_State(MONSTER_STATE _eState) 
	{
		eState[1]			= eState[0];
		eState[0]			= _eState;
		fSpeed				= 0.f;
		memset(fTimer, 0, sizeof(fTimer));

	}

	BOOL						bTrigger[5];

	uint16_t					ID = 0x0000;

	MONSTER_STATE				eState[2];
	_float						fTimer[4];
	GameObject*					pGameObj[10];

	_vec3						vDirection;
	_float						fSpeed;

	TEXINFO						Textureinfo;
	BOOL						bMiniGame;

	tagHurdleInfo*				_pHurdle;
}MONSTERINFO, MONINFO, MONBULLETINFO;

typedef struct tagRandomGenerator {
	static inline uint64_t Seed[2] = { (uint64_t)time(NULL), 0x9e3779b97f4a7c15 };

	static inline uint64_t Xorshift128p(void* _Seed = nullptr)
	{
	uint64_t x = Seed[0];
	const uint64_t y = (_Seed == nullptr) ? Seed[1] : reinterpret_cast<uintptr_t>(_Seed) ^ Seed[1];
	Seed[0] ^= y;
	x ^= x << 23;
	Seed[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
	return Seed[1] + y;
	}

	static _float Get_float(_float _Dst, _float _Src, void* _Seed = nullptr)	// A 이상 B 이하 (숫자 순서 상관없음)
	{
		if (_Dst == _Src) return _Dst;

		return (_Dst < _Src) ?
			_Dst + ((Xorshift128p(_Seed) % 1001) * 0.001f) * (_Src - _Dst)
			: _Src + ((Xorshift128p(_Seed) % 1001) * 0.001f) * (_Dst - _Src);
	}

	static int Get_int(int _Dst, int _Src, void* _Seed = nullptr)				// A 이상 B 이하  (숫자 순서 상관없음)
	{
		if (_Dst == _Src) return _Dst;

		return (_Dst < _Src) ?
			_Dst + (int)(Xorshift128p(_Seed) % (_Src - _Dst + 1))
			: _Src + (int)(Xorshift128p(_Seed) % (_Dst - _Src + 1));
	}
}RANDOM;

//class SceneManager;

class Monster
{
public:
	static	GameObject* Set_Target(CONST TCHAR* _TAG, GameObject*& GameObj);
	static	GameObject* Set_Target(CONST TCHAR* _TAG);

public:
	static	HRESULT			Set_TextureList(uint16_t _Key, TEXINFO* _TexInfo);
	static	HRESULT			Set_TextureList(CONST TCHAR* __FileName, TEXINFO* __Textures );
	static	HRESULT			Set_TextureList(CONST TCHAR* __FileName, MONINFO* _MonsterInfo);
	static	FLOAT			BillBoard(Transform* TransCom, LPDIRECT3DDEVICE9 _GRPDEV, _vec3 vDir = { 1.f, 0.f,0.f }, BOOL OffSet = true);
	static	HRESULT			Flip_Horizontal(Transform* TransCom, _vec3* pDir, _float Buffer);
	static	VOID			BillBoard_Standard(LPDIRECT3DDEVICE9 GRPDEV, Transform* Component_Transform);
	static	VOID			Destory_Tile(GameObject* pObj);
	
public:
	static	HRESULT			Minigame_Update(const _float& _DT, MONINFO* _pInfo, _vec3* vPos);
	static	BOOL			Minigame_LateUpdate(const _float& _DT, MONINFO* _pInfo);
	static	HRESULT			Staic_Obj(LPDIRECT3DDEVICE9 _GRPDEV, Transform* Transcom);

public:
	static	void		    Set_Camera(CameraObject* pCam)	{ m_pCam = pCam; }
	static	CameraObject*	Get_Camera()					{ return m_pCam; }
	static	void            Set_Player(Player* pPlayer)		{ m_pPlayer = pPlayer; }
	static	Player*			Get_Player()					{ return m_pPlayer; }

private:
	static	CameraObject*	m_pCam;
	static	Player*			m_pPlayer;

public:
	static VOID Add_Monster_to_Scene(GameObject* pMonster,wstring _TAG ,GAMEOBJECT_TYPE eType = GAMEOBJECT_TYPE::OBJECT_END);					// push GameObject ptr to LAYER_DYNAMIC_OBJECT & CollisionMgr
	static void Release_Hurdle(MONSTERINFO* _Info);


	template<typename T>
	static	GameObject* Create(LPDIRECT3DDEVICE9 _GRPDEV)
	{
		GameObject* MST = DBG_NEWW T(_GRPDEV);
		if (FAILED(MST->Ready_GameObject())) {
			string ErrorMsg = string("Cannot Create ") + typeid(T).name();
			//MSG_BOX(ErrorMsg.c_str());
			Safe_Release(MST);
			return nullptr;
		}
		return MST;
	}

	template<typename T>
	static	GameObject* Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 _vPos, _float _fScalemult = 1.f)
	{
		GameObject* MST = Create<T>(_GRPDEV);

		Transform* pTransCom = static_cast<Transform*>(MST->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		pTransCom->Set_Pos(_vPos);

		_vec2	vScale = { pTransCom->Get_Scale()->x,pTransCom->Get_Scale()->y};

		D3DXVec2Normalize(&vScale, &vScale);
		vScale *= _fScalemult;

		pTransCom->Set_Scale({ vScale.x, vScale.y, _fScalemult });
		
		Collider* pCollider = static_cast<Collider*>(MST->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER));
		if (nullptr != pCollider)	pCollider->Set_Scale(vScale.x * 0.5f, vScale.y, vScale.x * 0.5f);

		return MST;
	}

	template<typename T>
	static GameObject* Create(LPDIRECT3DDEVICE9 GRPDEV, _vec3 _vSrc, _vec3 _vDst, _float _fSpeed, _float _fScalemult = 1.f)
	{
		vector<tagHurdleInfo*>* pContainer = MonsterManager::GetInstance()->Get_Hurdles();
		tagHurdleInfo* pHurdle = nullptr;

		for (auto& it : *pContainer) {
			if (it->vSrc == _vSrc && 
				it->vDst == _vDst && 
				it->fSpeed == _fSpeed 
				&& it->fScale == _fScalemult) 
			{
				pHurdle = it;
				break;
			}
		}

		if (nullptr == pHurdle) {
			pHurdle = DBG_NEWW tagHurdleInfo(_vSrc, _vDst, _fSpeed, _fScalemult);
			pContainer->push_back(pHurdle);
		}

		if ((roundf(pHurdle->fDis / pHurdle->fScale) - pHurdle->fDis) > pHurdle->RefCount)
			return nullptr;

		GameObject* pObj = Monster::Create<T>(GRPDEV, (pHurdle->vDir * (float)pHurdle->RefCount) + pHurdle->vSrc, _fScalemult);
		if (nullptr == pObj) return nullptr;

		pHurdle->RefCount++;

		T* pCastObj = static_cast<T*>(pObj);
		MONSTERINFO* pInfo = pCastObj->Get_Info();
		if (pInfo) {
			pInfo->Change_State(MONSTER_STATE_MINIGAME_IDLE);
			pInfo->_pHurdle = pHurdle;
			pInfo->vDirection = pHurdle->vDir;
		}

		return pObj;
	}
};
