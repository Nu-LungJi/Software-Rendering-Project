#include "CDYBuffer.h"

CDYBuffer::CDYBuffer() : m_pVB(nullptr)
, m_dwVtxSize(0)
, m_dwVtxCnt(0)
, m_dwFVF(0), m_femitRate(0), m_fsize(0), m_iMaxParticle(0), m_dwOffset(0), m_dwBatchSize(0)
{
	ZeroMemory(&m_vOrigin, sizeof(_vec3));
	ZeroMemory(&m_bBoundingBox, sizeof(_vec3));
}

CDYBuffer::CDYBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Component(pGraphicDev), m_pVB(nullptr)
	, m_dwVtxSize(0)
	, m_dwVtxCnt(0)
	, m_dwFVF(0), m_femitRate(0), m_fsize(0), m_iMaxParticle(0), m_dwOffset(0), m_dwBatchSize(0)
{
	ZeroMemory(&m_vOrigin, sizeof(_vec3));
	ZeroMemory(&m_bBoundingBox, sizeof(_vec3));
}

CDYBuffer::CDYBuffer(const CDYBuffer& rhs)
	:Component(rhs), m_pVB(rhs.m_pVB)
	, m_dwVtxSize(rhs.m_dwVtxSize)
	, m_dwVtxCnt(rhs.m_dwVtxCnt)
	, m_dwFVF(rhs.m_dwFVF)
	, m_IdxFmt(rhs.m_IdxFmt),
	m_femitRate(rhs.m_femitRate),
	m_fsize(rhs.m_fsize),
	m_iMaxParticle(rhs.m_iMaxParticle),
	m_dwOffset(rhs.m_dwOffset),
	m_dwBatchSize(rhs.m_dwBatchSize), m_bBoundingBox(rhs.m_bBoundingBox)
{
	m_pVB->AddRef();
	memcpy(&m_vOrigin, &rhs.m_vOrigin, sizeof(_vec3));
	size_t i = m_ParticleList.size();
	m_ParticleList.resize(i);

	m_ParticleList = rhs.m_ParticleList;
}

CDYBuffer::~CDYBuffer()
{
}
_float CDYBuffer::Get_RandomFloat(_float flowBound, _float fhighBound)
{
	if (flowBound >= fhighBound)
		return flowBound;

	float f = (rand() % 10000) * 0.0001f;
	return (f * (fhighBound - flowBound) + flowBound);
}

void CDYBuffer::Get_RandomVector(_vec3* vOut, _vec3* vMin, _vec3* vMax)
{
	vOut->x = Get_RandomFloat(vMin->x, vMax->x);
	vOut->y = Get_RandomFloat(vMin->y, vMax->y);
	vOut->z = Get_RandomFloat(vMin->z, vMax->z);
}

HRESULT CDYBuffer::Ready_Buffer()
{
	// D3DPOOL_DEFAULT : 그래픽 메모리
	// D3DPOOL_MANAGED : 그래픽 메모리에 저장, 메인 메모리 백업
	// D3DPOOL_SYSTEMMEM : 메인 메모리에 저장
	// D3DPOOL_SCRATCH : 메인 메모리 저장(DX 장치로 접근 불가)

	if (FAILED(GRPDEV->CreateVertexBuffer(m_dwVtxCnt * m_dwVtxSize,	// 버텍스 버퍼의 크기
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,			// 0인 경우 정적 버퍼, D3DUSAGE_DYNAMIC인 경우 동적 버퍼
		m_dwFVF,	// 버텍스 속성
		D3DPOOL_DEFAULT,	// 정적 버퍼인 경우 MANAGED
		&m_pVB,		// 버텍스 버퍼 객체 생성
		NULL)))		// 공유할 일 없기 때문에 NULL
		return E_FAIL;

	return S_OK;
}

void CDYBuffer::Reset()
{
	//시스템 내의 모든 파티클 속성을 리셋
	for (auto& iter : m_ParticleList)
	{
		Reset_Particle(&(iter));
	}
}

void CDYBuffer::Add_Particle(_vec3* Look)
{
	//시스템에 파티클을 추가한다. 이 메서드는 리스트에 추가하기전에 파티클
	//초기화 하는데 리셋 파티클 메서드를 이용

	ATTR attribute;

	Reset_Particle(&attribute, Look);

	m_ParticleList.push_back(attribute);
}

_int CDYBuffer::Update_Particle(const _float& fTimeDelta)
{

	return 0;
}


//_bool CDYBuffer::IsEmpty_Particle()
//{
//	return _bool();
//}
//
//_bool CDYBuffer::IsDead_Particle()
//{
//	return _bool();
//}
//
void CDYBuffer::removeDead_Particle()
{
	for (auto iter = m_ParticleList.begin(); iter != m_ParticleList.end();)
	{
		_bool bLive = iter->bIsAlive;

		if (bLive == false)
		{
			iter = m_ParticleList.erase((iter));
			continue;
		}

		if (iter != m_ParticleList.end())
			++iter;
	}
}

void CDYBuffer::Render_Buffer()
{
	if (!m_ParticleList.empty())
	{

		//PreRedner_Particle();
		GRPDEV->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);

		GRPDEV->SetFVF(m_dwFVF);

		if (m_dwOffset >= m_dwVtxCnt)
			m_dwOffset = 0;

		Engine::VTXCOL* v;

		m_pVB->Lock(m_dwOffset * m_dwVtxSize,
			m_dwBatchSize * m_dwVtxSize,
			(void**)&v,
			m_dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		_ulong	dwParticleInBatch = 0;
		_ulong	dwCheck = 0;
		for (auto iter = m_ParticleList.begin(); iter != m_ParticleList.end(); ++iter)
		{
			_bool bAlive = iter->bIsAlive;

			++dwCheck;
			if (bAlive)
			{
				//한단계 생존한 파티클을 다음 버텍스 버퍼 세그먼트로 복사
				v->vPosition = iter->vPosition;
				v->dwColor = (D3DCOLOR)iter->dwColor;
				v++; // 다음 원소
				dwParticleInBatch++;
				if (dwParticleInBatch == m_dwBatchSize)
				{
					//다채움?
					m_pVB->Unlock();
					//그리셈
					GRPDEV->DrawPrimitive(D3DPT_POINTLIST, m_dwOffset, m_dwBatchSize);
					m_dwOffset += m_dwBatchSize; //다음 단계의 처음 오프셋 이동 
					//버텍스 버퍼의 경계를 넘는 메모리로 오프셋을 설정하지 않는다
					// 경계를 넘을 경우 처음부터시작

					if (m_dwOffset >= m_dwVtxCnt)
						m_dwOffset = 0;

					//다시열어?
					m_pVB->Lock(m_dwOffset * m_dwVtxSize,
						m_dwBatchSize * m_dwVtxSize,
						(void**)&v,
						m_dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					dwParticleInBatch = 0;
					//아
				}
			}

		}
		m_pVB->Unlock();


		if (dwParticleInBatch)
		{
			GRPDEV->DrawPrimitive(
				D3DPT_POINTLIST,
				m_dwOffset,
				dwParticleInBatch);
		}

		m_dwOffset += m_dwBatchSize;
		//PostRender_Particle();
	}

}

void CDYBuffer::Free()
{
	Safe_Release(m_pVB);
	//Safe_Release(m_pIB);

	Component::Free();
}
