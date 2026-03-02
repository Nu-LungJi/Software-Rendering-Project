#pragma once
#include "GameObject.h"

class CameraObject;

class CubeFloorTile :
    public GameObject
{
private:
    explicit CubeFloorTile();
    explicit CubeFloorTile(LPDIRECT3DDEVICE9 _GRPDEV);
    explicit CubeFloorTile(const GameObject& ths);
    virtual ~CubeFloorTile();

public:

    virtual			HRESULT		Ready_GameObject();
    virtual			INT			Update_GameObject(CONST FLOAT& _DT);
    virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
    virtual			VOID		Render_GameObject();

    //이건.. 지형이 가지고 있는 타일의 인덱스 정보를 담기위함 해당 인덱스가 가진 위치위에 이 타일이 놓여있다는걸 확인하기 위해서
    int             Get_TileNumber()                    { return m_iTileNumber; }
    void            Set_TileNumber(_int iTileNumber) {
        m_iTileNumber = iTileNumber;
        if (m_pTransform != nullptr) m_pTransform->Get_Position()->y += 0.0001f * iTileNumber;
    }
    //void            Set_TileState(TILE_STATE eid)       { m_eTileState = eid; }
    Transform* Get_TransCom() { return m_pTransform; }
    Buffer* Get_Buffer() { return m_pBuffer; }

private:
    HRESULT			Component_Initialize();

    int m_iTileNumber = 0;

    Buffer*             m_pBuffer;
    Buffer*             m_pOriginBuffer;
    Transform*          m_pTransform;
    Collider*           m_pCollider;

public:
    static         CubeFloorTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, bool _Grid = true);
    static         CubeFloorTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, bool _Grid = true);
    static         CubeFloorTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale, bool _Grid = true);

    BOOL			OnCollisionEnter(GameObject* _Other)	override;
    bool           m_bGrid;

private:
    virtual  void            Free();

    bool    Pooling();
private:
    CameraObject*   m_pCam = nullptr;
    bool            m_bTrigger = false;
    bool            IsIn_Cam = true;
    _float          m_fTimer = 0.f;
    IDirect3DTexture9* m_pTexture;
};

class CubeFunction
{
public:
    static HRESULT      Grid(Transform* _Transform, bool _Grid, float _SnapAngle = 15.f)
    {
        if (_Transform == nullptr)
            return E_POINTER;

        if (_SnapAngle == 0.f)
            return  E_FAIL;

        float SnapDgree = (fabsf(_SnapAngle) < 360.f) ? _SnapAngle :
                           fmodf(_SnapAngle, 360.f);
        SnapDgree += (SnapDgree < 0.f) * 360;

        if (_Grid)
        {
            _vec3* vPos = _Transform->Get_Position();

            vPos->x = roundf(vPos->x);
            vPos->y = roundf(vPos->y);
            vPos->z = roundf(vPos->z);

            vPos = _Transform->Get_Scale();

            vPos->x = roundf(vPos->x);
            vPos->y = roundf(vPos->y);
            vPos->z = roundf(vPos->z);

            vPos = _Transform->Get_Rotation();

            vPos->x = roundf(vPos->x / SnapDgree) * SnapDgree;
            vPos->y = roundf(vPos->y / SnapDgree) * SnapDgree;
            vPos->z = roundf(vPos->z / SnapDgree) * SnapDgree;

            return S_OK;
        }
        else
            return E_NOTIMPL;
    }


};