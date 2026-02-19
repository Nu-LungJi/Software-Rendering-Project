#include "../Include/PCH.h"

CLAttack::CLAttack(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_fAttackTick(0.f), m_iAttackIndex(0),m_FrameTick(0.f), m_TextureIndex(0), m_bCheck(false){}
CLAttack::CLAttack(const GameObject& _RHS) : GameObject(_RHS) {}
CLAttack::~CLAttack() {}

HRESULT CLAttack::Ready_GameObject(LEAF_ATTACK eLeaft, _vec3 vPos, _vec3 vLook)
{
    if (FAILED(Component_Initialize())) return E_FAIL;
    m_eLeaf = eLeaft;
    Component_Transform->Set_Pos(vPos);
    Component_Transform->Set_Scale(0.5f, 0.5f, 0.5f);

    Component_Transform->Rotation(ROT_Y, 45);
    dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Info(INFO_POS,&m_vPlayerPos);

    m_vLook = vLook;
    m_fSpeed = 5.f;
    m_CLPos = vPos;

    switch (m_eLeaf)
    {
    case LEAF_ATTACK::LEAF_FIRST:
        Make_TextureList(L"Spr_Bullet_LaulaStandardBullet_0");
        break;
    case LEAF_ATTACK::LEAF_SECOND:
        Make_TextureList(L"Spr_Bullet_Cheonlog_DivideFlowerLv3_Black_0");
        m_iRandCnt = 5 + rand() % 2;
        break;
    case LEAF_ATTACK::LEAF_THIRD:
        Make_TextureList(L"Spr_Bullet_Cheonlog_DivideFlowerLv3_White_0");
        m_iRandCnt = 5 + rand() % 2;
        break;
    case LEAF_ATTACK::LEAF_FOUR:
        break;
    }

    return S_OK;
}

INT CLAttack::Update_GameObject(const _float& _DT)
{
    GameObject::Update_GameObject(_DT);
    RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
    Move_Frame(_DT);
    Move_Leaf(_DT);
  
    return S_OK;
}

void CLAttack::LateUpdate_GameObject(const _float& _DT)
{
    GameObject::LateUpdate_GameObject(_DT);
}

void CLAttack::Render_GameObject()
{
    GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

    GRPDEV->SetTexture(0, TextureList[m_TextureIndex]);

    Component_Buffer->Render_Buffer();

    GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

HRESULT CLAttack::Component_Initialize()
{
    Component_Buffer    = ADD_COMPONENT_RECTTEX;
    Component_Transform = ADD_COMPONENT_TRANSFORM;

    return S_OK;
}

void CLAttack::Move_Frame(const _float& _DT)
{
    m_FrameTick += _DT;

    if (m_FrameTick > 0.1f)
    {
        ++m_TextureIndex;
        m_FrameTick = 0.f;
    }
    if (m_TextureIndex > TextureList.size() - 1)
    {
        m_TextureIndex = 0;
    }
}

void CLAttack::Move_Leaf(const _float& _DT)
{
    switch (m_eLeaf)
    {
    case LEAF_ATTACK::LEAF_FIRST:
        Leaf_First(_DT);
        break;
    case LEAF_ATTACK::LEAF_SECOND:
        Leaf_Second(_DT);
        break;
    case LEAF_ATTACK::LEAF_THIRD:
        Leaf_Third(_DT);
        break;
    case LEAF_ATTACK::LEAF_FOUR:
        break;
    }
}

void CLAttack::Leaf_First(const _float& _DT)
{
    _float fAngle;
    _matrix matScale,RotZ, matWorld, matBill, matView;
    matWorld = *Component_Transform->Get_World();
    GRPDEV->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matBill, nullptr, &matView);

    D3DXMatrixScaling(&matScale, 0.7f, 0.2f, 0.4f);

    fAngle = atan2f(m_vLook.z, m_vLook.x); // x 기준으로 z가 얼마나 돌아가있는지
    D3DXMatrixRotationZ(&RotZ, fAngle);    //그걸로 z만 돌리기

    matWorld = matScale * RotZ * matBill; 
    m_vLook.y = 0.f;
    m_CLPos += m_vLook * m_fSpeed * _DT;
  
    memcpy(matWorld.m[3], &m_CLPos, sizeof(_vec3));

    Component_Transform->Set_World(&matWorld);
    Component_Transform->Set_Pos({ matWorld._41 , 0.1f , matWorld._43 });

}

void CLAttack::Leaf_Second(const _float& _DT)
{
    m_fAttackTick += _DT;

    if (m_fAttackTick > 0.2)
    {
        m_fAttackTick = 0;
        ++m_iAttackIndex;
    }

    if (m_iAttackIndex == m_iRandCnt)
    {
        _vec3 vPos, vCLPos;
        Component_Transform->Get_Info(INFO_POS, &vPos);
        dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Info(INFO_POS, &vCLPos);
        vCLPos += { 0.9f, 1.f, 3.1f };
        m_vLook = vCLPos - vPos;
        D3DXVec3Normalize(&m_vLook, &m_vLook);
    }

    _float fAngle;
    _matrix matScale, RotZ, matWorld, matBill, matView;
    matWorld = *Component_Transform->Get_World();
    GRPDEV->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matBill, nullptr, &matView);

    D3DXMatrixScaling(&matScale, 0.4f, 0.4f, 0.4f);

    fAngle = atan2f(m_vLook.z, m_vLook.x) + D3DXToRadian(270); // x 기준으로 z가 얼마나 돌아가있는지
    D3DXMatrixRotationZ(&RotZ, fAngle);    //그걸로 z만 돌리기

    matWorld = matScale * RotZ * matBill;
    m_vLook.y = 0;
    m_CLPos += m_vLook * m_fSpeed * _DT;

    memcpy(matWorld.m[3], &m_CLPos, sizeof(_vec3));

    Component_Transform->Set_World(&matWorld);
    Component_Transform->Set_Pos({ matWorld._41 , 0.1f, matWorld._43 });
}

