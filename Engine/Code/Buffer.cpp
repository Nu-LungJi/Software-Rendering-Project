#include "Buffer.h"

Buffer::Buffer() :	VertexBuffer(nullptr), IndexBuffer(nullptr), pVBufferPos(nullptr), VertexSize(0), VertexCount(0), VertexFormat(0), TRICount(0), IndexSize(0) {}
Buffer::Buffer(LPDIRECT3DDEVICE9 _GRPDEV) : Component(_GRPDEV), 
					VertexBuffer(nullptr), IndexBuffer(nullptr), pVBufferPos(nullptr),  VertexSize(0), VertexCount(0), VertexFormat(0), TRICount(0), IndexSize(0) {}
Buffer::Buffer(const Buffer& _RHS)		  : Component(_RHS), 
					VertexBuffer(_RHS.VertexBuffer), IndexBuffer(_RHS.IndexBuffer),VertexSize(_RHS.VertexSize), VertexCount(_RHS.VertexCount), 
					VertexFormat(_RHS.VertexFormat), TRICount(_RHS.TRICount), IndexSize(_RHS.IndexSize),pVBufferPos(_RHS.pVBufferPos) { VertexBuffer->AddRef(); IndexBuffer->AddRef(); }
Buffer::~Buffer() {}

HRESULT Buffer::Ready_Buffer(BUFFER_TYPE _TYPE) {
	if		(_TYPE == BUFFER_TYPE::TRIANGLE	)		{ Ready_Triangle_Buffer() ;	}
	else if (_TYPE == BUFFER_TYPE::RECTANGLE)		{ Ready_Rectangle_Buffer(); }
	else if (_TYPE == BUFFER_TYPE::TEXTURE	)		{ Ready_Texture_Buffer()  ;	}
	else if (_TYPE == BUFFER_TYPE::TERRAIN	)		{ Ready_Terrain_Buffer()  ;	}
    else if (_TYPE == BUFFER_TYPE::TILE)			{ Ready_Tile_Buffer()	  ; }
    else if (_TYPE == BUFFER_TYPE::CUBE)			{ Ready_Cube_Buffer()	  ; }
	else if (_TYPE == BUFFER_TYPE::TILEFRONT)		{ Ready_TileFRONT_Buffer(); }
	else if (_TYPE == BUFFER_TYPE::TILELEFT)		{ Ready_TileLEFT_Buffer(); }
	else if (_TYPE == BUFFER_TYPE::TILERIGHT)		{ Ready_TileRIGHT_Buffer(); }
	else if (_TYPE == BUFFER_TYPE::TILEBACK)		{ Ready_TileBACK_Buffer(); }
	return S_OK;
}
VOID	Buffer::Render_Buffer() {
	GRPDEV->SetStreamSource(0, VertexBuffer, 0, VertexSize);
	GRPDEV->SetFVF(VertexFormat);

	GRPDEV->SetIndices(IndexBuffer);
	GRPDEV->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, VertexCount, 0, TRICount);
}
HRESULT Buffer::Ready_Triangle_Buffer() {
	VertexSize = sizeof(VTXCOL);
	VertexCount = 3;
	TRICount = 1;
	VertexFormat = FVF_COL;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;

	VTXCOL* Vertex = NULL;
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;
	VertexBuffer->Lock(0, 0, (void**)&Vertex, 0);

	Vertex[0].vPosition = { 0.f, 1.f, 0.f };
	Vertex[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	Vertex[1].vPosition = { 1.f, -1.f, 0.f };
	Vertex[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	Vertex[2].vPosition = { -1.f, -1.f, 0.f };
	Vertex[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	VertexBuffer->Unlock();

	INDEX32* Index = nullptr;

	IndexBuffer->Lock(0, 0, (void**)&Index, 0);

	Index[0]._0 = 0;
	Index[0]._1 = 1;
	Index[0]._2 = 2;

	IndexBuffer->Unlock();
	return S_OK;
}
HRESULT Buffer::Ready_Rectangle_Buffer() {
	VertexSize = sizeof(VTXCOL);
	VertexCount = 4;
	TRICount = 2;
	VertexFormat = FVF_COL;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;

	VTXCOL* Vertex = NULL;
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;
	VertexBuffer->Lock(0, 0, (void**)&Vertex, 0);

	Vertex[0].vPosition = { -1.f, 1.f, 0.f };
	Vertex[0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	Vertex[1].vPosition = { 1.f, 1.f, 0.f };
	Vertex[1].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	Vertex[2].vPosition = { 1.f, -1.f, 0.f };
	Vertex[2].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	Vertex[3].vPosition = { -1.f, -1.f, 0.f };
	Vertex[3].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	VertexBuffer->Unlock();

	INDEX32* Index = nullptr;

	IndexBuffer->Lock(0, 0, (void**)&Index, 0);

	Index[0]._0 = 0;
	Index[0]._1 = 1;
	Index[0]._2 = 2;
	
	Index[1]._0 = 0;
	Index[1]._1 = 2;
	Index[1]._2 = 3;

	IndexBuffer->Unlock();
	return S_OK;
}
HRESULT Buffer::Ready_Texture_Buffer() {
	VertexSize = sizeof(VTXTEX);
	VertexCount = 4;
	TRICount = 2;
	VertexFormat = FVF_TEX;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;

	VTXTEX* Vertex = NULL;
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;
	VertexBuffer->Lock(0, 0, (void**)&Vertex, 0);

	Vertex[0].vPosition = { -1.f, 1.f, 0.f };
	Vertex[0].vTexUV = { 0.f, 0.f };

	Vertex[1].vPosition = { 1.f, 1.f, 0.f };
	Vertex[1].vTexUV = { 1.f, 0.f };

	Vertex[2].vPosition = { 1.f, -1.f, 0.f };
	Vertex[2].vTexUV = { 1.f, 1.f };

	Vertex[3].vPosition = { -1.f, -1.f, 0.f };
	Vertex[3].vTexUV = { 0.f, 1.f };

	VertexBuffer->Unlock();

	INDEX32* Index = nullptr;

	IndexBuffer->Lock(0, 0, (void**)&Index, 0);

	Index[0]._0 = 0;
	Index[0]._1 = 1;
	Index[0]._2 = 2;

	Index[1]._0 = 0;
	Index[1]._1 = 2;
	Index[1]._2 = 3;

	IndexBuffer->Unlock();
	return S_OK;
}
HRESULT Buffer::Ready_Terrain_Buffer() {
	VertexSize = sizeof(VTXTEX);
	VertexCount = VTXCNTX * VTXCNTZ;
	TRICount = (VTXCNTX - 1) * (VTXCNTZ - 1) * 2;
	VertexFormat = FVF_TEX;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;
	pVBufferPos = new _vec3[VertexCount];
	VTXTEX*		Vertex	= NULL;
	INDEX32*	Index	= NULL;
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;
	VertexBuffer->Lock(0, 0, (void**)&Vertex, 0);

	for (INT Z = 0; Z < VTXCNTZ; ++Z) {
		for (INT X = 0; X < VTXCNTX; ++X) {

			INT INDEX = Z * VTXCNTX + X;

			Vertex[INDEX].vPosition = { (FLOAT)X, 0.f, (FLOAT)Z };
			Vertex[INDEX].vTexUV	= { ((FLOAT)(X) / (VTXCNTX-1)) * 20.f, ((FLOAT)(Z) / (VTXCNTZ - 1)) * 20.f };
			pVBufferPos[INDEX] = Vertex[INDEX].vPosition;
		}
	}

	VertexBuffer->Unlock();
	INT INDEX = 0, TriCount = 0;
	IndexBuffer->Lock(0, 0, (void**)&Index, 0);

	for (INT Z = 0; Z < VTXCNTZ - 1; ++Z) {
		for (INT X = 0; X < VTXCNTX - 1; ++X) {

			INDEX = Z * VTXCNTX + X;

			Index[TriCount]._0 = INDEX + VTXCNTX;
			Index[TriCount]._1 = INDEX + VTXCNTX + 1;
			Index[TriCount]._2 = INDEX + 1;
			TriCount++;

			Index[TriCount]._0 = INDEX + VTXCNTX;
			Index[TriCount]._1 = INDEX + 1;
			Index[TriCount]._2 = INDEX;
			TriCount++;
		}
	}

	IndexBuffer->Unlock();
	return S_OK;
}
HRESULT Buffer::Ready_TileRIGHT_Buffer()
{
	VertexSize = sizeof(VTXCUBE);
	VertexCount = 4;
	TRICount = 2;
	VertexFormat = FVF_CUBE;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;

	VTXCUBE* Vertex = NULL;
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;
	VertexBuffer->Lock(0, 0, (void**)&Vertex, 0);


	Vertex[0].vPosition = { 1.f, 1.f, -1.f };
	Vertex[0].vTexUV = { 0,0 };

	Vertex[1].vPosition = { 1.f, 1.f, 1.f };
	Vertex[1].vTexUV = { 1,0 };

	Vertex[2].vPosition = { 1.f, -1.f, 1.f };
	Vertex[2].vTexUV = { 1,1 };

	Vertex[3].vPosition = { 1.f, -1.f, -1.f };
	Vertex[3].vTexUV = { 0,1 };
	VertexBuffer->Unlock();

	INDEX32* Index = nullptr;

	IndexBuffer->Lock(0, 0, (void**)&Index, 0);

	Index[0]._0 = 0;
	Index[0]._1 = 1;
	Index[0]._2 = 2;

	Index[1]._0 = 0;
	Index[1]._1 = 2;
	Index[1]._2 = 3;

	IndexBuffer->Unlock();
	return S_OK;
}
HRESULT Buffer::Ready_TileFRONT_Buffer()
{

	VertexSize = sizeof(VTXCUBE);
	VertexCount = 4;
	TRICount = 2;
	VertexFormat = FVF_CUBE;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;

	VTXCUBE* Vertex = NULL;
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;
	VertexBuffer->Lock(0, 0, (void**)&Vertex, 0);


	Vertex[0].vPosition = { -1.f, 1.f, 0.f };
	Vertex[0].dwColor = D3DXCOLOR(1.f, 1.f, 1.f,1.f);
	Vertex[0].vTexUV = { 0,0 };
	Vertex[1].vPosition = { 1.f, 1.f, 0.f };
	Vertex[1].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	Vertex[1].vTexUV = { 1,0 };
	Vertex[2].vPosition = { 1.f, -1.f, 0.f };
	Vertex[2].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	Vertex[2].vTexUV = { 1,1 };
	Vertex[3].vPosition = { -1.f, -1.f, 0.f };
	Vertex[3].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	Vertex[3].vTexUV = { 0,1 };
	VertexBuffer->Unlock();

	INDEX32* Index = nullptr;

	IndexBuffer->Lock(0, 0, (void**)&Index, 0);

	Index[0]._0 = 0;
	Index[0]._1 = 1;
	Index[0]._2 = 2;

	Index[1]._0 = 0;
	Index[1]._1 = 2;
	Index[1]._2 = 3;

	IndexBuffer->Unlock();
	return S_OK;
}
HRESULT Buffer::Ready_TileBACK_Buffer()
{
	VertexSize = sizeof(VTXCOL);
	VertexCount = 4;
	TRICount = 2;
	VertexFormat = FVF_COL;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;

	VTXCOL* Vertex = NULL;
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;
	VertexBuffer->Lock(0, 0, (void**)&Vertex, 0);

	Vertex[0].vPosition = { -1.f, 0.f, 1.f };
	Vertex[0].dwColor = D3DXCOLOR(1.0f, 0.f, 0.f, 0.3f);

	Vertex[1].vPosition = { 1.f, 0.f, 1.f };
	Vertex[1].dwColor = D3DXCOLOR(1.0f, 0.f, 1.f, 0.3f);

	Vertex[2].vPosition = { 1.f, 0.f, -1.f };
	Vertex[2].dwColor = D3DXCOLOR(1.0f, 0.f, 0.f, 0.3f);
	
	Vertex[3].vPosition = { -1.f, 0.f, -1.f };
	Vertex[3].dwColor = D3DXCOLOR(1.0f, 1.f, 0.f, 0.3f);
	VertexBuffer->Unlock();

	INDEX32* Index = nullptr;

	IndexBuffer->Lock(0, 0, (void**)&Index, 0);

	Index[0]._0 = 0;
	Index[0]._1 = 1;
	Index[0]._2 = 2;

	Index[1]._0 = 0;
	Index[1]._1 = 2;
	Index[1]._2 = 3;

	IndexBuffer->Unlock();
	return S_OK;
}
HRESULT Buffer::Ready_TileLEFT_Buffer()
{
	VertexSize = sizeof(VTXCUBE);
	VertexCount = 4;
	TRICount = 2;
	VertexFormat = FVF_CUBE;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;

	VTXCUBE* Vertex = NULL;
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;
	VertexBuffer->Lock(0, 0, (void**)&Vertex, 0);

	Vertex[0].vPosition = { -1.f, 0.f, 1.f };
	Vertex[0].vTexUV = { 0,0 };

	Vertex[1].vPosition = { 1.f, 0.f, 1.f };
	Vertex[1].vTexUV = { 1,0 };

	Vertex[2].vPosition = { 1.f, 0.f, -1.f };
	Vertex[2].vTexUV = { 1,1 };

	Vertex[3].vPosition = { -1.f, 0.f, -1.f };
	Vertex[3].vTexUV = { 0,1 };
	VertexBuffer->Unlock();

	INDEX32* Index = nullptr;

	IndexBuffer->Lock(0, 0, (void**)&Index, 0);

	Index[0]._0 = 0;
	Index[0]._1 = 1;
	Index[0]._2 = 2;

	Index[1]._0 = 0;
	Index[1]._1 = 2;
	Index[1]._2 = 3;

	IndexBuffer->Unlock();
	return S_OK;
}
HRESULT Buffer::Ready_Tile_Buffer()
{
	VertexSize = sizeof(VTXCUBE);
	VertexCount = 4;
	TRICount = 2;
	VertexFormat = FVF_CUBE;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;

	VTXCUBE* Vertex = NULL;
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;
	VertexBuffer->Lock(0, 0, (void**)&Vertex, 0);


	Vertex[0].vPosition = { -1.f, 0.f, 1.f };
	Vertex[0].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	Vertex[0].vTexUV = {0,0};

	Vertex[1].vPosition = { 1.f, 0.f, 1.f };
	Vertex[1].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	Vertex[1].vTexUV = {1,0};

	Vertex[2].vPosition = { 1.f, 0.f, -1.f };
	Vertex[2].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	Vertex[2].vTexUV = { 1,1 };

	Vertex[3].vPosition = { -1.f, 0.f, -1.f };
	Vertex[3].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	Vertex[3].vTexUV = { 0,1};
	VertexBuffer->Unlock();

	INDEX32* Index = nullptr;

	IndexBuffer->Lock(0, 0, (void**)&Index, 0);

	Index[0]._0 = 0;
	Index[0]._1 = 1;
	Index[0]._2 = 2;

	Index[1]._0 = 0;
	Index[1]._1 = 2;
	Index[1]._2 = 3;

	IndexBuffer->Unlock();
	return S_OK;
}
HRESULT Buffer::Ready_Cube_Buffer()
{
	VertexSize = sizeof(VTXTRUECUBE);
	VertexCount = 24;
	TRICount = 12;
	VertexFormat = FVF_TRUECUBE;

	IndexSize = sizeof(INDEX32);
	IndexFormat = D3DFMT_INDEX32;


	VTXTRUECUBE* pVertex = NULL;

	// &pVertex : 버텍스버퍼에 보관된 버텍스 중 첫 번째 버텍스의 주소를 얻어옴
	if (FAILED(GRPDEV->CreateVertexBuffer(VertexSize * VertexCount, 0, VertexFormat, D3DPOOL_MANAGED, &VertexBuffer, NULL)))	return E_FAIL;
	if (FAILED(GRPDEV->CreateIndexBuffer(IndexSize * TRICount, 0, IndexFormat, D3DPOOL_MANAGED, &IndexBuffer, NULL)))			return E_FAIL;

	VertexBuffer->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0].vPosition = { -1, 1, -1 }; pVertex[0].vNormal = { 0, 0, -1 }; pVertex[0].vTexUV = { 0.f, 0.f, 0.f };
	pVertex[1].vPosition = { 1, 1, -1 }; pVertex[1].vNormal = { 0, 0, -1 }; pVertex[1].vTexUV = { 1.f, 0.f, 0.f };
	pVertex[2].vPosition = { 1,-1, -1 }; pVertex[2].vNormal = { 0, 0, -1 }; pVertex[2].vTexUV = { 1.f, 1.f, 0.f };
	pVertex[3].vPosition = { -1,-1, -1 }; pVertex[3].vNormal = { 0, 0, -1 }; pVertex[3].vTexUV = { 0.f, 1.f, 0.f };

	// --- 4~7: 뒷면 (Z+) ---
	pVertex[4].vPosition = { 1, 1,  1 }; pVertex[4].vNormal = { 0, 0, 1 }; pVertex[4].vTexUV = { 0.f, 0.f, 0.f };
	pVertex[5].vPosition = { -1, 1,  1 }; pVertex[5].vNormal = { 0, 0, 1 }; pVertex[5].vTexUV = { 1.f, 0.f, 0.f };
	pVertex[6].vPosition = { -1,-1,  1 }; pVertex[6].vNormal = { 0, 0, 1 }; pVertex[6].vTexUV = { 1.f, 1.f, 0.f };
	pVertex[7].vPosition = { 1,-1,  1 }; pVertex[7].vNormal = { 0, 0, 1 }; pVertex[7].vTexUV = { 0.f, 1.f, 0.f };

	// --- 8~11: 윗면 (Y+) ---
	pVertex[8].vPosition = { -1, 1,  1 }; pVertex[8].vNormal = { 0, 1, 0 }; pVertex[8].vTexUV = { 0.f, 0.f, 0.f };
	pVertex[9].vPosition = { 1, 1,  1 }; pVertex[9].vNormal = { 0, 1, 0 }; pVertex[9].vTexUV = { 1.f, 0.f, 0.f };
	pVertex[10].vPosition = { 1, 1, -1 }; pVertex[10].vNormal = { 0, 1, 0 }; pVertex[10].vTexUV = { 1.f, 1.f, 0.f };
	pVertex[11].vPosition = { -1, 1, -1 }; pVertex[11].vNormal = { 0, 1, 0 }; pVertex[11].vTexUV = { 0.f, 1.f, 0.f };

	// --- 12~15: 아랫면 (Y-) ---
	pVertex[12].vPosition = { -1,-1, -1 }; pVertex[12].vNormal = { 0,-1, 0 }; pVertex[12].vTexUV = { 0.f, 0.f, 0.f };
	pVertex[13].vPosition = { 1,-1, -1 }; pVertex[13].vNormal = { 0,-1, 0 }; pVertex[13].vTexUV = { 1.f, 0.f, 0.f };
	pVertex[14].vPosition = { 1,-1,  1 }; pVertex[14].vNormal = { 0,-1, 0 }; pVertex[14].vTexUV = { 1.f, 1.f, 0.f };
	pVertex[15].vPosition = { -1,-1,  1 }; pVertex[15].vNormal = { 0,-1, 0 }; pVertex[15].vTexUV = { 0.f, 1.f, 0.f };

	// --- 16~19: 왼쪽면 (X-) ---
	pVertex[16].vPosition = { -1, 1,  1 }; pVertex[16].vNormal = { -1, 0, 0 }; pVertex[16].vTexUV = { 0.f, 0.f, 0.f };
	pVertex[17].vPosition = { -1, 1, -1 }; pVertex[17].vNormal = { -1, 0, 0 }; pVertex[17].vTexUV = { 1.f, 0.f, 0.f };
	pVertex[18].vPosition = { -1,-1, -1 }; pVertex[18].vNormal = { -1, 0, 0 }; pVertex[18].vTexUV = { 1.f, 1.f, 0.f };
	pVertex[19].vPosition = { -1,-1,  1 }; pVertex[19].vNormal = { -1, 0, 0 }; pVertex[19].vTexUV = { 0.f, 1.f, 0.f };

	// --- 20~23: 오른쪽면 (X+) ---
	pVertex[20].vPosition = { 1, 1, -1 }; pVertex[20].vNormal = { 1, 0, 0 }; pVertex[20].vTexUV = { 0.f, 0.f, 0.f };
	pVertex[21].vPosition = { 1, 1,  1 }; pVertex[21].vNormal = { 1, 0, 0 }; pVertex[21].vTexUV = { 1.f, 0.f, 0.f };
	pVertex[22].vPosition = { 1,-1,  1 }; pVertex[22].vNormal = { 1, 0, 0 }; pVertex[22].vTexUV = { 1.f, 1.f, 0.f };
	pVertex[23].vPosition = { 1,-1, -1 }; pVertex[23].vNormal = { 1, 0, 0 }; pVertex[23].vTexUV = { 0.f, 1.f, 0.f };

	// 루프에서 vTexUV = vPosition 부분은 삭제하세요!
	for (int i = 0; i < 24; ++i) pVertex[i].dwColor = 0xffffffff;

	VertexBuffer->Unlock();

	INDEX32* pIndex = nullptr;

	IndexBuffer->Lock(0, 0, (void**)&pIndex, 0);

	for (int i = 0; i < 6; ++i)
	{
		int vIdx = i * 4; // 각 면의 시작 정점 번호
		int iIdx = i * 2; // 각 면의 첫 번째 삼각형 번호

		// 첫 번째 삼각형 (0-1-2)
		pIndex[iIdx]._0 = vIdx;
		pIndex[iIdx]._1 = vIdx + 1;
		pIndex[iIdx]._2 = vIdx + 2;

		// 두 번째 삼각형 (0-2-3)
		pIndex[iIdx + 1]._0 = vIdx;
		pIndex[iIdx + 1]._1 = vIdx + 2;
		pIndex[iIdx + 1]._2 = vIdx + 3;
	}

	IndexBuffer->Unlock();

	return S_OK;
}

HRESULT Buffer::Ready_Cube_Batch()
{
	return E_NOTIMPL;
}

Buffer* Buffer::Create(LPDIRECT3DDEVICE9 _GRPDEV, BUFFER_TYPE _TYPE) {
	Buffer* BUF = new Buffer(_GRPDEV);
	if (FAILED(BUF->Ready_Buffer(_TYPE))) {
		MSG_BOX("Cannot Create Buffer.");
		Safe_Release(BUF);
		return nullptr;
	}
	return BUF;
}
Component*	Buffer::Clone() {
	return new Buffer(*this);
}
VOID		Buffer::Free() {

	if(false == CLONE)
		Safe_Delete_Array(pVBufferPos);
		
	Safe_Release(VertexBuffer);
	Safe_Release(IndexBuffer);

	Component::Free();
}