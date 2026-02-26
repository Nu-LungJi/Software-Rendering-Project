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

    explicit FilenameInfo(const std::wstring& _Filename)
        : usResult(0xffff), Type(0x00), name(0x00), State(0x00), FrameNum(0), iCount(0)//, extension(0x00),
    {
        memset(szType, 0, sizeof(szType));
        memset(szName, 0, sizeof(szName));
        memset(szState, 0, sizeof(szState));
        memset(szExtension, 0, sizeof(szExtension));
        
        Fullname = _Filename;
        iCount = swscanf_s(_Filename.c_str(), L"%*[^_]_%[^_]_%[^_]_%[^_]_%hhu%s",
            szType,         (unsigned)_countof(szType),
            szName,         (unsigned)_countof(szName),
            szState,        (unsigned)_countof(szState),
            &FrameNum,
            szExtension,    (unsigned)_countof(szExtension));
    }
}FILENAMEINFO;

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
};

END