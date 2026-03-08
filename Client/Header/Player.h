#pragma once
#include "GameObject.h"
#include "Bow.h"
#include "Artifact.h"

enum class skillState {
	STATE_TIMESLOW,
	STATE_ATOMIC,
	NONE
};
enum class pState
{
	STATE_IDLE,
	STATE_DASH,
	STATE_ATTACK,
	STATE_LANDING,
	STATE_DEATH,
	STATE_START,
	//KJJ 03 06
	STATE_ON_EVENT,

	End
};
enum class eState
{
	STATE_STANDING,
	STATE_RUN_UP,
	STATE_RUN_DOWN,
	STATE_RUN_LEFT,
	STATE_RUN_RIGHT,
	STATE_RUN_LU,
	STATE_RUN_RU,
	STATE_RUN_LD,
	STATE_RUN_RD,
	STATE_DASH,
	STATE_HANDUP,
	STATE_LANDING_DOWN,
	STATE_LANDING_UP,

	STATE_ATTACK_LU,
	STATE_ATTACK_RU,
	STATE_ATTACK_LD,
	STATE_ATTACK_RD,

	STATE_ATTACK_RUN_LU,
	STATE_ATTACK_RUN_RU,
	STATE_ATTACK_RUN_LD,
	STATE_ATTACK_RUN_RD,

	STATE_ATTACK_RUN_BACK_LU,
	STATE_ATTACK_RUN_BACK_RU,
	STATE_ATTACK_RUN_BACK_LD,
	STATE_ATTACK_RUN_BACK_RD,

	STATE_DASH_LEFT,
	STATE_DASH_RIGHT,
	STATE_DASH_UP,
	STATE_DASH_DOWN,
	STATE_DASH_LU,
	STATE_DASH_RU,
	STATE_DASH_LD,
	STATE_DASH_RD,
	
	STATE_SLIDE,
	STATE_VICTORY,
	STATE_DEAD,
	STATE_LAND,

	End
};
enum class pSee
{
	SEE_UP,
	SEE_DOWN,
	SEE_RIGHT,
	SEE_LEFT,
	SEE_LU,
	SEE_RU,
	SEE_LD,
	SEE_RD,

	End
};
enum class mousePos
{
	MOUSE_LT,
	MOUSE_RT,
	MOUSE_LD,
	MOUSE_RD,

	End
};

