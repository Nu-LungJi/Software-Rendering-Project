#pragma once
#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL ResourceManager : public Base {
	DECLARE_SINGLETON(ResourceManager)
private:
	explicit ResourceManager();
	virtual ~ResourceManager();
public:
	HRESULT					GlobalImport_Texture(IDirect3DDevice9* _GRPDEV, wstring _MasterFolder);
	HRESULT					PathFinder(IDirect3DDevice9* _GRPDEV, wstring _MasterFolder);
	unordered_map<CONST TCHAR*, IDirect3DTexture9*>& Get_Map() { return TextureList; }
	IDirect3DTexture9* Find_Texture(wstring _FileName);

private:
	unordered_map<CONST TCHAR*, IDirect3DTexture9*>		TextureList;
	vector<wstring*>							KEY_Array;
public:
	virtual			VOID	Free();
};

END