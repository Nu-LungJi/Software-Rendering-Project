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

HRESULT ParticleEffect::Ready_Buffer(_int NumParticles)
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 64;
	m_fsize = 0.2f;
	m_dwFVF = FVF_COL;
	m_dwOffset = 0;
	m_dwBatchSize = 64;
	i = 0.1f;
	j = 0.5f;
	fAngle = 0.f;
	for (int i = 0; i < NumParticles; i++)
		CDYBuffer::Add_Particle();
	m_bBoundingBox.vMin = { -0.9f,-0.3f, -0.9f };
	m_bBoundingBox.vMax = { 0.1f,-0.1f, 0.1f };

	if (FAILED(CDYBuffer::Ready_Buffer()))
		return E_FAIL;

	return S_OK;
}

void ParticleEffect::Render_Buffer()
{
	CDYBuffer::Render_Buffer();
}

ParticleEffect* ParticleEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int NumParticles)
{
	ParticleEffect* pParticle = new ParticleEffect(pGraphicDev);
	if (FAILED(pParticle->Ready_Buffer(NumParticles)))
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
	_vec3 vMin = { -0.9f,-0.3f, -0.9f };
	_vec3 vMax = { 0.1f, -0.1f, 0.1f };
	

	//D3DXVec3Normalize(&m_bBoundingBox.vMin, &m_bBoundingBox.vMin);
	//D3DXVec3Normalize(&m_bBoundingBox.vMax, &m_bBoundingBox.vMax);
	//
	//Get_RandomVector(&attribute->vPosition, &vMin, &vMax);
	vOriginPos = vOriginPos - vPos;

	attribute->vPosition.x =  CDYBuffer::Get_RandomFloat(0.15f, 0.35f) * -3.f;
	attribute->vPosition.y = CDYBuffer::Get_RandomFloat(-0.25f, -0.4f) * -3.f;
	attribute->vPosition.z = CDYBuffer::Get_RandomFloat(-0.2f, 0.2f);
	attribute->vPosition += vOriginPos;
	attribute->vVelocity =   vLook + vOriginLook* 0.5f;
	attribute->World = World;
	attribute->dwColor = D3DXCOLOR(CDYBuffer::Get_RandomFloat(0.1f, 1.0f),
	
	CDYBuffer::Get_RandomFloat(0.0f, 1.0f),
	CDYBuffer::Get_RandomFloat(0.0f, 1.0f), 0.5f);

	_int iRand = 1 + rand()%3;
		
	
	attribute->fAge = 0 + j;
	attribute->fLifeTime = 0 + i;
	i += 0.1f;
	j -= 0.1f;
	if (j < 0)
		j = 0.5f;
	if (i > 3)
		i = 0.5f;
}
void  ParticleEffect::Reset()
{
	for (auto iter = m_ParticlePool.begin(); iter != m_ParticlePool.end();)
	{
		if (m_ParticlePool.empty())
			return;

		if ((*iter).bIsAlive == false)
		{
			Reset_Particle(&(*iter));

			m_ParticleList.push_back(*iter);
			iter = m_ParticlePool.erase(iter);
		}
		else
			++iter;
			
	}
}

void  ParticleEffect::Add_Particle(_vec3* Look)
{
	CDYBuffer::Add_Particle(Look);
}

void  ParticleEffect::PreRedner_Particle()
{

	GRPDEV->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	GRPDEV->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	GRPDEV->SetRenderState(D3DRS_POINTSIZE, FtoDw(0.1F));

	GRPDEV->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	GRPDEV->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	GRPDEV->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(0.5f));
	

	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	GRPDEV->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

}
_int ParticleEffect::Update_Particle(const _float& fTimeDelta)
{
	for (auto iter = m_ParticleList.begin(); iter != m_ParticleList.end();)
	{
		if ((*iter).bIsAlive)
		{
			(*iter).vPosition += (*iter).vVelocity * 1.f * fTimeDelta *fAngle;

			(*iter).fAge += fTimeDelta;

			if ((*iter).fAge > (*iter).fLifeTime)
			{
				(*iter).bIsAlive = false;
				m_ParticlePool.push_back(*iter);
				iter = m_ParticleList.erase(iter);
			
			}
			else
				++iter;
		}
	}
	_int EXIT = fTimeDelta;
	return EXIT;
}


void  ParticleEffect::PostRender_Particle()
{
	GRPDEV->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	GRPDEV->SetRenderState(D3DRS_POINTSCALEENABLE, false);
}
Component* ParticleEffect::Clone()
{
	return new ParticleEffect(*this);
}
void  ParticleEffect::Free()
{
	CDYBuffer::Free();
}
