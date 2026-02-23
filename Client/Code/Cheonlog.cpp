#include "../Include/PCH.h"

Cheonlog::Cheonlog(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV), m_bDead(false), m_bStartPattern(false), m_iNextSkill(0), m_fAttackSecondTick(0.f), m_frameAttack(0.f), m_iBulletCnt(0), m_fRotY(0.f), m_iSkillDelay(0), m_bMoveEffect(false), m_iStatuCnt(0), m_iSkillMaxCnt(0), m_iSkillCnt(0), m_StartAttack(false), m_EndEffect(true), m_vDebug(0, 0, 0), m_pTarget(nullptr), m_frameTick(0.f), m_iFrameCnt(0), m_eCheck(CHECK_END), m_eStatu(CL_END) {}
Cheonlog::Cheonlog(const GameObject& _RHS) : GameObject(_RHS), m_pTarget(nullptr), m_bCrystal(false) {}
Cheonlog::~Cheonlog() {}

HRESULT Cheonlog::Ready_GameObject(_vec3 vPos) {
    if (FAILED(Component_Initialize())) return E_FAIL;

    Texture_Initalize(8, L"Spr_Boss_Cheonlog_Shining_Stand_R_0%d.png", CL_IDELR);
    Texture_Initalize(8, L"Spr_Boss_Cheonlog_Shining_Jump_L_045_0%d.png", CL_LJUMP);
    Texture_Initalize(8, L"Spr_Boss_Cheonlog_Shining_RU_135_0%d.png", CL_RJUMP);
    Texture_Initalize(26, L"Spr_Boss_Cheonlog_Death_0%d.png", CL_DEAD);
    m_fPivot = 0.1f;
    m_eCheck = IDEL;
    m_eStatu = SPAWN;
    m_vCenter = vPos;
    Component_Transform->Set_Pos(vPos);
    m_bSpawn = true;
    CollisionManager::GetInstance()->Add_ColliderObject(this);

    return S_OK;
}
INT   Cheonlog::Update_GameObject(const _float& _DT)
{
    if (Component_Collider->Get_Hp() <= 0)
        m_eStatu = CL_DEAD;

    GameObject::Update_GameObject(_DT);

    {
        _float fAngle;
        _vec3 vScale, vPos;
        _matrix matScale, RotZ, matWorld, matBill, matView;
        vScale = *Component_Transform->Get_Scale();
        matWorld = *Component_Transform->Get_World();
        vPos = *Component_Transform->Get_Position();
        GRPDEV->GetTransform(D3DTS_VIEW, &matView);
        D3DXMatrixInverse(&matBill, nullptr, &matView);

        D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);

        matWorld = matScale * matBill;
        memcpy(matWorld.m[3], vPos, sizeof(_vec3));
        Component_Transform->Set_World(&matWorld);
        Component_Transform->Set_Pos({ matWorld._41 , 0.5f , matWorld._43 });
    }
    RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
    return 0;
}
void Cheonlog::LateUpdate_GameObject(const _float& _DT) {

    GameObject::LateUpdate_GameObject(_DT);

    if (!m_bDead)
    {
        Change_Pattern(_DT);
        Change_Statu(_DT, m_vecCheonlogTexture[m_eStatu].size());
    }

}
void Cheonlog::Render_GameObject() {
    GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

    Set_Statu();
    Component_Buffer->Render_Buffer();

    GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    GRPDEV->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}
HRESULT Cheonlog::Component_Initialize() {
    Component_Buffer = ADD_COMPONENT_RECTTEX;
    Component_Transform = ADD_COMPONENT_TRANSFORM;

    Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
    Component_Transform->Set_Scale(3.f, 3.f, 3.f);
    Component_Transform->Set_Pos(0.f, 0.5f, 0.f);
    //ÁÂ¿ì¹ÝÀü
    //Component_Transform->Set_Scale(-2.f, 2.f, 2.f);

    Component_Collider = ADD_COMPONENT_COLLIDER;
    Component_Collider->Set_CenterPos(Component_Transform);
    Component_Collider->Set_Hp(100);

    Component_Collider->Set_Scale(2.f, 1.5f, 2.f);

    return S_OK;
}

