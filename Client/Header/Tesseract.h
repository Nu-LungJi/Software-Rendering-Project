#pragma once
#include "GameObject.h"

struct TESSERACTVERTEX {
    D3DXVECTOR3     pos;
    DWORD           color;
};
#define D3DFVF_TESSERACTVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

class Tesseract :
    public GameObject
{
private:
    explicit Tesseract();
    explicit Tesseract(LPDIRECT3DDEVICE9 _GRPDEV);
    explicit Tesseract(const GameObject& ths);
    virtual ~Tesseract();

public:

    virtual			HRESULT		Ready_GameObject();
    virtual			INT			Update_GameObject(CONST FLOAT& _DT);
    virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
    virtual			VOID		Render_GameObject();

    
private:
    HRESULT			Component_Initialize();

    LPDIRECT3DVERTEXBUFFER9 m_fVertexBuffer = nullptr;
    LPDIRECT3DINDEXBUFFER9  m_fIndexBuffer = nullptr;
    D3DXVECTOR4     m_fVertices4D[16];
    float           m_fRadian           = 0.0f;
    const float     m_fScale            = 12.5f;
    IDirect3DTexture9* m_pTexture = nullptr;
    Transform*      m_pTransform = nullptr;
    Collider*       m_pCollider = nullptr;
public:
    static         Tesseract* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual  void            Free();

};

