#include "UIManager.h"

IMPLEMENT_SINGLETON(UIManager)

UIManager::UIManager() : DXSprite(nullptr), isActive(FALSE) {}
UIManager::~UIManager() { Free(); }

HRESULT UIManager::Ready_UIManager(LPDIRECT3DDEVICE9 _GRPDEV) {
    
    if (FAILED(D3DXCreateSprite(_GRPDEV, &DXSprite)))   return E_FAIL;

	return S_OK;
}
INT UIManager::Update_UIManager(const FLOAT& _DT) {
	return 0;
}
VOID UIManager::LateUpdate_UIManager(const FLOAT& _DT) {
}
VOID UIManager::Render_UIManager(LPDIRECT3DDEVICE9 _GRPDEV) {
}

FontObject* UIManager::Add_FontSprite(LPDIRECT3DDEVICE9 _GRPDEV, wstring _Text, _vec2 _Position, _int _TextScale, wstring _FontTag, wstring _FontType, D3DCOLOR _Color, _int TextWeight, BOOL _Visible, DWORD FORMAT) {

    FontObject* FO = new FontObject(_Position, _Text, _TextScale, TextWeight, _FontTag, _FontType, _Color, _Visible, FORMAT);

    D3DXFONT_DESCW FontInfo;
    ZeroMemory(&FontInfo, sizeof(FontInfo));

    FontInfo.Height = FO->TextScale;
    FontInfo.Weight = FO->TextWeight;
    FontInfo.CharSet = HANGUL_CHARSET;
    FontInfo.OutputPrecision = OUT_DEFAULT_PRECIS;
    FontInfo.Quality = DEFAULT_QUALITY;
    FontInfo.PitchAndFamily = DEFAULT_PITCH | FW_DONTCARE;

    lstrcpyW(FontInfo.FaceName, FO->FontType.c_str());

    if (FAILED(D3DXCreateFontIndirectW(_GRPDEV, &FontInfo, &FO->DXFont))) {
        MSG_BOX("Cannot Create FontObject.");
        return nullptr;
    }

    FontList.insert({ FO->FontTag.c_str(), FO });

    return FO;
}
FontObject* UIManager::Find_FontObject(wstring _Text) {
    auto iter = find_if(FontList.begin(), FontList.end(), CTag_Finder(_Text.c_str()));
    if (iter == FontList.end())	return nullptr;
    return iter->second;
}
ItemINFO* UIManager::Find_Item(wstring _TAG) {
    auto iter = find_if(ItemList.begin(), ItemList.end(), CTag_Finder(_TAG.c_str()));
    if(iter == ItemList.end()) return nullptr;
    return iter->second;
}

VOID UIManager::Render_FontObjects() {
    DXSprite->Begin(D3DXSPRITE_ALPHABLEND);

    for (auto& TXT : FontList) {
        if (TXT.second->Visible == TRUE) {
            FLOAT XPos = TXT.second->Position.x;
            FLOAT YPos = TXT.second->Position.y;
            RECT RT = { XPos, YPos, XPos + 1, YPos + 1 };
            TXT.second->DXFont->DrawTextW(DXSprite, TXT.second->Text.c_str(), -1, &RT, TXT.second->FORMAT | DT_NOCLIP, TXT.second->TextColor);
        }
    }

    DXSprite->End();
}

VOID UIManager::Free() {
    for (auto& Item : ItemList) 
        Safe_Delete(Item.second);
    for (auto& FO : FontList)
        Safe_Delete(FO.second);
    Safe_Release(DXSprite);
    ItemList.clear();
}