void Cheonlog::Texture_Initalize(_int iCnt, const _tchar* pName, CL_STATU CheongLog)
{
    for (int i = 1; i < iCnt + 1; ++i)
    {
        TCHAR   Name[128] = L"";
        wsprintf(Name, pName, i);

        auto tex = ResourceManager::GetInstance()->Find_Texture(Name);

        if (tex == nullptr)
            return;

        tex->AddRef();

        m_vecCheonlogTexture[CheongLog].push_back(tex);
    }
}

void Cheonlog::Set_Statu()
{
    switch (m_eStatu)
    {
    case SPAWN:
        break;
    case CL_IDELR:
        GRPDEV->SetTexture(0, m_vecCheonlogTexture[m_eStatu][m_iFrameCnt]);
        break;
    case CL_LJUMP:
        GRPDEV->SetTexture(0, m_vecCheonlogTexture[m_eStatu][m_iFrameCnt]);
        break;
    case CL_RJUMP:
        GRPDEV->SetTexture(0, m_vecCheonlogTexture[m_eStatu][m_iFrameCnt]);
        break;
    case CL_DEAD:
        GRPDEV->SetTexture(0, m_vecCheonlogTexture[m_eStatu][m_iFrameCnt]);
        break;
    }
}
void Cheonlog::Change_Statu(const _float& _DT, _int iMaxCnt)
{
    _vec3 vPos, vScale;
    Component_Transform->Get_Info(INFO_POS, &vPos);

    if (KeyManager::GetInstance()->Get_KeyState(DIK_Z))
    {
        vPos.x += 0.5;
        Component_Transform->Set_Pos(vPos);
    }
    if (KeyManager::GetInstance()->Get_KeyState(DIK_X))
    {
        vPos.z += 0.5;
        Component_Transform->Set_Pos(vPos);
    }
    if (KeyManager::GetInstance()->Get_KeyState(DIK_K))
    {
        m_eStatu = CL_RJUMP;
    }
    if (KeyManager::GetInstance()->Get_KeyState(DIK_L))
    {
        m_eStatu = CL_IDELR;
        m_iSkillDelay = 0;
    }
    if (m_bStartPattern)
    {
        Change_Pattern(_DT);
    }
    if (KeyManager::GetInstance()->Get_KeyState(DIK_M))
    {
        //m_eCheck = ATTACK_B;
        //m_eStatu = SPAWN;
    }
    //ÀÌµ¿ ¸ð¼Ç °ü·Ã
    switch (m_eStatu)
    {
    case SPAWN:
        Create_Cheonlog(_DT, vPos);
        break;
    case CL_IDELR:
        m_frameTick += _DT;
        if (m_frameTick > 0.1)
        {
            ++m_iFrameCnt;
            m_frameTick = 0;
        }
        if (m_iFrameCnt > iMaxCnt - 1)
            m_iFrameCnt = 0;
        break;
    case CL_LJUMP:
        CL_Jump(_DT, iMaxCnt);
        break;
    case CL_RJUMP:
        CL_JumpCenter(_DT, iMaxCnt);
        break;
    case CL_DEAD:
        m_frameTick += _DT;
        if (m_frameTick > 0.1)
        {
            ++m_iFrameCnt;
            m_frameTick = 0;
        }
        if (m_iFrameCnt > iMaxCnt - 1)
        {
            m_iFrameCnt = 0;
            m_bDead = true;
        }
        break;
    }

    //°ø°Ý °ü·Ã
    switch (m_eCheck)
    {
    case SPAWN_AFTER:
        Create_Cheonlog_After(_DT, vPos);
        break;
    case IDEL:
        break;
    case ATTACK_A:
        AttackLeaf_First(_DT, vPos);
        break;
    case ATTACK_B:
        AttackLeaf_Second(_DT, vPos);
        break;
    case ATTACK_C:
        AttackLeaf_Third(_DT, vPos);
        break;
    case ATTACK_D:
        AttackLeaf_Four(_DT, vPos);
        break;
    }

    static _float fMin(-100), fMax(100);
    ImGui::SetNextWindowSize({ 800,600 });
    ImGui::Begin("CheonLogDebug", NULL, ImGuiWindowFlags_MenuBar);
    ImGui::Text("CheonL");
    ImGui::SameLine(100.f, 0.f);
    ImGui::SliderFloat3("CL", m_vDebug, fMin, fMax);

    Debug_Button("CLde", &m_vDebug, 10.f);
    ImGui::End();


    if (KeyManager::GetInstance()->Get_KeyState(DIK_K))
    {
        vPos = { 0,0,0 };
        m_eCheck = ATTACK_A;
        m_EndEffect = true;
    }

}

