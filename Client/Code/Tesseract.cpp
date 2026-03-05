#include "Tesseract.h"
#include "../Include/PCH.h"

Tesseract::Tesseract(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Tesseract::Tesseract(const GameObject& _RHS) : GameObject(_RHS) {}
Tesseract::~Tesseract() {}

HRESULT Tesseract::Ready_GameObject() {

	if (FAILED(Component_Initialize())) return E_FAIL;

	return S_OK;
}
INT	Tesseract::Update_GameObject(const _float& _DT) {
	_vec3 vEye = *Monster::Get_Camera()->Get_EyeVec();
	_vec3 vPos = *m_pTransform->Get_Position();
	_vec3 vDiff = vEye - vPos;
	_float fDis = D3DXVec3Length(&vDiff);
	if (fDis < 4.f)	fDis = 4.f;

	m_fRadian += _DT;

    TESSERACTVERTEX* pVertex;
    if (SUCCEEDED(m_fVertexBuffer->Lock(0, 0, (void**)&pVertex, 0))) {
        float fCos = cosf(m_fRadian), fSin = sinf(m_fRadian);
        float fDistance = 4.0f;

        for (int i = 0; i < 16; i++) {
            D3DXVECTOR4 v = m_fVertices4D[i];
            D3DXVECTOR4 vRot;

            vRot.x = v.x * fCos - v.w * fSin;
            vRot.y = v.y;
            vRot.z = v.z;
            vRot.w = v.x * fSin + v.w * fCos;

            float fWScale = 1.0f / (fDistance - vRot.w);

			pVertex[i].pos.x = vRot.x * fWScale * m_fScale;
			pVertex[i].pos.y = vRot.y * fWScale * m_fScale;
			pVertex[i].pos.z = vRot.z * fWScale * m_fScale;
			pVertex[i].color = 0xFFFFFFFF;
        }
		m_fVertexBuffer->Unlock();
    }
	
	RenderManager::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return 0;
}
VOID Tesseract::LateUpdate_GameObject(const _float& _DT) {

}

VOID Tesseract::Render_GameObject()
{
	GRPDEV->SetStreamSource(0, m_fVertexBuffer, 0, sizeof(TESSERACTVERTEX));
	GRPDEV->SetIndices(m_fIndexBuffer);
	GRPDEV->SetFVF(D3DFVF_TESSERACTVERTEX);
	GRPDEV->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 16, 0, 32);
}

HRESULT Tesseract::Component_Initialize() {

	m_pTransform = ADD_COMPONENT_TRANSFORM;

	for (int i = 0; i < 16; i++) {
		m_fVertices4D[i] = D3DXVECTOR4((i & 1) ? 1.f : -1.f, (i & 2) ? 1.f : -1.f,
				                      (i & 4) ? 1.f : -1.f, (i & 8) ? 1.f : -1.f);
    }

	GRPDEV->CreateVertexBuffer(16 * sizeof(TESSERACTVERTEX), D3DUSAGE_WRITEONLY,
							D3DFVF_TESSERACTVERTEX, D3DPOOL_MANAGED, &m_fVertexBuffer, NULL);
	GRPDEV->CreateIndexBuffer(64 * sizeof(WORD), D3DUSAGE_WRITEONLY,
							D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_fIndexBuffer, NULL);

	WORD* pIndices;
	if (SUCCEEDED(m_fVertexBuffer->Lock(0, 0, (void**)&pIndices, 0))) {
		int count = 0;
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 4; j++) {
				int neighbor = i ^ (1 << j);
				if (neighbor > i) {
					pIndices[count++] = (WORD)i;
					pIndices[count++] = (WORD)neighbor;
				}
			}
		}
		m_fIndexBuffer->Unlock();
	}
	return S_OK;
}

Tesseract* Tesseract::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Tesseract* pTesseract = new Tesseract(_GRPDEV);
	if (FAILED(pTesseract->Ready_GameObject())) {
		MSG_BOX("Cannot Create Tesseract.");
		Safe_Release(pTesseract);
		return nullptr;
	}
	return pTesseract;
}
VOID Tesseract::Free() {
	GameObject::Free();
}