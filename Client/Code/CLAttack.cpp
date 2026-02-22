#include "../Include/PCH.h"

CLAttack::CLAttack(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_fFrameSpeed(0.1f), m_fDeadTick(0.f),m_iDeadCnt(0),m_bSpin(false), m_fAttackTick(0.f), m_iAttackIndex(0),m_FrameTick(0.f), m_TextureIndex(0), m_bCheck(false){}
CLAttack::CLAttack(const GameObject& _RHS) : GameObject(_RHS) {}
CLAttack::~CLAttack() {}

HRESULT CLAttack::Ready_GameObject(LEAF_ATTACK eLeaft, _vec3 vPos, _vec3 vLook, _bool bSpin)
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
    m_bSpin = bSpin;
    switch (m_eLeaf)
    {
    case LEAF_ATTACK::LEAF_FIRST:
        Make_TextureList(L"Spr_Bullet_LaulaStandardBullet_0");
        Component_Collider->Set_Att(5.f);
        break;
    case LEAF_ATTACK::LEAF_SECOND:
        Make_TextureList(L"Spr_Bullet_Cheonlog_DivideFlowerLv3_Black_0");
        m_iRandCnt = 5 + rand() % 2;
        break;
    case LEAF_ATTACK::LEAF_THIRD:
        Make_TextureList(L"Spr_Bullet_Cheonlog_DivideFlowerLv3_White_0");
        m_iRandCnt = 5 + rand() % 2;
        break;
    case LEAF_ATTACK::LEAF_EXPLOSION:
        Make_TextureList(L"Spr_Effect_Cheonlog_BigExplosione_Birth");
        Component_Transform->Set_Scale(1.5f, 1.5f, 1.5f);
        m_fFrameSpeed = 0.08f;
        break;
    case LEAF_ATTACK::LEAF_FOUR:
        Make_TextureList(L"Spr_Bullet_Cheonlog_DivideFlowerLv2_0");        
        break;
    }
    CollisionManager::GetInstance()->Add_ColliderObject(this);
    return S_OK;
}

INT CLAttack::Update_GameObject(const _float& _DT)
{
    if (Get_ObjectDead() == TRUE)
        return -1;

    GameObject::Update_GameObject(_DT);
    RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
    Move_Frame(_DT);
    Move_Leaf(_DT);

    return S_OK;
}
BOOL CLAttack::OnCollisionEnter(GameObject* _Other)
{
    wstring Tag = _Other->Get_ObjectTag();
    if (Tag == L"Player" && m_eLeaf != LEAF_ATTACK::LEAF_EXPLOSION)
    {
        _float hp(0);
        MainUI* mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
        mainUI->Player_LostHP();
        Set_ObjectDead(TRUE);
    }
    else
    {
        return false;
    }

    return TRUE;
}
BOOL CLAttack::OnCollisionStay(GameObject* _Other)
{
    wstring Tag = _Other->Get_ObjectTag();
   if(Tag == L"Player" && m_eLeaf == LEAF_ATTACK::LEAF_EXPLOSION && m_TextureIndex == TextureList.size() - 3)
    {
        MainUI* mainUI = dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"MainUI"));
        mainUI->Player_LostHP();
    }
    else
    {
        return false;
    }

    return TRUE;
}
void CLAttack::LateUpdate_GameObject(const _float& _DT)
{
    GameObject::LateUpdate_GameObject(_DT);
  
    m_fDeadTick += _DT;
    if (m_fDeadTick > 1)
    {
        m_fDeadTick = 0;
        ++m_iDeadCnt;
    }
    
    if (m_iDeadCnt >= 9)
        Set_ObjectDead(TRUE);

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
    Component_Collider  = ADD_COMPONENT_COLLIDER;

    Component_Collider->Set_CenterPos(Component_Transform);
    Component_Collider->Set_Scale(1.f, 1.0f, 1.f);
    return S_OK;
}