void Cheonlog::Change_Pattern(const _float& _DT)
{
    if (m_iNextSkill < 3 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
    {
        m_framePattern += _DT;
        if (m_framePattern > 3)
        {
            Reset_Pattern(ATTACK_A, CL_IDELR);
            return;
        }
    }

    if (m_iNextSkill == 3 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
    {
        m_framePattern += _DT;
        if (m_framePattern > 5)
        {
            Reset_Pattern(IDEL, CL_RJUMP);
            return;
        }
    }
    if (m_iNextSkill == 4 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
    {
        m_framePattern += _DT;
        if (m_framePattern > 8)
        {
            Reset_Pattern(ATTACK_B, CL_IDELR);
            return;
        }
    }
    if (m_iNextSkill == 5 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
    {
        m_framePattern += _DT;
        if (m_framePattern > 11)
        {
            Reset_Pattern(ATTACK_C, CL_IDELR);
            return;
        }
    }
    if (m_iNextSkill == 6 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
    {
        m_framePattern += _DT;
        if (m_framePattern > 15)
        {
            Reset_Pattern(ATTACK_A, CL_IDELR);
            return;
        }
    }
    if (m_iNextSkill == 7 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
    {
        m_framePattern += _DT;
        if (m_framePattern > 20)
        {
            Reset_Pattern(ATTACK_D, CL_IDELR);
            return;
        }
    }
}
void Cheonlog::Reset_Pattern(CL_CHECK eCheck, CL_STATU eStatu)
{
    m_iFrameCnt = 0;
    m_EndEffect = true;
    m_eCheck = eCheck;
    m_eStatu = eStatu;
    ++m_iNextSkill;
}

void Cheonlog::AttackLeaf_First(const _float& _DT, _vec3 vPos)
{
    //»Ô À§¿¡ÀÖ´Â ¼öÁ¤ + »Ô ÁÖº¯¿¡ÀÖ´Â Àü±â ÀÌÆåÆ®
    Create_Crystal();
    //³ª¹µÀÙ ½î±âÀü ÀÌÆåÆ®
    if (m_EndEffect)
    {
        vPos += { 1.1f, 1.5f, 3.7f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEAF_FIRST, vPos, TRUE));
        m_EndEffect = false;
        m_StartAttack = true;
        m_iSkillDelay = 0;
    }

    if (m_StartAttack)
    {
        Create_Leaf(_DT);
    }

}
void Cheonlog::AttackLeaf_Second(const _float& _DT, _vec3 vPos)
{
    _vec3 vLook, vLookReset = { 0,0,0 }, vOrigin;
    _matrix matRotY;
    Component_Transform->Get_Info(INFO_LOOK, &vLook);
    vOrigin = { vPos.x, vPos.y ,vPos.z + 0.5f };
    vPos = vOrigin;

    m_frameAttack += _DT;
    if (m_frameAttack > 0.2)
    {
        ++m_iSkillDelay;
        m_frameAttack = 0;
    }
    if (m_iSkillDelay > 8)
    {
        m_iSkillDelay = 0;
        m_EndEffect = true;
        ++m_iSkillCnt;
    }
    switch (m_iSkillCnt)
    {
    case 0:
        if (m_EndEffect)
        {
            EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEAF_EXPLOSION_CIRCLE, { vPos.x,-0.2f,vPos.z }, FALSE, { 6,0,5 }, { 0,0,0 }, 0.2f));
            SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(Create_Leaf_Second({ vPos.x,2.f,vPos.z + 3.f }));
            m_EndEffect = false;
        }
        break;
    case 1:
        if (m_EndEffect)
        {
            for (_int i = 0; i < 4; ++i)
            {
                D3DXMatrixRotationY(&matRotY, D3DXToRadian(i * 90));
                D3DXVec3TransformNormal(&vLookReset, &vLook, &matRotY);

                vPos += vLookReset * 3;

                EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEAF_EXPLOSION_CIRCLE, { vPos.x  , -0.2f ,vPos.z + _float(i * 0.001) }, FALSE, { 6,0,5 }, { 0,0,0 }, 0.08f));
                SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(Create_Leaf_Second({ vPos.x  , 2.f, vPos.z }));
                vLookReset = { 0,0,0 };
                vPos = vOrigin;
            }
            m_EndEffect = false;
        }
        break;
    case 2:
        if (m_EndEffect)
        {
            for (_int i = 0; i < 9; ++i)
            {
                D3DXMatrixRotationY(&matRotY, D3DXToRadian(i * 40));
                D3DXVec3TransformNormal(&vLookReset, &vLook, &matRotY);

                vPos += vLookReset * 8;

                EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEAF_EXPLOSION_CIRCLE, { vPos.x  , -0.2f ,vPos.z + _float(i * 0.001) }, FALSE, { 6,0,5 }, { 0,0,0 }, 0.08f));
                SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(Create_Leaf_Second({ vPos.x  , 2.f, vPos.z }));
                vLookReset = { 0,0,0 };
                vPos = vOrigin;
            }

            m_eCheck = IDEL;
            m_iSkillCnt = 0;
            m_EndEffect = false;
        }
        break;
    }
}
void Cheonlog::AttackLeaf_Third(const _float& _DT, _vec3 vPos)
{
    _vec3 vPlayerPos, vLook, vLookReset, vOrigin;
    _matrix RotY;
    CLAttack* pAttack = nullptr;
    if (m_EndEffect)
    {
        vPos += { 1.1f, 1.5f, 3.7f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEAF_CHARGING, vPos, TRUE));
        m_EndEffect = false;
    }

    if (!m_EndEffect)
    {
        m_frameAttack += _DT;
        m_fAttackSecondTick += _DT;
        if (m_frameAttack > 0.09)
        {
            ++m_iSkillCnt;
            m_frameAttack = 0;
            Create_Leaf_Third_S(vPos);
        }

        if (m_fAttackSecondTick > 0.08)
        {

            Create_Leaf_Third(vPos);
            m_fAttackSecondTick = 0.f;
        }

        if (m_iSkillCnt > 20)
        {
            m_eCheck = IDEL;
            m_iBulletCnt = 0;
            m_fRotY = 0;
            m_iSkillCnt = 0;
            m_iSkillDelay = 0;
        }
    }
}
void Cheonlog::AttackLeaf_Four(const _float& _DT, _vec3 vPos)
{
    if (m_EndEffect)
    {
        vPos += { 1.1f, 1.5f, 3.3f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEAF_SPIN, vPos, FALSE));
        m_EndEffect = false;
    }

    if (!m_EndEffect)
    {
        m_frameAttack += _DT;
        if (m_frameAttack > 0.3)
        {
            m_frameAttack = 0;
            ++m_iSkillCnt;
            Create_Leaf_Four(vPos, 90);
            Create_Leaf_Four(vPos, 180);
            Create_Leaf_Four(vPos, 270);
        }
        if (m_iSkillCnt > 20)
        {
            m_eCheck = IDEL;
            m_iBulletCnt = 0;
            m_fRotY = 0;
            m_iSkillCnt = 0;
            m_iNextSkill = 0;
            m_iSkillDelay = 0;
        }
    }

}

