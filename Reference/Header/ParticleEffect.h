#pragma once
#include "CDYBuffer.h"
BEGIN(Engine)

class ENGINE_DLL ParticleEffect :
    public CDYBuffer
{
private:
    explicit ParticleEffect();
    explicit ParticleEffect(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit ParticleEffect(const ParticleEffect& rhs);
    virtual ~ParticleEffect();

public:
    virtual     void        PostRender_Particle();

    virtual     HRESULT     Ready_Buffer(_vec3* origin, _int NumParticles);
    virtual     void        Render_Buffer();
    virtual		void		Reset_Particle(ATTR* attribute, _vec3* Look = 0);
    virtual	    void		Reset();
    virtual		void		Add_Particle(_vec3* Look = 0);
    virtual		void		PreRedner_Particle();
    virtual     _int		Update_Particle(const _float& fTimeDelta);

public:
    static ParticleEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* origin, _int NumParticles);
    virtual Component* Clone();
private:
    virtual void Free();
};


END