class Player : public GameObject {
private:
	explicit Player(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Player(const GameObject& _RHS);
	virtual ~Player();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();
	void			Reset();
private:
	Buffer*			Component_Buffer;
	Transform*		Component_Transform;
	Texture*		Component_Texture;
	Collider*		Component_Collider;
public:
	BowType			Get_Weapon_Type()	{ return _weaponSlot[_equipNum]->Get_Bow_Type(); }	// 현재 장착한 활 타입
	BowStat*		Get_CurBow_Stat()	{ return _weaponSlot[_equipNum]->Get_Bow_Stat(); }	// 현재 장착한 활 스텟

	BOOL	Get_PlayerStop() { return _isStop; }
	void			Set_PlayerStop(bool isStop) {
		_isStop = isStop; 
		if (_isStop)
			_weaponSlot[_equipNum]->Set_Bow_Equip(false);
		else 
			_weaponSlot[_equipNum]->Set_Bow_Equip(true);

		if((_pState == pState::STATE_DASH || _pState == pState::STATE_DEATH))
			_weaponSlot[_equipNum]->Set_Bow_Equip(false);
	}

	static Player* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	float	Get_Speed()				{ return _speed;}
	void	Set_Speed(INT _value)	{ _speed = _value; }

	float	Get_DefaultSpeed() { return _defaultSpeed; }
	void	Set_DefaultSpeed(FLOAT _value) { _defaultSpeed *= _value; }

	int		Get_HP() { return Component_Collider->Get_Hp(); }
	void	Set_HP(INT _value) { Component_Collider->Set_Hp(_value); }

	int		Get_Key() { return _key; }
	void	Set_Key(INT _value) { _key = _value; }

	int		Get_Coin() { return _coin; }
	void	Set_Coin(INT _value) { _coin = _value; }

	int		Get_Crystal() { return _crystal; }
	void	Set_Crystal(INT _value) { _crystal = _value; }
	
	int		Get_DashStock() { return _dashstock; }
	void	Set_DashStock(INT _value) { _dashstock = _value; }
	
	int		Get_Token() { return _token; }
	void	Set_Token(INT _value) { _token = _value; }

	int		Get_Atk() { return (int)Component_Collider->Get_Att(); }
	void	Set_Atk(int atk) { Component_Collider->Set_Att(atk); _atk = atk; }

	int*	Get_Critical() { return &_critical; }
	void	Set_Critical(int critical) { _critical = critical; }

	float*	Get_ChargingSpeed() { return &_chargingSpeed; }
	void	Set_ChargingSpeed(int chargingSpeed) { _chargingSpeed = chargingSpeed; }

	float*	Get_Range() { return &_range; }
	void	Set_Range(int range) { _range = range; }

	float*	Get_ArrowSize() { return &_arrowSize; }
	void	Set_ArrowSize(int arrowSize) { _arrowSize = arrowSize; }

	float*	Get_ArrowSpeed() { return &_arrowSpeed; }
	void	Set_ArrowSpeed(int arrowSpeed) { _arrowSpeed = arrowSpeed; }

	int		Get_MaxArrow() { if (nullptr == _weaponSlot[_equipNum]) return 0; else return (int)_weaponSlot[_equipNum]->Get_Bow_Stat()->maxArrow; }

	float	Get_AddMaxArrow() { return _MaxArrow; }

	float*	Get_SlowTime() { return &_slowTime; }
	void	Set_SlowTime(int slowTime) { _slowTime = slowTime; }

	int		Get_CurArrowCount() { if (nullptr == _weaponSlot[_equipNum]) return 0;  else return _weaponSlot[_equipNum]->Get_Bow_Stat()->curArrow; } // 화살 개수
	int		Get_Bow_ImgIDX() { if (nullptr == _weaponSlot[_equipNum]) return -1; return _weaponSlot[_equipNum]->Get_IMG_IDX(); }

	pSee    Get_See()	 { return _see; }
	_int	GetBowCharging() { return _weaponSlot[_equipNum]->Get_Charging(); }

	bool	Get_Invincible() { return _isInvincible; }
	void	Set_Invincible(bool value) { _isInvincible = value;  }

	float*	Get_AttackSpeed() { return &_attackSpeed; }
	VOID	Set_AttackSpeed(float val) { _attackSpeed = val; }

	pState	Get_pState() { return _pState; }

	VOID	Set_AtkUpgrad(float val) { _atkUpgrade = val; }

	VOID	Set_DefaultAttackSpeed(float val) { _defaultAttackSpeed = val; }

	int		Get_Equipnum() { return _equipNum; }
	
	void			Set_CameraMove(bool Came) { CameraMove = Came; }
	bool			Get_CameraMove()		  { return CameraMove; }
	void			CheonLog_Spawn();

	TCHAR*	Get_FileName() { return FileName; }
	int		Get_AtomicTotal() { return _atomicTotal; }
	int		Get_AtomicRaady() { return _atomicReady; }
	void	Set_AtomicRaady() { _atomicReady += 1; }

	_vec3				Get_MouseDir();
	_float			Get_MouseDistance();
	void		Set_Start_Effect_Dead(bool val) {StartEffectDead = val;}

	// GET REliC
	int Get_Relic_ImgIdx(int slotIdx)
	{
		if (slotIdx < 0 || slotIdx >= 4) return -1; // 범위 체크
		if (nullptr == _artifactSlot[slotIdx]) return -1;

		return _artifactSlot[slotIdx]->Get_ItemIdx();
	}
	/// 인벤용
	void		Artifact_Effect();
	void		Chage_Item(int src, int dst);
	void		Buy_item(int itemIdx);
	void		Delete_item(int itemIdx);
	void		Set_MaxArrow(float val);
	void		Reset_MaxArrow();
private:
	virtual VOID Free();


private:
	D3DXVECTOR3			MousePicker_NonTarget(HWND _hWnd, Buffer* _TerrainBuffer, Transform* _TerrainTransform);
	D3DXVECTOR3			RayOnTerrain();
	D3DXVECTOR3			SetOnTerrain();
	
	void			Destroy_Weapon(int idx);

	void			IDLE_STATE(const _float& _DT);
	void			DASH_STATE(const _float& _DT);
	void			ATTACK_STATE(const _float& _DT);
	void			LANDING_STATE(const _float& _DT);
	bool			DEATH_STATE(const _float& _DT);
	void			START_STATE(const _float& _DT);
	void			Idle_Final_Input(const _float& _DT);

	void			SKILL_NONE(const _float& _DT);
	void			SKILL_TIMESLOW(const _float& _DT);
	void			SKILL_ATOMIC(const _float& _DT);

	void			SetGrahpic();
	void			Anim(TCHAR FileName[128], float delay, int maxIdx, bool reverse = false);
	void			Set_Effect(const _float& _DT);
	void			Calc_Near();
private:
	Bow*			_weaponSlot[4];
	Artifact*		_artifactSlot[4]; // 렐릭
	GameObject*		_inventory[10];
	int				_equipNum;
	Artifact*		_auguEffect;

	bool			Debug;
	bool			CameraMove;
	float			_cameraAngle;

	pState			_pState;
	eState			_eState;
	pSee			_see;
	skillState		_skillState;

	_uint			_frame;
	float			_frameTick;
	bool			_dashStart;
	float			_defaultSpeed;
	float			_dashTime;
	float			_dashG;
	float			_speed;
	float			_g;
	float			_slideTime;
	_vec3			_pulsepos;
	float			_attackDelay;
	int				_arrowCount;
	bool			_isStop;
	_vec3			_shadowPos;
	float			_popupTimer;
	bool			_bPopup;

	float			_dashRefillTimer;
	float			_skillTimer;
	float			_partnerTimer;
	bool			_skillNPC_On;
	bool			_skillArea_On;
	_vec3			_NPC_Pos;
	_vec3			_nearPos;

	bool			_Skill2;
	bool			_Skill2_Death;
	float			_Sheep_Summon_Timer;

	float			_animSpeed;
	float			_originArrowSpeed;
	float			_originDefualtSpeed;

	float			_alphaRatio;
	bool			_isInvincible;		//	무적 상태
	float			_invincibleTimer;	//	무적 타이머
	float			_alphaDelayTimer;	//	깜박임 타이머
	float			_arrowTimer;

	int				_atomicCount;
	int				_atomicTotal;
	int				_atomicReady;

	float			_defaultAttackSpeed;
	BOOL			RealStart;

	float			LandingTimer;
	bool			StartEffectDead;
	bool			SummonStart;

	////////////////// UI
	int				_hp;			// 플레이어 HP
	int				_dashstock;		// 플레이어 MP(눈물모양)
	int				_key;			// 플레이어 key
	int				_coin;			// 플레이어 coin
	int				_crystal;		// 플레이어 crystal
	int				_token;			// 플레이어 스킬 횟수(다이아몬드 모양)
	int				_atk;			// 공격력
	int				_critical;		// 크리티컬확율 ex) 30퍼면 30
	float			_chargingSpeed; // 차징 스피드 ex) 0.5면 2배 빨라짐, 왠만하면 0.5로 차징 애니메이션 때문에
	float			_range;			// 사거리 ex) 1.5면 1.5배 증가
	float			_arrowSize;		// 화살 크기 ex) 1.5면 1.5배 증가
	float			_arrowSpeed;	// 화살 스피드 ex) 1.5면 1.5배 증가
	float			_MaxArrow;		// 화살 개수 증가 ex) 1.3이면 30퍼 증가
	float			_slowTime;		// 시간 제어 스킬 지속시간 일단 4초 초기화
	float			_hit_inv_Time;	// 피격시 무적 유지시간;
	float			_dash_inv_Time;	// 대시시 무적 유지시간;
	float			_walk_time;     // 걷는 모션시 사운드 간격 조절용 타이머
	float			_attackSpeed;   // 공격속도
	float			_atkUpgrade;

	TCHAR FileName[128] = L"";
	//temp
	public:

		BOOL			OnCollisionEnter(GameObject* _Other)override;
		BOOL			OnCollisionStay(GameObject* _Other) override;
		BOOL			OnCollisionExit(GameObject* _Other)		override;

	public:
		HRESULT			MiniGameInit();
		HRESULT			MiniGameExit();
		void			Set_IsFalling(bool b) { Is_Falling = b;}
		bool			Get_IsFalling() { return Is_Falling; }
		void			Fall(const _float& _DT);
		void			Set_pState(pState pState) { _pState = pState; }
		void			Set_eState(eState eState) { _eState = eState; }
		void			Set_EventState(bool b) { m_bEventState = b; }

	private:
		SCENE_TYPE		m_eCurrScene;
		_vec3			m_vBackUpPos;
		_vec3			m_vBackupScale;
		bool			Is_Falling;
		_float			m_fBackUpHp;
		bool			m_bEventState = false;

};