void Cheonlog::Create_Cheonlog(const _float& _DT, _vec3 vPos)
{
    m_frameAttack += _DT;
    _vec3 vLook, vLookReset = { 0,0,0 }, vOrigin;
    _matrix matRotY;
    Component_Transform->Get_Info(INFO_LOOK, &vLook);
    vOrigin = { vPos.x, vPos.y ,vPos.z + 0.5f };
    vPos = vOrigin;

    switch (m_iSkillDelay)
    {
    case 0:
        D3DXMatrixRotationY(&matRotY, D3DXToRadian(30));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
        vPos += vLook * 3;
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x,6,vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, FALSE));
        ++m_iSkillDelay;
        break;
    case 1:
        D3DXMatrixRotationY(&matRotY, D3DXToRadian(30));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
        vPos += vLook * 4;
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x  ,6,vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, FALSE));
        break;
    case 2:
        D3DXMatrixRotationY(&matRotY, D3DXToRadian(50));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
        vPos += vLook * 4;
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_R, { vPos.x ,0.5,vPos.z }, TRUE, { 3.f,3.f,3.f }, { 55,0,0 }, 0.2f, { 0,0,1 }, FALSE));
        ++m_iSkillDelay;
        break;
    case 3:

        D3DXMatrixRotationY(&matRotY, D3DXToRadian(50));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
        vPos += vLook * 5;
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x, 6, vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, FALSE));
        break;
    case 5:
        D3DXMatrixRotationY(&matRotY, D3DXToRadian(-120));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
        vPos += vLook * 6;
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x, 6, vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, FALSE));
        break;
    case 6:

        D3DXMatrixRotationY(&matRotY, D3DXToRadian(-120));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
        vPos += vLook * 7;
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_L, { vPos.x ,0.5, vPos.z }, TRUE, { 3.f,3.f,3.f }, { 55,0,0 }, 0.2f, { 0,0,1 }, FALSE));
        ++m_iSkillDelay;
        return;
    case 7:
        D3DXMatrixRotationY(&matRotY, D3DXToRadian(-120));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
        vPos += vLook * 6;
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x, 6, vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, FALSE));
        ++m_iSkillDelay;
        return;
    case 9:
        D3DXMatrixRotationY(&matRotY, D3DXToRadian(180));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
        vPos += vLook * 8;
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x, 6, vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, TRUE));
        ++m_iSkillDelay;
        break;
    }

    if (m_frameAttack > 0.3)
    {
        ++m_iSkillDelay;
        m_frameAttack = 0;
        if (m_iSkillDelay > 9)
        {
            m_fRotY += 20;
            D3DXMatrixRotationY(&matRotY, D3DXToRadian(100 + m_fRotY));
            D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
            vPos += vLook * 6;
            EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x, 6, vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, FALSE));

        }

    }


}
void Cheonlog::Create_Cheonlog_After(const _float& _DT, _vec3 vPos)
{
    _vec3 vLook, vLookReset = { 0,0,0 }, vOrigin;
    _matrix matRotY;
    Component_Transform->Get_Info(INFO_LOOK, &vLook);
    vOrigin = { vPos.x, vPos.y ,vPos.z + 6 };
    vPos = vOrigin;
    m_frameAttack += _DT;
    switch (m_iSkillDelay)
    {
    case 3:
        for (_int i = 0; i < 12; ++i)
        {
            D3DXMatrixRotationY(&matRotY, D3DXToRadian(i * 30));
            D3DXVec3TransformNormal(&vLookReset, &vLook, &matRotY);

            vPos += vLookReset * 4;
            EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x, 6, vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, FALSE));
            vLookReset = { 0,0,0 };
            vPos = vOrigin;
        }

        Create_Crystal();
        ++m_iSkillDelay;
        break;
    }
    if (m_frameAttack > 0.3)
    {
        ++m_iSkillDelay;
        m_frameAttack = 0;
    }
    if (m_iSkillDelay > 7)
    {
        m_bStartPattern = true; m_eCheck = IDEL;
        m_bSpawn = false;
    }

}

