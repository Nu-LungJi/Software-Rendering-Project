#pragma once
#include "GameObject.h"
#include "Player.h"

static int			_arrowCount = 1;
enum class BowType {
	FairyBow,
	IceBow,
	EvilHeadBow,
	WindBow,
	AtomicBow,

	End
};

struct BowStat {
	int bowLv;
	int minAtk;
	int maxAtk;
	int maxArrow;
	int curArrow;
	int range;
	_float delay;
};

class Bow : public GameObject
{
private:
	explicit Bow(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Bow(const GameObject& _RHS);
	virtual ~Bow();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();

public:
	void			Set_PlayerPos(_vec3* pos) { _bowPos = pos; }
	void			Set_Destroy() { _isDestroied = true; }

private:
	HRESULT			Component_Initialize();
	void			SetGrahpic();
	void			CreateArrow(const _float& _DT);
	void			CreateEffect(const _float& _DT);
	void			CreateChargingArrow(const _float& _DT);
	void			CreateChargingEffect(const _float& _DT);
	void			Late_Ready();
	void			MakeArrow(_vec3 pos, _vec2 arrowDir, bool charging = false);
private:
	Buffer* Component_Buffer;
	Transform* Component_Transform;
	Texture* Component_Texture;
	IDirect3DTexture9* g_pTexture = NULL; // 로드된 이미지
	ID3DXSprite* g_pSprite = NULL;   // 스프라이트 객체
public:
	static Bow* Create(LPDIRECT3DDEVICE9 _GRPDEV);
	BowType		Get_Bow_Type()					{ return _type; }
	void		Set_Bow_Type(BowType type)		{ _type = type; }
	void		Set_Bow_Equip(bool isequip)		{ _isEquip = isequip; }
	BowStat*	Get_Bow_Stat()					{ return &_Stat; }
	_int		Get_Charging()					{ return _Charging; }
	_int		Get_IMG_IDX()					{ return _imgIDX; }
	void		Refill_Arrow()					{ _Stat.curArrow = _Stat.maxArrow; }
	void		Reset_MaxArrow();
private:
	virtual VOID Free();
private:
	_vec3*		_bowPos;
	_float		_cameraAngle;
	_vec3		_cameraDir;
	int			_imgIDX;
private:
	BowType		_type;
	BowStat		_Stat;
	bool		_lateReady;

	bool		_isEquip;
	bool		_isDestroied;
	float		_motionDelay;
	float		_alphaRatio;

	_vec3		_pulsepos;
	_vec3		_arrowPos;
	float		_attackDelay;
	float		_attackTimer;

	_int		_Charge;
	_int		_Charging;
	_float		_ChargingTime;
	_float		_chargingTime;

	_float		_windChargingTimer;

	int			_playerAtk;
	int*		_playerCritical;
	float*		_playerChargingSpeed;
	float*		_playerAttackSpped;
};

