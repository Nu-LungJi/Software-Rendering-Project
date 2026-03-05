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
    m_fRadian += _DT;

    TESSERACTVERTEX* pVertex = nullptr;
    if (SUCCEEDED(m_fVertexBuffer->Lock(0, 0, (void**)&pVertex, 0))) {
        float fCos = cosf(m_fRadian), fSin = sinf(m_fRadian);
        float fDistance = 4.0f;

        for (int i = 0; i < 16; i++) {
            D3DXVECTOR4 vRot;
            vRot.x = m_fVertices4D[i].x * fCos - m_fVertices4D[i].w * fSin;
            vRot.y = m_fVertices4D[i].y;
            vRot.z = m_fVertices4D[i].z;
            vRot.w = m_fVertices4D[i].x * fSin + m_fVertices4D[i].w * fCos;

            float fWScale = 1.0f / (fDistance - vRot.w);

            pVertex[i].pos.x = vRot.x * fWScale * m_fScale;
            pVertex[i].pos.y = vRot.y * fWScale * m_fScale;
            pVertex[i].pos.z = vRot.z * fWScale * m_fScale;
            pVertex[i].color = D3DCOLOR_ARGB(100, 0, 255, 255);
        }
        m_fVertexBuffer->Unlock();
    }

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return GameObject::Update_GameObject(_DT);
}
VOID Tesseract::LateUpdate_GameObject(const _float& _DT) {

}

VOID Tesseract::Render_GameObject()
{
    GRPDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    GRPDEV->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    GRPDEV->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    GRPDEV->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());
    GRPDEV->SetTexture(0, m_pTexture);

    GRPDEV->SetStreamSource(0, m_fVertexBuffer, 0, sizeof(TESSERACTVERTEX));
    GRPDEV->SetIndices(m_fIndexBuffer);
    GRPDEV->SetFVF(D3DFVF_TESSERACTVERTEX);

    GRPDEV->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 16, 0, 48);

    GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT Tesseract::Component_Initialize() {
    m_pTransform = ADD_COMPONENT_TRANSFORM;
    m_pTransform->Set_Pos(-10.f, 0.f, 5.f);
    m_pTransform->Set_Scale(1.f, 1.f, 1.f);

    m_pCollider = ADD_COMPONENT_COLLIDER;
    m_pCollider->Set_CenterPos(m_pTransform);
    m_pCollider->Set_Scale(6.f, 6.f, 6.f);

    m_pTexture = ResourceManager::GetInstance()->Find_Texture(L"Cyan.png");

    for (int i = 0; i < 16; i++) {
        m_fVertices4D[i] = D3DXVECTOR4((i & 1) ? 1.f : -1.f, (i & 2) ? 1.f : -1.f,
            (i & 4) ? 1.f : -1.f, (i & 8) ? 1.f : -1.f);
    }

    GRPDEV->CreateVertexBuffer(16 * sizeof(TESSERACTVERTEX), D3DUSAGE_WRITEONLY,
        D3DFVF_TESSERACTVERTEX, D3DPOOL_MANAGED, &m_fVertexBuffer, NULL);

    GRPDEV->CreateIndexBuffer(144 * sizeof(WORD), D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_fIndexBuffer, NULL);

    WORD* pIndices = nullptr;
    if (SUCCEEDED(m_fIndexBuffer->Lock(0, 0, (void**)&pIndices, 0))) {
        int iIdx = 0;
        for (int i = 0; i < 16; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = j + 1; k < 4; ++k) {
                    int v1 = i;
                    int v2 = i ^ (1 << j);
                    int v3 = i ^ (1 << k);
                    int v4 = i ^ (1 << j) ^ (1 << k);

                    if (v1 < v2 && v1 < v3 && v1 < v4) {
                        pIndices[iIdx++] = (WORD)v1; pIndices[iIdx++] = (WORD)v2; pIndices[iIdx++] = (WORD)v4;
                        pIndices[iIdx++] = (WORD)v1; pIndices[iIdx++] = (WORD)v4; pIndices[iIdx++] = (WORD)v3;
                    }
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