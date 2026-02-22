#pragma once
#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL Light :
    public Base
{
private:
    explicit Light(LPDIRECT3DDEVICE9 _GRPDEV);
    virtual ~Light();

public:
    HRESULT         Ready_Light(D3DLIGHT9* pLight, const _uint& iIndex);

private:
    LPDIRECT3DDEVICE9 GRPDEV;
    _uint             m_iIndex;
    D3DLIGHT9         m_tLight;
public:
    static Light* Create(LPDIRECT3DDEVICE9 _GRPDEV,
        D3DLIGHT9* pLight, const _uint& iIndex);

private:
    virtual void    Free();

};

END