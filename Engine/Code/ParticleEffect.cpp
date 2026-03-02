#include "ParticleEffect.h"

ParticleEffect::ParticleEffect()
{
}

ParticleEffect::ParticleEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	:CDYBuffer(pGraphicDev)
{
}

ParticleEffect::ParticleEffect(const ParticleEffect& rhs)
	:CDYBuffer(rhs)
{

}

ParticleEffect::~ParticleEffect()
{
}

HRESULT ParticleEffect::Ready_Buffer(_vec3* origin, _int NumParticles)
{
	m_vOrigin = *origin;
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 2048;
	m_fsize = 10.f;
	m_dwFVF = FVF_COL;
	m_dwOffset = 0;
	m_dwBatchSize = 512;


	for (int i = 0; i < NumParticles; i++)
		CDYBuffer::Add_Particle();

	if (FAILED(CDYBuffer::Ready_Buffer()))
		return E_FAIL;

	return S_OK;
}

void ParticleEffect::Render_Buffer()
{
	CDYBuffer::Render_Buffer();
}

ParticleEffect* ParticleEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* origin, _int NumParticles)
{
	ParticleEffect* pParticle = new ParticleEffect(pGraphicDev);
	if (FAILED(pParticle->Ready_Buffer(origin, NumParticles)))
	{
		Safe_Release(pParticle);
		MSG_BOX("Create Particle Failed");
		return nullptr;
	}

	return pParticle;
}
void  ParticleEffect::Reset_Particle(ATTR* attribute, _vec3* Look)
{
	attribute->bIsAlive = true;
	attribute->vPosition = m_vOrigin;
	_vec3 vMin = { -1.0f, -1.0f, -1.0f };
	_vec3 vMax = { 1.0f, 1.0f, 1.0f };
	CDYBuffer::Get_RandomVector(&attribute->vVelocity, &vMin, &vMax);

	D3DXVec3Normalize(&attribute->vVelocity, &attribute->vVelocity);
	attribute->vVelocity *= 10.f;
	attribute->dwColor = D3DXCOLOR(CDYBuffer::Get_RandomFloat(0.0f, 1.0f),
		CDYBuffer::Get_RandomFloat(0.0f, 1.0f),
		CDYBuffer::Get_RandomFloat(0.0f, 1.0f), 1.f);

	attribute->fAge = 0.0f;
	attribute->fLifeTime = 2.0f;
}
void  ParticleEffect::Reset()
{
	CDYBuffer::Reset();
}

void  ParticleEffect::Add_Particle(_vec3* Look)
{
	CDYBuffer::Add_Particle(Look);
}

void  ParticleEffect::PreRedner_Particle()
{

	GRPDEV->SetRenderState(D3DRS_LIGHTING, false);
	GRPDEV->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	GRPDEV->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	GRPDEV->SetRenderState(D3DRS_POINTSIZE, FtoDw(0.2f));

	//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));
	//
	//
	//m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	//m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);

}
_int ParticleEffect::Update_Particle(const _float& fTimeDelta)
{
	for (auto& iter : m_ParticleList)
	{
		if (iter.bIsAlive)
		{
			iter.vPosition += iter.vVelocity * fTimeDelta;

			iter.fAge += fTimeDelta;

			if (iter.fAge > iter.fLifeTime)
				iter.bIsAlive = false;
		}
	}

	_int EXIT = fTimeDelta;
	return EXIT;
}


void  ParticleEffect::PostRender_Particle()
{

}
Component* ParticleEffect::Clone()
{
	return new ParticleEffect(*this);
}
void  ParticleEffect::Free()
{
	CDYBuffer::Free();
}
