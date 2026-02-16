#include "../Include/PCH.h"

CLAttack::CLAttack(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_FrameTick(0.f), m_TextureIndex(0), m_bCheck(false){}
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
    m_fSpeed = 3.f;
    m_CLPos = vPos;
    _vec3 up(0, 1, 0), vRight,vPlayerR, vPlayerL;
    _matrix matWorld, RotZ;
    vPlayerL = m_vLook - m_vPlayerPos;
    
    D3DXVec3Cross(&vRight, &up, &vLook);
    D3DXVec3Normalize(&vRight, &vRight);
    vPlayerR = vRight- m_vPlayerPos;
    D3DXVec3Normalize(&vPlayerR, &vPlayerR);
    D3DXVec3Normalize(&vPlayerL, &vPlayerL);

    D3DXVec3Cross(&up, &vLook, &vRight);
    D3DXVec3Normalize(&up, &up);

    D3DXMatrixRotationAxis(&RotZ,&m_vLook,acosf(D3DXVec3Dot(&vPlayerR,&vPlayerL)));
    
    D3DXVec3TransformNormal(&vRight,&vRight,&RotZ);
    D3DXVec3TransformNormal(&up, &up, &RotZ);
    D3DXVec3TransformNormal(&m_vLook, &m_vLook, &RotZ);
    memcpy(matWorld.m[0], &vRight,  sizeof(_vec3));
    memcpy(matWorld.m[1], &up,      sizeof(_vec3));
    memcpy(matWorld.m[2], &m_vLook, sizeof(_vec3));
    memcpy(matWorld.m[3], &vPos,    sizeof(_vec3));
    
    Component_Transform->Set_World(&matWorld);

    switch (m_eLeaf)
    {
    case LEAF_ATTACK::LEAF_FIRST:
        Make_TextureList(L"Spr_Bullet_LaulaStandardBullet_0");
        break;
    case LEAF_ATTACK::LEAF_SECOND:
        break;

    case LEAF_ATTACK::LEAF_THIRD:
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
        break;
    case LEAF_ATTACK::LEAF_THIRD:
        break;
    case LEAF_ATTACK::LEAF_FOUR:
        break;
    }
}

void CLAttack::Leaf_First(const _float& _DT)
{
    _vec3 vUp, vRight, vCross, vLocal, _vPos, SetPos = {}, up{ 0,1,0 };
    _float fAngle;
    _matrix RotZ, Axis, matWorld, matBill, matView;
    matWorld = *Component_Transform->Get_World();
    GRPDEV->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixIdentity(&matBill);
    
    matBill._11 = matView._11;
    matBill._13 = matView._13;
    matBill._31 = matView._31;
    matBill._33 = matView._33;
    
    matWorld =  matBill * matWorld;

    Component_Transform->Move_Pos(&m_vLook, m_fSpeed, _DT);
    //memcpy(matWorld.m[3], &m_CLPos, sizeof(_vec3));
    //
    //Component_Transform->Set_Pos({ matWorld._41 , matWorld._42 , matWorld._43 });
    Component_Transform->Set_World(&matWorld);

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