#pragma once
#include "Component.h"
BEGIN(Engine)

class ENGINE_DLL CDYBuffer : public Component
{
protected:
	explicit CDYBuffer();
	explicit CDYBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDYBuffer(const CDYBuffer& rhs);
	virtual ~CDYBuffer();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();
	virtual     _int		Update_Particle(const _float& fTimeDelta);
	virtual		void		Add_Particle(_vec3* Look = 0);
	virtual		void		PreRedner_Particle() = 0;
	virtual		void		Reset_Particle(ATTR* attribute, _vec3* Look = 0) = 0;
	virtual		void		PostRender_Particle() = 0;
	void					removeDead_Particle();
	_float					Get_RandomFloat(_float flowBound, _float fhighBound);
	void					Get_RandomVector(_vec3* vOut, _vec3* vMin, _vec3* vMax);
	void					Reset();

	DWORD		FtoDw(float f)
	{
		return *((DWORD*)&f);
	}

	//_bool		IsEmpty_Particle(); // 현재 시스템에 파티클이 없는경우 true 아니면 false
	//_bool		IsDead_Particle(); // 시스템 내의 파티클이 전부 죽으면 true 아니면 false
protected:

	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	_ulong				m_dwVtxSize;
	_ulong				m_dwVtxCnt;			//버텍스 버퍼가 보관할 수 있는 파티클의 수
	_ulong				m_dwFVF;
	_vec3				m_vOrigin;				// 시스탬 내에서 파티클이 시작되는곳
	Engine::_Bound		m_bBoundingBox;		// 파티클이 이동할 수 있는 부피를 제한하는데 이용 
	// 원하는 영역을 벗어난 파티클은 소멸
	_float				m_femitRate;     // 시스템에 새로운 파티클이 추가되는 비율
	_float				m_fsize;			//시스탬 내 모든 파티클의 크기
	list<ATTR>			m_ParticleList;		//파티클을 만들고 제거하고 갱신하는데 리스트를 이용
	_int				m_iMaxParticle;			//주어진 시간 동안 시스템이 가질 수 있는 파티클의 수
	_ulong				m_dwOffset;			   //파티클 시스템 렌더링용
	_ulong				m_dwBatchSize;         //파티클 시스템 렌더링용

	D3DFORMAT			m_IdxFmt;

public:
	virtual		void	Free();
};

END

