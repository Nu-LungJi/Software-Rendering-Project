#pragma once
#include "CDYBuffer.h"
class ENGINE_DLL ParticleRain :
    public CDYBuffer
{
private:
    explicit ParticleRain();
    explicit ParticleRain(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit ParticleRain(const ParticleRain& rhs);
    virtual ~ParticleRain();

public:

    virtual		void		PreRedner_Particle();
    virtual     void        PostRender_Particle();
    virtual HRESULT Ready_Buffer(_Bound* BoundingBox, _int NumParticles);
    virtual void    Render_Buffer();

    virtual		void		Reset_Particle(ATTR* attribute, _vec3* Look = 0);
    virtual	    void		Reset();
    virtual		void		Add_Particle(_vec3* Look = 0);
    virtual     _int		Update_Particle(const _float& fTimeDelta);


public:
    static ParticleRain* Create(LPDIRECT3DDEVICE9 pGraphicDev, _Bound* BoundingBox, _int NumParticles);
    virtual Component* Clone();
private:
    virtual void Free();
};