void Cheonlog::Create_Crystal()
{
    if (!m_bCrystal)
    {
        _vec3 vPos, vCur;
        Component_Transform->Get_Info(INFO_POS, &vPos);
        vCur = vPos;
        vCur += { 0.9f, 1.f, 3.1f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLcrystal::Create(GRPDEV, vCur));

        //¿ÞÂÊ»Ô
        vCur = vPos;
        vCur += { 0.0f, 0.5f, 2.7f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEFT_HORN, vCur, FALSE));

        //¿À¸¥ÂÊ»Ô
        vCur = vPos;
        vCur += { 2.3f, 0.0f, 3.2f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::RIGHT_HORN, vCur, FALSE));

        //¸öÅë
        vCur = vPos;
        vCur += { -1.2f, 0.2f, 0.8f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::CL_BODY, vCur, FALSE));

        m_bCrystal = true;
    }
}
_bool Cheonlog::Create_Leaf(const _float& _DT)
{
    _vec3 vPos, vPlayerPos, vLook, vLookS, vLookReset, vOrigin;
    _matrix RotY, RotX;
    CLAttack* pAttack = nullptr;
    Component_Transform->Get_Info(INFO_POS, &vPos);
    dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Info(INFO_POS, &vPlayerPos);
    vPos += { 1.1f, 1.5f, 3.7f }; // ÇöÀç »Ô °¡¿îµ¥ À§Ä¡
    //³ª¹µÀÙ 4°³¹ß½Î È÷È÷
    vOrigin = vPos;
    vLook = vPlayerPos - vPos;
    switch (m_iSkillCnt)
    {
    case 0:
        D3DXVec3Normalize(&vLook, &vLook);
        D3DXMatrixRotationY(&RotY, D3DXToRadian(3));
        D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
        pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_FIRST, vPos, vLookReset);
        pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
        SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pAttack);
        vLookReset = {};

        D3DXMatrixIdentity(&RotY);
        D3DXMatrixRotationY(&RotY, D3DXToRadian(-3));
        D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
        pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_FIRST, vPos, vLookReset);
        pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
        SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pAttack);
        ++m_iSkillCnt;
        break;
    case 1:

        m_frameAttack += _DT;
        if (m_frameAttack > 0.2)
        {
            ++m_iSkillCnt;
            m_frameAttack = 0;
        }
        break;

    case 2:

        D3DXVec3Normalize(&vLook, &vLook);
        vLookReset = {};
        D3DXMatrixIdentity(&RotY);
        D3DXMatrixRotationY(&RotY, D3DXToRadian(6));
        D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
        pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_FIRST, vPos, vLookReset);
        pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
        SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pAttack);

        vLookReset = {};

        D3DXMatrixIdentity(&RotY);
        D3DXMatrixRotationY(&RotY, D3DXToRadian(-6));
        D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
        pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_FIRST, vPos, vLookReset);
        pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
        SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pAttack);

        m_eCheck = IDEL;
        m_eStatu = CL_LJUMP;
        m_iSkillCnt = 0;
        m_iFrameCnt = 0;
        m_iSkillDelay = 0;
        m_bMoveEffect = true;
        m_StartAttack = false;
        m_EndEffect = true;
        return true;
    }
    return false;
}
CLAttack* Cheonlog::Create_Leaf_Second(_vec3 vPos)
{
    CLAttack* pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_EXPLOSION, vPos, { 0,0,1 });
    return pAttack;
}

