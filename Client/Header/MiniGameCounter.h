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
    _bool        Get_End() { return m_bEnd; }
private:
    HRESULT			Component_Initialize();
    HRESULT         Make_TextureList(wstring _FileName);

    TileInfo* m_pTileInfo;
    Transform* m_pTransform;
    Buffer* m_pBuffer;
private:
    

    _float                  m_fFrame;
    _float                  m_fTime;
    _float                  m_fDefense;
    _bool                   m_bStopFrame;
    _bool                   m_bEnd;
    _bool                   m_bDead;
    _int                    m_iCnt;
    _int                    m_StageCnt[3];
    _int                    m_iKeyCnt;

    ID3DXSprite*            Sprite;
    SpriteObject*           m_pSprite;
    vector<wstring>         m_vecKeyList;
public:
    static         MiniGameCounter* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual  void            Free();

};

