#pragma once
#include"GameObject.h"

enum class eHurdleType	: uint8_t { Bat = 0, Scorpion, Shotgun, Slime, Cube, Random };	//must 0~64

typedef struct tagHurdleComponent
{
	uint16_t	ID					= 0;
	Buffer*		Component_Buffer	= nullptr;
	Transform*	Component_Transform = nullptr;
	Collider*	Component_Collider	= nullptr;
	TEXINFO		Textures;

	~tagHurdleComponent() {
		if (Component_Buffer)	{ Component_Buffer->Release();		Component_Buffer = nullptr; }
		if (Component_Transform){ Component_Transform->Release();	Component_Transform = nullptr; }
		if (Component_Collider) { Component_Collider->Release();	Component_Collider = nullptr; }
	}
}HURDLECOM;

class Hurdle : public GameObject
{
public:
	explicit Hurdle(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Hurdle(const GameObject& _RHS);
	virtual ~Hurdle();

public:
	static	Hurdle* Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 _vSrc, _vec3 _vDst, _float _fSpeed = 2.f, _float _fScale = 1.f, uint8_t _Type = (uint8_t)eHurdleType::Random);

	HRESULT			Ready_Hurdle(_vec3 __vSrc, _vec3 __vDst, _float __fSpeed, _float _fScale, uint8_t __Type);		//	충돌이 AABB 만 가능하므로 반드시 직선이어야 함
	INT				Update_GameObject(const _float& _DT)	override;
	VOID			LateUpdate_GameObject(const _float& _DT)override;
	VOID			Render_GameObject()						override;


	BOOL			OnCollisionEnter(GameObject* _Other)	override;

private:
	VOID		Free() {};

	Transform*	Component_Transform;
	Collider*	Component_Collider;

private:
	GameObject*			m_pPlayer;

	vector<HURDLECOM*>	m_vecComponent;

	uint8_t		m_ubType	= (uint8_t)eHurdleType::Random;

	_vec3		m_vSrc		= { 0.f,0.f,0.f };
	_vec3		m_vDst		= { 0.f,0.f,0.f };
	_float		m_fSpeed	= 10.f;
	_float		m_fScale	= 1.f;
	_float		m_fDis		= 0.f;
	_vec3		m_vDir		= { 1.f,0.f,0.f };
};