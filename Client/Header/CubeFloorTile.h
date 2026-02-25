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
    //int             Get_TileNumber()                    { return m_iTileNumber; }
    //void            Set_TileNumber(_int iTileNumber)    { m_iTileNumber = iTileNumber; }
    //void            Set_TileState(TILE_STATE eid)       { m_eTileState = eid; }

private:
    HRESULT			Component_Initialize();
    //TILE_STATE      m_eTileState;
    //TILE_SIDE       m_eTileSide;

    const  _tchar*      m_pTileName;

    Buffer*             m_pBuffer;
    Transform*          m_pTransform;
    Collider*           m_pCollider;

    //Texture*            m_pTexture;
    //int                 m_iTileNumber;
public:
    static         CubeFloorTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, bool _Grid = true);
    static         CubeFloorTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, bool _Grid = true);
    static         CubeFloorTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale, bool _Grid = true);
    //static         CubeFloorTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, TILE_SIDE eid = TILE_SIDE::TILE_END, TILE_STATE eState = TILE_STATE::STATE_NORMAL, FLOAT& X1, FLOAT& X2, FLOAT& Y1, FLOAT& Y2);

    bool           m_bGrid;

private:
    virtual  void            Free();


private:
    CameraObject*   m_pCam = nullptr;
    bool            m_bTrigger = false;
    bool            IsIn_Cam = true;
    _float          m_fTimer = 0.f;
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