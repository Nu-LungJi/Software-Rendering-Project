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

    virtual     HRESULT     Ready_Buffer( _int NumParticles);
    virtual     void        Render_Buffer();
    virtual		void		Reset_Particle(ATTR* attribute, _vec3* Look = 0);
    virtual	    void		Reset();
    virtual		void		Add_Particle(_vec3* Look = 0);
    virtual		void		PreRedner_Particle();
    virtual     _int		Update_Particle(const _float& fTimeDelta);

    void                    Set_Pos(_vec3 Pos) { vPos = Pos; }
    void                    Set_Look(_vec3 Look, _float Ang) { vLook = Look;  fAngle = Ang; }
    void                    Set_OiriginLook(_vec3 Look) { vOriginLook = Look; }
    void                    Set_OiriginPos(_vec3 Pos) { vOriginPos = Pos; }

    void                    Set_Right(_vec3 Right) { vRight = Right; }
    void                    Set_Up(_vec3 Up) { vUp = Up; }
private:
    list<Attribute>         m_ParticlePool;


    _matrix                 World;
    _vec3                   vPos;
    _vec3                   vOriginPos;
    _vec3                   vLook;
    _vec3                   vOriginLook;
    _vec3                   vRight;
    _vec3                   vUp;
    _float                    i;
    _float                   j;
    _float                  fAngle;
public:
    static ParticleEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev,_int NumParticles);
    virtual Component* Clone();
private:
    virtual void Free();
};


END
