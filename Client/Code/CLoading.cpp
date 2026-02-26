#include "../Include/pch.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev), m_bFinish(false), m_eLoadingID(LOADING_END)
{
    ZeroMemory(m_szLoading, sizeof(m_szLoading));
    m_pGraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID, wstring Path)
{
    InitializeCriticalSection(&m_Crt);

    m_eLoadingID = eID;
    m_pPath = Path;

    m_hThread = (HANDLE)_beginthreadex(NULL, // 보안 속성(핸들의 상속 여부, NULL인 경우 상속에서 제외)
        0,  // 디폴트 스탯 사이즈(1 바이트)
        Thread_Main, // 구동할 쓰레드 함수
        this,          // 3번 매개 변수 함수를 통해 가공할 데이터 주소
        0,             // 쓰레드 생성 및 실행을 조정하기 위한 옵션
        NULL);         // 쓰레드 ID


    return S_OK;
}

_uint CLoading::Loading_Stage()
{
    for (auto& iter : TileManager::GetInstance()->Get_TileList(TileManager::GetInstance()->Get_CurrentStage(), TileManager::GetInstance()->Get_Mode()))
    {
        iter->Update_GameObject(0.5f);
    }
    for (auto& iter : TileManager::GetInstance()->Get_TileList(TileManager::GetInstance()->Get_CurrentStage(), TileManager::GetInstance()->Get_Mode()))
    {
        iter->LateUpdate_GameObject(0.5f);
    }
    for (auto& iter : TileManager::GetInstance()->Get_TileList(TileManager::GetInstance()->Get_CurrentStage(), TileManager::GetInstance()->Get_Mode()))
    {
        iter->Render_GameObject();
    }
    m_bFinish = true;

    return 0;
}

_uint CLoading::Loading_Resource()
{
    ResourceManager::GetInstance()->GlobalImport_Texture(m_pGraphicDev,  m_pPath);
    m_bFinish = true;
  
    return 0;

}

unsigned int CLoading::Thread_Main(void* pArg)
{
    CLoading* pLoading = reinterpret_cast<CLoading*>(pArg);

    int iFlag(0);

    EnterCriticalSection(pLoading->Get_Crt());

    switch (pLoading->Get_LoadingID())
    {
    case LOADING_STAGE:
        iFlag = pLoading->Loading_Stage();
        break;

    case LOADING_BOSS:
        break;

    case LOADING_RESOURCE:
        iFlag = pLoading->Loading_Resource();
        break;
    }

    LeaveCriticalSection(pLoading->Get_Crt());

    //_endthreadex(0);

    return iFlag;       // 0 리턴 시, _endthreadex가 자동 호출
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID, wstring Path)
{
    CLoading* pLoading = new CLoading(pGraphicDev);

    if (FAILED(pLoading->Ready_Loading(eID, Path)))
    {
        Safe_Release(pLoading);

        MSG_BOX("CLoading Create Failed");
        return nullptr;
    }

    return pLoading;
}

void CLoading::Free()
{
    WaitForSingleObject(m_hThread, INFINITE);

    CloseHandle(m_hThread);

    DeleteCriticalSection(&m_Crt);

    Safe_Release(m_pGraphicDev);

}
