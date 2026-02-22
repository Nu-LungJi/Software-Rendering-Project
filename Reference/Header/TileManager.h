#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include "GameObject.h"
#include "Buffer.h"
BEGIN(Engine)

class ENGINE_DLL TileManager :
    public Base
{
    DECLARE_SINGLETON(TileManager)
private:
    explicit TileManager();
    virtual ~TileManager();


public:
    vector<GameObject*>& Get_TileList(TILE_STAGE eStage, TILEMODE_CHANGE eid) {
       
            return m_vecTileBuffer[eStage][eid];
     }
    vector<GameObject*>& Get_DestoryTile_List() {

        _int a = Get_Stage();
        return m_vecTileBuffer[Get_Stage()][0];
    }
    void                    Set_StageCnt() 
    { 
        for (_int i = 0; i < TILE_STAGE::STAGE_END; ++i)
        {
            _int iCnt = 0;
            for (_int j = 0; j < TILEMODE_CHANGE::MODE_END; ++j)
            {
                for (auto iter : m_vecTileBuffer[i][j])
                {
                    if (dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_Spawner() == TILE_SPAWNER::MONSTER_SPAWN1 ||
                        dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_Spawner() == TILE_SPAWNER::MONSTER_SPAWN2 ||
                        dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_Spawner() == TILE_SPAWNER::MONSTER_SPAWN3 ||
                        dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_Spawner() == TILE_SPAWNER::MONSTER_SPAWN4 ||
                        dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_Spawner() == TILE_SPAWNER::BOSS_SPAWN)
                        ++iCnt;

                }
            }
            m_StageCntArray[i] = iCnt;
        }
    }
public:
    HRESULT                 Add_Tile(GameObject* pObject, _vec3 vPos, TILE_STAGE eStage,TILEMODE_CHANGE eMode, TILE_SIDE eSid = TILE_SIDE::TILE_OTHER,_vec3 PivotHeight = { 0,0,0 }, _bool bAni=false);
    void                    Delete_Tile(_vec3 vPos, _vec3 Origin, _vec3 vDir);
  
    HRESULT                 Stage_Update(const _float& fTimeDelta);
    void                    Stage_LateUpdate(const _float& fTimeDelta);
    void                    Stage_Render();

   
    HRESULT                 Update_TileList(const _float& fTimeDetla);
    void                    LateUpdate_Tile(const _float& fTimeDelta);
    void                    Render_TileList();
    
    void                    Set_TileMode(TILEMODE_CHANGE eMode) { m_eMode = eMode; }
    _int                    Set_StageArray() { return --m_StageCntArray[m_eStage]; }
    void                    Set_Trigger(TILE_STAGE eStage, TILEMODE_CHANGE eMode, TILE_STATE eState);

    TILE_STAGE              Get_Stage() { return m_eStage; };
    TILEMODE_CHANGE         Get_Mode() { return m_eMode; }

    void                    Save_Tile(HWND g_hWnd);
    void                    Load_TilePush(GameObject* pGame,TILE_STAGE eStage, TILEMODE_CHANGE eMod) { m_vecTileBuffer[eStage][eMod].push_back(pGame); }

    _bool                   Choice_Tile(_int* eState, _int* eMode, _int* iTileNumber, _vec3 Origin, _vec3 vDir, _vec3* returnPos, _vec3* returnScale, _vec3* returnRot, _bool* bAni);
    void                    Set_Tile(_vec3 vPos, _vec3 returnPos, _vec3 returnRot, _int eStage, _int eMode, _int TileNumber, _bool bAni);
    void                    Move_Tile(_vec3 vPos, _vec3 Origin, _vec3 vDir);
    void                    Change_Stage(TILE_STAGE eStage) { m_eCurrent = eStage; m_bStageChange = true; }
    void                    Reset_TileList();
    _int                    Get_StageCnt() { return m_StageCntArray[m_eCurrent]; }
private:
    _bool                   m_bStageChange;
    _bool                   m_bCheck;
    TILE_STAGE              m_eCurrent;
    TILE_STAGE              m_eStage;
    TILEMODE_CHANGE         m_eMode;
    int                     m_StageCntArray[TILE_STAGE::STAGE_END];
    vector<GameObject*>     m_vecTileBuffer[TILE_STAGE::STAGE_END][TILEMODE_CHANGE::MODE_END];

private:
    virtual void            Free();
};

END