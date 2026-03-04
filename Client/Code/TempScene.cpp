#include "../Include/pch.h"

TempScene::TempScene(LPDIRECT3DDEVICE9 _GRPDEV) : Scene(_GRPDEV) {}
TempScene::~TempScene() {}

HRESULT   TempScene::Ready_Scene() {
   
    return S_OK;
}
INT    TempScene::Update_Scene(CONST FLOAT& _DT) {
    return S_OK;
}
VOID TempScene::LateUpdate_Scene(CONST FLOAT& _DT) {

}
VOID TempScene::Render_Scene() {

}
HRESULT TempScene::Ready_Enviroment_Layer() {
   return S_OK;
}
HRESULT TempScene::Ready_GameLogic_Layer() {
   return S_OK;
}
HRESULT TempScene::Ready_UserInterface_Layer() {
   
    return S_OK;
}
TempScene* TempScene::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
    TempScene* LS = new TempScene(_GRPDEV);

    SceneManager::GetInstance()->Set_CurrentScene(LS);

    if (FAILED(LS->Ready_Scene())) {
        MSG_BOX("Cannot Create TempScene.");
        Safe_Release(LS);
        return nullptr;
    }
    return LS;
}
void TempScene::Free() {
    Safe_Release(pLoading);
    Scene::Free();
}