void Cheonlog::Create_Leaf_Third(_vec3 vPos)
{

    _vec3 vPlayerPos, vLook, vLookReset, vOrigin;
    _matrix RotY;
    CLAttack* pAttack = nullptr;

    vPos += { 1.1f, 1.5f, 3.7f };
    ++m_iBulletCnt;

    m_fRotY += 60;
    D3DXVec3Normalize(&vLook, &vLook);
    D3DXMatrixRotationY(&RotY, D3DXToRadian(m_fRotY));
    D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
    pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_SECOND, vPos, vLookReset);
    pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
    SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pAttack);



}
void Cheonlog::Create_Leaf_Third_S(_vec3 vPos)
{
    _vec3 vPlayerPos, vLook, vLookReset, vOrigin;
    _matrix RotY;
    CLAttack* pAttack = nullptr;

    vPos += { 1.1f, 1.5f, 3.7f };
    ++m_iBulletCnt;
    m_fRotY += 45;
    D3DXVec3Normalize(&vLook, &vLook);
    D3DXMatrixRotationY(&RotY, D3DXToRadian(m_fRotY));
    D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
    pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_THIRD, vPos, vLookReset);
    pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
    SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pAttack);

}
void Cheonlog::Create_Leaf_Four(_vec3 vPos, _float fRot)
{
    _vec3 vPlayerPos, vLook, vLookReset, vOrigin;
    _matrix RotY;
    CLAttack* pAttack = nullptr;

    vPos += { 1.1f, 1.5f, 3.3f };
    ++m_iBulletCnt;
    m_fRotY += 3;
    D3DXVec3Normalize(&vLook, &vLook);
    D3DXMatrixRotationY(&RotY, D3DXToRadian(fRot + m_fRotY));
    D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
    pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_FOUR, vPos, vLookReset);
    pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
    SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pAttack);
}

