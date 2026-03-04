#pragma once
#include "Base.h"
#include "Engine_Define.h"

#include <filesystem>

#include "GameObject.h"

#include "Buffer.h"
#include "Transform.h"
#include "Texture.h"
#include "Collider.h"

BEGIN(Engine)

typedef struct FilenameInfo {
    uint16_t    usResult;
    uint8_t     extension;
    uint8_t     Type;
    uint8_t     name;
    uint8_t     State;
    uint8_t     FrameNum;
    int         iCount;

    wstring     Fullname;
    TCHAR szType[64];
    TCHAR szName[256];
    TCHAR szState[64];
    TCHAR szExtension[16];

    void Reset() {
        memset(szType, 0, sizeof(szType));
        memset(szName, 0, sizeof(szName));
        memset(szState, 0, sizeof(szState));
        memset(szExtension, 0, sizeof(szExtension));
    }

    explicit FilenameInfo(const std::wstring& _Filename)
        : usResult(0xffff), Type(0x00), name(0x00), State(0x00), FrameNum(0), iCount(0)//, extension(0x00),
    {
        Reset();

        Fullname = _Filename;
        iCount = swscanf_s(_Filename.c_str(), L"%*[^_]_%[^_]_%[^_]_%[^_]_%hhu%s",
            szType, (unsigned)_countof(szType),
            szName, (unsigned)_countof(szName),
            szState, (unsigned)_countof(szState),
            &FrameNum,
            szExtension, (unsigned)_countof(szExtension));

        if (iCount < 4) {
            Reset();

            iCount = swscanf_s(_Filename.c_str(), L"%*[^_]_%[^_]_%[^_]_%hhu%s",
                szType, (unsigned)_countof(szType),
                szName, (unsigned)_countof(szName),
                &FrameNum,
                szExtension, (unsigned)_countof(szExtension));
        }
    }
}FILENAMEINFO;

typedef struct tagHurdleInfo {
    _vec3   vSrc;
    _vec3   vDst;
    _vec3   vPos;
    _vec3   vDir;
    _float  fDis;
    _float  fSpeed;
    _float  fScale;
    uint8_t VisibleCount;
    uint8_t RefCount;

    explicit    tagHurdleInfo(_vec3 _vSrc, _vec3 _vDst, _float _fSpeed, _float _fScale) : VisibleCount(0), RefCount(0)
    {
        vSrc = _vSrc;
        vDst = _vDst;
        vPos = (vSrc + vDst) * 0.5f;
        vDir = _vDst - vSrc;
        fDis = D3DXVec3Length(&vDir);
        D3DXVec3Normalize(&vDir, &vDir);
        fSpeed = _fSpeed;
        fScale = _fScale;
    }

    bool operator==(const tagHurdleInfo& other) const {
        return (vSrc == other.vSrc &&
            vDst == other.vDst &&
            fSpeed == other.fSpeed &&
            fScale == other.fScale);
    }
};


class ENGINE_DLL MonsterManager : public Base
{
    DECLARE_SINGLETON(MonsterManager);
public:
    explicit MonsterManager();
    virtual ~MonsterManager();

public:
    void		    Load_Textures_from_Folder(IDirect3DDevice9* _GRPDEV, const wstring& _Filepath);
    FILENAMEINFO	Make_ID_from_Filename(const wstring& _Filename);

    static  uint16_t    Make_Key(uint8_t eType, uint8_t eName, uint8_t eState);
    static  uint16_t    Update_Key(uint16_t Key, uint8_t eState);
    const   vector<IDirect3DTexture9*>* Find_Textures(uint16_t uiID);

private:
    void Free() override;

private:
    map<uint16_t, vector<IDirect3DTexture9*>>	    mapProtoType;

public:
    vector<tagHurdleInfo*>* Get_Hurdles() { return &vecHurdles; }
private:
    vector<tagHurdleInfo*>  vecHurdles;


public:
    HRESULT                     Ready_Origin_Buffer(Buffer* _pBuffer);
    HRESULT                     Ready_Origin_Buffer();
    vector<GameObject*>* Get_Tiles() { return &m_vecTiles; }
    LPDIRECT3DVERTEXBUFFER9* TILEVB() { return &m_pTileVB; }
    LPDIRECT3DINDEXBUFFER9* TILEIB() { return &m_pTileIB; }

    void    Set_Maxtile(_uint _Num) { m_uiMaxTile = _Num; }
    HRESULT Ready_Static_Batch(LPDIRECT3DDEVICE9 _GRPDEV);
    void    Release_Static_Batich();
    void    Render_Static_Batch(LPDIRECT3DDEVICE9 GRPDEV, LPDIRECT3DTEXTURE9 Texture);

    void    Update_Tile(_uint _Num, Transform* TransCom);

private:
    IDirect3DTexture9* m_pTileTex;

    _uint                   m_uiMaxTile = 0;
    LPDIRECT3DVERTEXBUFFER9 m_pTileVB = nullptr;
    LPDIRECT3DINDEXBUFFER9  m_pTileIB = nullptr;

    vector<GameObject*>     m_vecTiles;

    VTXTRUECUBE          m_vOriginVtx[24];
    INDEX32              m_vOriginIdx[12];
};

END