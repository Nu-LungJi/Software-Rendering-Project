#pragma once
#include "GameObject.h"

class Spawner :
    public GameObject
{
private:
    explicit Spawner(LPDIRECT3DDEVICE9 _GRPDEV);
    explicit Spawner(const GameObject& _RHS);
    virtual ~Spawner();

public:

    virtual			HRESULT		Ready_GameObject(TILE_SIDE eid, TILE_SPAWNER eSpawn, _vec3 vPos);
    virtual			INT			Update_GameObject(CONST FLOAT& _DT);
    virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
    virtual			VOID		Render_GameObject();

public:
    void                    Set_Buffer(TILE_SIDE eid);
    void                    Frame_Move(CONST FLOAT& _DT);
    void                    Monster_Spawn();
    void                    Monster_Spawn2();
    void                    Monster_Spawn3();
    void                    Monster_Spawn4();
    void                    CL_Spawn();
    void                    Set_Spawn(_bool bSpawn) { m_bSpawn = bSpawn; }
    void                    Defense_Spawn(const _float& _DT);
    Transform*               Crash_Player();
    void                      Boss();
private:
    HRESULT			Component_Initialize(TILE_SIDE eid, TILE_SPAWNER eSpawn );

    TileInfo*               m_pTileInfo;
    Transform*              m_pTransform;
    Buffer*                 m_pBuffer;
    
    vector<GameObject*>     m_vecMonsterDefense;
    
private:
    _float                  m_fFrame;
    _float                  m_fTime;
    _float                  m_fDefense;
    _float                  m_SpawnDelay;
    _bool                   m_bStopFrame;
    _bool                   m_bSpawn;
    _bool                   m_bTrigger;


    _int                    m_SpawnCnt;
    _int                    m_iCnt;
    _int                    m_StageCnt[3];
public:
    static         Spawner* Create(LPDIRECT3DDEVICE9 pGraphicDev, TILE_SIDE eid, TILE_SPAWNER eSpawn, _vec3 vPos);

private:
    virtual  void            Free();

};

