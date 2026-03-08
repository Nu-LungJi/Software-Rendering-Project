#pragma once
#include "GameObject.h"

class MiniGameDis : public GameObject
{
private:
    explicit MiniGameDis(LPDIRECT3DDEVICE9 _GRPDEV);
    explicit MiniGameDis(const GameObject& _RHS);
    virtual ~MiniGameDis();

public:
    virtual			HRESULT		Ready_GameObject();
    virtual			INT			Update_GameObject(CONST FLOAT& _DT);
    virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
    virtual			VOID		Render_GameObject();

public:
    static         MiniGameDis* Create(LPDIRECT3DDEVICE9 _GRPDEV);

private:
    virtual  void            Free();

private:
    ID3DXSprite* m_pDrawSprite;
    ID3DXFont* m_pFont;
    wstring m_wstrText;

    bool    m_bSound[2];

    _vec2   m_vStartPos;
    _vec2   m_vEndPos;
    
    _float  m_fTimer;
    _float  m_fElapsed;
    _float  m_fDuration;
};

