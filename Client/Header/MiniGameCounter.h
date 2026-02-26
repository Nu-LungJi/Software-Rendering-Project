#pragma once
#include "GameObject.h"

class MiniGameCounter :
    public GameObject
{
private:
    explicit MiniGameCounter(LPDIRECT3DDEVICE9 _GRPDEV);
    explicit MiniGameCounter(const GameObject& _RHS);
    virtual ~MiniGameCounter();

public:

    virtual			HRESULT		Ready_GameObject( );
    virtual			INT			Update_GameObject(CONST FLOAT& _DT);
    virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
    virtual			VOID		Render_GameObject();

public:
    void                    Frame_Move(CONST FLOAT& _DT);
    Transform*              Crash_Player();


    void        Set_Count() { --m_StageCnt[m_iCnt]; }
    _int        Get_Stage() { return m_iCnt;}
private:
    HRESULT			Component_Initialize();

    TileInfo* m_pTileInfo;
    Transform* m_pTransform;
    Buffer* m_pBuffer;
    void                        Imgui_Setting();
    void                        Imgui();
    void                        Imgui_ButtonStyle();
private:


    _float                  m_fFrame;
    _float                  m_fTime;
    _float                  m_fDefense;
    _bool                   m_bStopFrame;

    _int                    m_iCnt;
    _int                    m_StageCnt[3];
public:
    static         MiniGameCounter* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual  void            Free();

};

