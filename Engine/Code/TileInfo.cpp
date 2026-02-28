#include "TileInfo.h"

TileInfo::TileInfo(LPDIRECT3DDEVICE9 _GRPDEV) : Component(_GRPDEV), m_eTileSpawner(TILE_SPAWNER::SPAWN_END),m_iTileNumber(0), m_pTexture(nullptr),
m_eTileState(TILE_STATE::STATE_END), m_bPortal(false), m_bOnlyAnimation(false), m_iTextureCount(0), m_eTileSide(TILE_SIDE::TILE_END), m_eTileMode(TILEMODE_CHANGE::MODE_END) {
}
TileInfo::TileInfo(const TileInfo& _RHS) : Component(_RHS),m_bOnlyAnimation(_RHS.m_bOnlyAnimation), m_eNextStage(_RHS.m_eNextStage), m_eTileStage(_RHS.m_eTileStage), m_eTileSide(_RHS.m_eTileSide), m_bPortal(_RHS.m_bPortal), m_pTexture(_RHS.m_pTexture),
m_eTileState(_RHS.m_eTileState), m_eTileMode(_RHS.m_eTileMode),m_iTextureCount(_RHS.m_iTextureCount), m_pTileName(_RHS.m_pTileName), m_iTileNumber(_RHS.m_iTileNumber) {
}
TileInfo::~TileInfo() { }

HRESULT TileInfo::Ready_Component() {

	return S_OK;
}
INT	TileInfo::Update_Component(const _float& _DT) {

	
	return 0;
}
VOID TileInfo::LateUpdate_Component(const _float& _DT) {

}

TileInfo* TileInfo::Create(LPDIRECT3DDEVICE9 _GRPDEV) {


	TileInfo* pTileInfo = new TileInfo(_GRPDEV);

	if (FAILED(pTileInfo->Ready_Component())) {
		MSG_BOX("Cannot Create TileInfo.");
		Safe_Release(pTileInfo);
		return nullptr;
	}

	return pTileInfo;
}

Component* TileInfo::Clone() {
	return new TileInfo(*this);
}
VOID TileInfo::Free() {

	Component::Free();
}