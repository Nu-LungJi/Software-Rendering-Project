#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include "Light.h"
BEGIN(Engine)
class ENGINE_DLL LightManager :
    public Base
{
    DECLARE_SINGLETON(LightManager)
private:
    explicit LightManager();
    virtual ~LightManager();

public:
   HRESULT  Ready_Light(LPDIRECT3DDEVICE9 _GRPDEV, D3DLIGHT9* pLight,const _uint& iIndex);

private:
    list<Light*> m_LightList;

private:
    void        Free();
};

END