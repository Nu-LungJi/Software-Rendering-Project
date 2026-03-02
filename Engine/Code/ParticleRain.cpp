#include "ParticleRain.h"

ParticleRain::ParticleRain()
{
}

ParticleRain::ParticleRain(LPDIRECT3DDEVICE9 pGraphicDev)
	:CDYBuffer(pGraphicDev)
{
}

ParticleRain::ParticleRain(const ParticleRain& rhs)
	:CDYBuffer(rhs)
{

}

ParticleRain::~ParticleRain()
{
}

HRESULT ParticleRain::Ready_Buffer(_Bound* BoundingBox, _int NumParticles)
{
	m_bBoundingBox = *BoundingBox;
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 2048;
	m_fsize = 0.5;
	m_dwFVF = FVF_COL;
	m_dwOffset = 0;
	m_dwBatchSize = 512;


	for (int i = 0; i < NumParticles; i++)
		CDYBuffer::Add_Particle();

	if (FAILED(CDYBuffer::Ready_Buffer()))
		return E_FAIL;

	return S_OK;
}

void ParticleRain::Render_Buffer()
{
	CDYBuffer::Render_Buffer();
}

ParticleRain* ParticleRain::Create(LPDIRECT3DDEVICE9 pGraphicDev, _Bound* BoundingBox, _int NumParticles)
{
	ParticleRain* pParticle = new ParticleRain(pGraphicDev);
	if (FAILED(pParticle->Ready_Buffer(BoundingBox, NumParticles)))
	{
		Safe_Release(pParticle);
		MSG_BOX("Create Particle Failed");
		return nullptr;
	}

	return pParticle;
}

void  ParticleRain::Reset()
{
	CDYBuffer::Reset();
}

void  ParticleRain::Add_Particle(_vec3* Look)
{
	CDYBuffer::Add_Particle(Look);
}

void  ParticleRain::PreRedner_Particle()
{
	//초기 렌더링 상태를 지정한다
	GRPDEV->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	GRPDEV->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	GRPDEV->SetRenderState(D3DRS_POINTSIZE, FtoDw(0.2F));
	//GRPDEV->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));
	//GRPDEV->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	//GRPDEV->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	GRPDEV->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));


	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

}
void  ParticleRain::PostRender_Particle()
{
	GRPDEV->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	GRPDEV->SetRenderState(D3DRS_POINTSCALEENABLE, false);
}
void  ParticleRain::Reset_Particle(ATTR* attribute, _vec3* Look)
{
	_vec3 vRot = { 1,0,0 };
	_matrix  matRotx;
	attribute->bIsAlive = true;
	CDYBuffer::Get_RandomVector(&attribute->vPosition,
		&m_bBoundingBox.vMin, &m_bBoundingBox.vMax);

	attribute->vVelocity.x = CDYBuffer::Get_RandomFloat(0.5f, 1.0f) * -10.0f;
	attribute->vVelocity.y = CDYBuffer::Get_RandomFloat(0.5f, 1.0f) * -10.0f;
	attribute->vVelocity.z = 0.0f;

	//D3DXMatrixRotationX(&matRotx, 55);
	//
	//D3DXVec3TransformNormal(&attribute->vVelocity, &vRot, &matRotx);

	attribute->dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 0.f);
}

_int ParticleRain::Update_Particle(const _float& fTimeDelta)
{
	for (auto& iter : m_ParticleList)
	{
		iter.vPosition += iter.vVelocity * fTimeDelta;

		if (m_bBoundingBox.isPointInside(iter.vPosition) == false)
		{
			Reset_Particle(&(iter));
		}
	}
	_int EXIT = fTimeDelta;
	return EXIT;
}
Component* ParticleRain::Clone()
{
	return new ParticleRain(*this);
}
void  ParticleRain::Free()
{
	CDYBuffer::Free();
}
