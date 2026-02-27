#pragma once
#include "GameObject.h"

class CXZTile :
    public GameObject
{
private:
    explicit CXZTile(LPDIRECT3DDEVICE9 _GRPDEV);
    explicit CXZTile(const GameObject& _RHS);
    virtual ~CXZTile();

public:

    virtual			HRESULT		Ready_GameObject(TILE_SIDE eid, TILE_STATE eState);
    virtual			INT			Update_GameObject(CONST FLOAT& _DT);
    virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
    virtual			VOID		Render_GameObject();
public:
    void                    Set_Buffer(TILE_SIDE eid);
    D3DMATERIAL9            Set_Material();
    void                    Frame_Move      (CONST FLOAT& _DT);
    void                    Tile_Animation  (CONST FLOAT& _DT);
    void                    Tile_Destory    (CONST FLOAT& _DT);
    void                    Tile_Potal      (CONST FLOAT& _DT);
    void                    Tile_Potal_Effect(CONST FLOAT& _DT);
    void                    Tile_Trigger      ();
    void                    Tile_Gasi_Destory(CONST FLOAT& _DT);
    void                    Tile_Move_Effect(CONST FLOAT& _DT, TILE_STAGE eid);
    void                    Tile_Boom(CONST FLOAT& _DT);
    void                    Set_Destory()   { m_bDestroy = true; }
    Transform*              Crash_Player();
    Transform*              Crash_Arrow();
private:
    HRESULT			        Component_Initialize(TILE_SIDE eid, TILE_STATE eState);
    
    TileInfo*               m_pTileInfo;
    Transform*              m_pTransform;
    Buffer*                 m_pBuffer;
    Buffer*                 m_CubeBuffer;
    D3DMATERIAL9            m_Material;
private:
    _float                  m_fAlpha;
    _float                  m_fHeight;
    _float                  m_fHeightSpeed;
    _float                  m_fCount;
    _float                  m_fFrame;
    _float                  m_fTime;

    _bool                   m_bEffect;
    _bool                   m_bStopFrame;
    _bool                   m_bDestroy;
    _bool                   m_bMiniGame;
    DWORD                   m_Alpha;

public:
    static         CXZTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, TILE_SIDE eid, TILE_STATE eState);

private:
   virtual  void            Free();

};