void CLAttack::Move_Frame(const _float& _DT)
{
    m_FrameTick += _DT;

    if (m_FrameTick > m_fFrameSpeed)
    {
        ++m_TextureIndex;
        m_FrameTick = 0.f;
    }
    if (m_TextureIndex > TextureList.size() - 1)
    {
        if (m_eLeaf != LEAF_ATTACK::LEAF_EXPLOSION)
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
        Leaf_Four(_DT);
        break;
    case LEAF_ATTACK::LEAF_EXPLOSION:
        Leaf_Explosion(_DT);
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
    if (!m_bSpin)
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

        Leaf_Bill(_DT);
    }
    else
    {
        m_fSpeed = 7.f;
        Leaf_Bill(_DT);
    }
}
void CLAttack::Leaf_Third(const _float& _DT)
{
    if (!m_bSpin)
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

        Leaf_Bill(_DT);
    }
    else
    {
        m_fSpeed = 7.f;
        Leaf_Bill(_DT);
    }
}

void CLAttack::Leaf_Four(const _float& _DT)
{
    m_fAttackTick += _DT;

    _float fAngle;
    _vec3 vPos;
    _matrix matScale, RotZ, matWorld, matBill, matView;
    if (m_fAttackTick > 2)
    {
        Component_Transform->Get_Info(INFO_POS, &vPos);
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEAF_SPIN_DEATH, m_CLPos, TRUE, { 0.4,0.4,0.4 },{45,0,0},0.1f,m_vLook));
        Set_ObjectDead(TRUE);
    }
    
    if (Get_ObjectDead() == TRUE)
        return;

    matWorld = *Component_Transform->Get_World();
    GRPDEV->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matBill, nullptr, &matView);

    D3DXMatrixScaling(&matScale, 0.4f, 0.4f, 0.4f); 
    matWorld = matScale * matBill;
    m_vLook.y = 0;
    m_CLPos += m_vLook * m_fSpeed * _DT;

    memcpy(matWorld.m[3], &m_CLPos, sizeof(_vec3));

    Component_Transform->Set_World(&matWorld);
    Component_Transform->Set_Pos({ matWorld._41 , 0.1f, matWorld._43 });
}

void CLAttack::Leaf_Explosion(const _float& _DT)
{
    Component_Collider->Set_Scale(2.f, 2.f, 2.f);
    _vec3 vPos;
    vPos = *Component_Transform->Get_Position();

    if (m_TextureIndex == 16)
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEAF_EXPLOSION_CIRCLE, { vPos.x,-0.2f,vPos.z - 3 }, TRUE, { 6,0,5 }, { 20,0,0 }, 0.1f));
    if (m_TextureIndex == 25)
        Component_Transform->Set_Scale(5.f, 5.f, 5.f);

    if (m_TextureIndex > TextureList.size() - 1)
        Set_ObjectDead(TRUE);

    _float fAngle;
    _vec3 vScale;
    _matrix matScale, RotZ, matWorld, matBill, matView;
    vScale = *Component_Transform->Get_Scale();
    matWorld = *Component_Transform->Get_World();
    GRPDEV->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matBill, nullptr, &matView);

    D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);

    matWorld = matScale * matBill;
    memcpy(matWorld.m[3], vPos, sizeof(_vec3));
    Component_Transform->Set_World(&matWorld);
    Component_Transform->Set_Pos({ matWorld._41 , 1.f , matWorld._43 });
}

void CLAttack::Leaf_Bill(const _float& _DT)
{
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




CLAttack* CLAttack::Create(LPDIRECT3DDEVICE9 _GRPDEV, LEAF_ATTACK eLeaft, _vec3 vPos, _vec3 vLook, _bool bSpin)
{
    CLAttack* CL = new CLAttack(_GRPDEV);
    if (FAILED(CL->Ready_GameObject(eLeaft, vPos, vLook, bSpin))) {
        MSG_BOX("Cannot Create CLAttack.");
        Safe_Release(CL);
        return nullptr;
    }
    return CL;
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
        wstring FileName = _FileName + to_wstring(FRAME) + L".dds";
        IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
        if (TEX == nullptr) break;
        else { TEX->AddRef();  TextureList.push_back(TEX); }
    }

    return S_OK;
}