void CLAttack::Leaf_Third(const _float& _DT)
{
    m_fAttackTick += _DT;

    if (m_fAttackTick > 0.2)
    {
        m_fAttackTick = 0;
        ++m_iAttackIndex;
    }
    if (m_iAttackIndex == m_iRandCnt)
    {
        _vec3 vPos, vCLPos;
        Component_Transform->Get_Info(INFO_POS, &vPos);
        dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"CheonLog")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Info(INFO_POS, &vCLPos);
        vCLPos += { 0.9f, 1.f, 3.1f };
        m_vLook = vCLPos - vPos;
        D3DXVec3Normalize(&m_vLook, &m_vLook);
    }

    _float fAngle;
    _matrix matScale, RotZ, matWorld, matBill, matView;
    matWorld = *Component_Transform->Get_World();
    GRPDEV->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matBill, nullptr, &matView);

    D3DXMatrixScaling(&matScale, 0.4f, 0.4f, 0.4f);

    fAngle = atan2f(m_vLook.z, m_vLook.x) + D3DXToRadian(270); // x 기준으로 z가 얼마나 돌아가있는지
    D3DXMatrixRotationZ(&RotZ, fAngle);    //그걸로 z만 돌리기

    matWorld = matScale * RotZ * matBill;
    m_vLook.y = 0;
    m_CLPos += m_vLook * m_fSpeed * _DT;

    memcpy(matWorld.m[3], &m_CLPos, sizeof(_vec3));

    Component_Transform->Set_World(&matWorld);
    Component_Transform->Set_Pos({ matWorld._41 , 0.1f , matWorld._43 });
}


CLAttack* CLAttack::Create(LPDIRECT3DDEVICE9 _GRPDEV, LEAF_ATTACK eLeaft, _vec3 vPos, _vec3 vLook)
{
    CLAttack* CL = new CLAttack(_GRPDEV);
    if (FAILED(CL->Ready_GameObject(eLeaft, vPos, vLook))) {
        MSG_BOX("Cannot Create CLAttack.");
        Safe_Release(CL);
        return nullptr;
    }
    return CL;
}

BOOL CLAttack::OnCollisionEnter(GameObject* _Other)
{
    return 0;
}

void CLAttack::Free()
{
    for (auto& iter : TextureList)
        Safe_Release(iter);

    GameObject::Free();
}

HRESULT CLAttack::Make_TextureList(wstring _FileName) {
    INT FRAME = 0;

    while (++FRAME) {
        wstring FileName = _FileName + to_wstring(FRAME) + L".png";
        IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
        if (TEX == nullptr) break;
        else { TEX->AddRef();  TextureList.push_back(TEX); }
    }

    return S_OK;
}