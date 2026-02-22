#pragma once
#include "GameObject.h"

static int			_evilCount = 1;
enum class ArrowType {
	FairyArrow,
	FairyCharging,
	IceArrow_LV1,
	IceArrow_LV2,
	IceCharging,
	EvilHead_Arrow,
	EvilHeadCharging,
	Wind_Arrow,
	WindCharging,

	End
};

class Arrow : public GameObject
{
private:
	explicit Arrow(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Arrow(const GameObject& _RHS);
	virtual ~Arrow();

public:
	virtual			HRESULT		Ready_GameObject(BowType _BOWTYPE, int _LVEL, int arrowAtk, _vec3* _PlayerPOS, _vec2 _arrowDir);
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();

private:
	HRESULT			Component_Initialize();
	void			SetGrahpic();
	void			Destory_Tile();
private:
	Buffer*			Component_Buffer;
	Transform*		Component_Transform;
	Collider*		Component_Collider;

	IDirect3DTexture9* g_pTexture = NULL; // 로드된 이미지
	ID3DXSprite* g_pSprite = NULL;   // 스프라이트 객체

public:
	static Arrow*	Create(LPDIRECT3DDEVICE9 _GRPDEV, BowType _BOWTYPE, int _LVEL, int arrowAtk, _vec3* _PlayerPOS, _vec2 _arrowDir);
	int				Get_Atk()	{ return _arrowAtk; }
	void			Set_HP(int hp) { _hp = hp; }
	BOOL			OnCollisionEnter(GameObject* _Other);
	BOOL			OnCollisionStay(GameObject* _Other);
private:
	virtual VOID Free();
	void			Search_Target();
private:
	ArrowType	_type;
	BowType		_bowType;
	int			_hp;
	_float		_originAngle;

	float*		_playerRange;
	float*		_playerArrowSize;
	float*		_playerArrowSpeed;

	_vec3		_playerPos;
	float		_angle;
	float		_size;

	int			_frame;
	float		_frameDelay;
	float		_effectDelay;

	float		_lifeTime;
	float		_speed;
	float		_sumSpeed;
	int			_arrowAtk;
	_vec3*		_targetPos;

	float		_evilMoveTime;
	float		_sumEvilSpeed;
	float		_ThunderDelay;

	float		_EvilTime;
};