void Cheonlog::CL_Jump(const _float& _DT, _int iMaxCnt)
{
    _vec3 vRight, vUp(0, 1, 0);

    Component_Transform->Get_Info(INFO_POS, &vRight);
    m_frameTick += _DT;
    _int iRand = rand() % 3;
    _int iRandPM = rand() % 2;

    if (iRandPM == 0) iRand *= -1;

    vRight = { -1.f,0.f,(_float)iRand };

    Component_Transform->Move_Pos(&vRight, 8.f, _DT);
    if (m_frameTick > 0.1)
    {
        ++m_iFrameCnt;
        m_frameTick = 0;
    }

    if (m_iFrameCnt > iMaxCnt - 1)
    {
        m_iFrameCnt = 0;
        m_eStatu = CL_IDELR;
        m_iSkillDelay = 0;
    }
}
void Cheonlog::CL_JumpCenter(const _float& _DT, _int iMaxCnt)
{
    _vec3 vLook, vPos;
    Component_Transform->Get_Info(INFO_POS, &vPos);

    m_frameTick += _DT;
    //vLook = m_vCenter - vPos;
    vLook = { 2,0,2 };
    Component_Transform->Move_Pos(&vLook, 8.f, _DT);
    if (m_frameTick > 0.1)
    {
        ++m_iFrameCnt;
        m_frameTick = 0;
    }

    if (m_iFrameCnt > iMaxCnt - 1)
    {
        m_iFrameCnt = 0;
        m_eStatu = CL_IDELR;
        m_iSkillDelay = 0;
    }
}
void Cheonlog::Debug_ButtonStyle()
{
    ImGui::PushStyleColor(ImGuiCol_Button, D3DXCOLOR(0.0f, 0.f, 0.f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.8f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.5f, 0.7f, 0.7f));
}
void Cheonlog::Debug_Button(const char pName[32], _vec3* vPivot, _float iLinePivot)
{

    char tXP[32] = "X", tYP[32] = "Y", tZP[32] = "Z", tXM[32] = "X", tYM[32] = "Y", tZM[32] = "Z",
        tP[32] = "+", tM[32] = "-",
        tfXP[32] = "FX", tfYP[32] = "FY", tfZP[32] = "FZ", tfXM[32] = "FX", tfYM[32] = "FY", tfZM[32] = "FZ";

    strcat_s(tXP, 32, pName); strcat_s(tXP, 32, tP); strcat_s(tXM, 32, pName); strcat_s(tXM, 32, tM);
    strcat_s(tYP, 32, pName); strcat_s(tYP, 32, tP); strcat_s(tYM, 32, pName); strcat_s(tYM, 32, tM);
    strcat_s(tZP, 32, pName); strcat_s(tZP, 32, tP); strcat_s(tZM, 32, pName); strcat_s(tZM, 32, tM);

    strcat_s(tfXP, 32, pName); strcat_s(tfXP, 32, tP); strcat_s(tfXM, 32, pName); strcat_s(tfXM, 32, tM);
    strcat_s(tfYP, 32, pName); strcat_s(tfYP, 32, tP); strcat_s(tfYM, 32, pName); strcat_s(tfYM, 32, tM);
    strcat_s(tfZP, 32, pName); strcat_s(tfZP, 32, tP); strcat_s(tfZM, 32, pName); strcat_s(tfZM, 32, tM);

    ///////////////Á¤¼ö/////////////////
    {

        Debug_ButtonStyle();
        if (ImGui::Button(tXP))
        {
            vPivot->x += 1;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine((70.f + iLinePivot), 0.f);

        Debug_ButtonStyle();
        if (ImGui::Button(tXM))
        {
            vPivot->x -= 1;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine((140.f + iLinePivot), 0.f);

        Debug_ButtonStyle();
        if (ImGui::Button(tYP))
        {
            vPivot->y += 1;
        }

        ImGui::PopStyleColor(3);
        ImGui::SameLine(210.f + iLinePivot, 0.f);

        Debug_ButtonStyle();
        if (ImGui::Button(tYM))
        {
            vPivot->y -= 1;
        }

        ImGui::PopStyleColor(3);
        ImGui::SameLine(280.f + iLinePivot, 0.f);
        Debug_ButtonStyle();
        if (ImGui::Button(tZP))
        {
            vPivot->z += 1;
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine(350.f + iLinePivot, 0.f);

        Debug_ButtonStyle();
        if (ImGui::Button(tZM))
        {
            vPivot->z -= 1;
        }
        ImGui::PopStyleColor(3);
    }

    ///////////////½Ç¼ö/////////////////
    {

        Debug_ButtonStyle();
        if (ImGui::Button(tfXP))
        {
            vPivot->x += m_fPivot;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine(70.f + iLinePivot, 0.f);

        Debug_ButtonStyle();
        if (ImGui::Button(tfXM))
        {
            vPivot->x -= m_fPivot;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine(140.f + iLinePivot, 0.f);

        Debug_ButtonStyle();
        if (ImGui::Button(tfYP))
        {
            vPivot->y += m_fPivot;
        }

        ImGui::PopStyleColor(3);
        ImGui::SameLine(210.f + iLinePivot, 0.f);

        Debug_ButtonStyle();
        if (ImGui::Button(tfYM))
        {
            vPivot->y -= m_fPivot;
        }

        ImGui::PopStyleColor(3);
        ImGui::SameLine(280.f + iLinePivot, 0.f);
        Debug_ButtonStyle();
        if (ImGui::Button(tfZP))
        {
            vPivot->z += m_fPivot;
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine(350.f + iLinePivot, 0.f);

        Debug_ButtonStyle();
        if (ImGui::Button(tfZM))
        {
            vPivot->z -= m_fPivot;
        }
        ImGui::PopStyleColor(3);
    }
}


Cheonlog* Cheonlog::Create(LPDIRECT3DDEVICE9 _GRPDEV, _vec3 vPos) {
    Cheonlog* CL = new Cheonlog(_GRPDEV);
    if (FAILED(CL->Ready_GameObject(vPos))) {
        MSG_BOX("Cannot Create Cheonlog.");
        Safe_Release(CL);
        return nullptr;
    }
    return CL;
}
void Cheonlog::Free()
{
    for (_int i = 0; i < CL_END; ++i)
    {
        for (auto& iter : m_vecCheonlogTexture[i])
        {
            Safe_Release(iter);
        }
        m_vecCheonlogTexture[i].clear();
    }

    GameObject::Free();
}