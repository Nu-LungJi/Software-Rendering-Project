#include "../Include/PCH.h"

Cheonlog::Cheonlog(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV),m_bBgm(false), m_bDead(false), m_bStartPattern(false), m_iNextSkill(0), m_fAttackSecondTick(0.f), m_frameAttack(0.f), m_iBulletCnt(0), m_fRotY(0.f), m_iSkillDelay(0), m_bMoveEffect(false), m_iStatuCnt(0), m_iSkillMaxCnt(0), m_iSkillCnt(0), m_StartAttack(false), m_EndEffect(true), m_vDebug(0, 0, 0), m_pTarget(nullptr), m_frameTick(0.f), m_iFrameCnt(0), m_eCheck(CHECK_END), m_eStatu(CL_END) {}
Cheonlog::Cheonlog(const GameObject& _RHS)    : GameObject(_RHS), m_bDeadBgm(false), m_pTarget(nullptr), m_bCrystal(false){}
Cheonlog::~Cheonlog() {}

HRESULT Cheonlog::Ready_GameObject(_vec3 vPos) {
	if (FAILED(Component_Initialize())) return E_FAIL;

	Texture_Initalize(8, L"Spr_Boss_Cheonlog_Shining_Stand_R_0%d.png",    CL_IDELR);
	Texture_Initalize(6, L"Spr_Boss_Cheonlog_Shining_Jump_L_045_0%d.png", CL_LJUMP);
	Texture_Initalize(6, L"Spr_Boss_Cheonlog_Shining_LU_135_0%d.png",    CL_LUJUMP);
	Texture_Initalize(6, L"Spr_Boss_Cheonlog_Shining_Jump_R_045_0%d.png", CL_RUJUMP);
	Texture_Initalize(6, L"Spr_Boss_Cheonlog_Shining_RU_135_0%d.png",     CL_RJUMP);
	Texture_Initalize(26, L"Spr_Boss_Cheonlog_Death_0%d.png",			  CL_DEAD);
	m_fPivot = 0.1f;
	m_eCheck = IDEL;
	m_eStatu = SPAWN;
	m_vCenter = vPos;
	Make_TextureList(L"Spr_Bullet_LaulaStandardBullet_0",LEAF_ATTACK::LEAF_FIRST);
	Make_TextureList(L"Spr_Bullet_Cheonlog_DivideFlowerLv3_Black_0", LEAF_ATTACK::LEAF_SECOND);
	Make_TextureList(L"Spr_Bullet_Cheonlog_DivideFlowerLv3_White_0", LEAF_ATTACK::LEAF_THIRD);
	Make_TextureList(L"Spr_Effect_Cheonlog_BigExplosione_Birth", LEAF_ATTACK::LEAF_EXPLOSION);
	Make_TextureList(L"Spr_Bullet_Cheonlog_DivideFlowerLv2_0", LEAF_ATTACK::LEAF_FOUR);
	Make_TextureList(L"Spr_Ui_Effect_ChaosGazeCircleEffect01_0", LEAF_ATTACK::LEAF_BOOM_CIRCLE);

	Make_EffectTextureList(L"Spr_Effect_Cheonlog_AttackMode_Rage_0", CL_EFFECT::LEFT_HORN);
	Make_EffectTextureList(L"Spr_Effect_Cheonlog_AttackMode_Rage_0", CL_EFFECT::RIGHT_HORN);		
	Make_EffectTextureList(L"Spr_Effect_Cheonlog_AttackModeEffect_0", CL_EFFECT::CL_BODY);		
	Make_EffectTextureList(L"Spr_Effect_Cheonlog_BaseBullet_Birth01_0", CL_EFFECT::LEAF_FIRST);
	Make_EffectTextureList(L"Spr_Effect_Cheonlog_ChargeAccelReturn_Birth01_0", CL_EFFECT::LEAF_CHARGING);
	Make_EffectTextureList(L"Spr_Effect_Cheonlog_RadialCrossSplit_Loop_0", CL_EFFECT::LEAF_SPIN);
	Make_EffectTextureList(L"Spr_Bullet_Cheonlog_DivideFlower_Death_0", CL_EFFECT::LEAF_SPIN_DEATH);
	Make_EffectTextureList(L"Spr_Effect_Cheonlog_Appear_Electric01_0", CL_EFFECT::SPAWN_BOOM);
	Make_EffectTextureList(L"Green_Evil_Thunder0", CL_EFFECT::SPAWN_THUNDER);
	Make_EffectTextureList(L"Green_Shader0", CL_EFFECT::SPAWN_BOOM_CIRCLE);
	Make_EffectTextureList(L"Cheonlog_Spawn_L0", CL_EFFECT::SPAWN_L);
	Make_EffectTextureList(L"Cheonlog_Spawn0", CL_EFFECT::SPAWN_R);

	Set_ObjectTag(L"CheonLog");
	Component_Transform->Set_Pos(vPos);
	m_bSpawn = true;
	CollisionManager::GetInstance()->Add_ColliderObject(this);
	//BossUI* pBossUi = BossUI::Create(GRPDEV, BOSSUI_INFO::CHLG,this);
	//pBossUi->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_UI);
	//pBossUi->Set_ObjectTag(L"BossUI");
	//SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pBossUi);
	dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Set_BossMaxHP(Component_Collider->Get_Hp());

	/////////////obj pooling////////
	for (_int i = 0; i < 40; ++i)
	{
		GameObject* pAttack;
		pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_EXPLOSION, { 0,0,0 }, { 0,0,0 });
		pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		m_vecPoolBullet[(int)LEAF_ATTACK::LEAF_EXPLOSION].push_back(pAttack);
	}
	for (_int i = 0; i < 100; ++i)
	{
		GameObject* pAttack;
		pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_BOOM_CIRCLE, { 0,0,0 }, { 0,0,0 },false,false);
		pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		m_vecPoolBullet[(int)LEAF_ATTACK::LEAF_BOOM_CIRCLE].push_back(pAttack);
	}
	for (_int i = 0; i < 40; ++i)
	{
		GameObject* pAttack;
		pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_FIRST, { 0,0,0 }, { 0,0,0 });
		pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		m_vecPoolBullet[(int)LEAF_ATTACK::LEAF_FIRST].push_back(pAttack);
	}
	for (_int i = 0; i < 80; ++i)
	{
		GameObject* pAttack;
		pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_SECOND, { 0,0,0 }, { 0,0,0 });
		pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		m_vecPoolBullet[(int)LEAF_ATTACK::LEAF_SECOND].push_back(pAttack);
	}
	for (_int i = 0; i < 80; ++i)
	{
		GameObject* pAttack;
		pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_THIRD, { 0,0,0 }, { 0,0,0 });
		pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		m_vecPoolBullet[(int)LEAF_ATTACK::LEAF_THIRD].push_back(pAttack);
	}
	for (_int i = 0; i < 80; ++i)
	{
		GameObject* pAttack;
		pAttack = CLAttack::Create(GRPDEV, LEAF_ATTACK::LEAF_FOUR, { 0,0,0 }, { 0,0,0 });
		pAttack->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_MONSTER_BULLET);
		m_vecPoolBullet[(int)LEAF_ATTACK::LEAF_FOUR].push_back(pAttack);
	}

	SoundManager::GetInstance()->Stop_AllSound();
	SoundManager::GetInstance()->Play_Sound(L"CheonLog/Stage1-2_chunlog_normal__Start.wav", CHANNELID::SOUND_BGM01, 0.4f,FALSE);
	
	return S_OK;
}
HRESULT	 Cheonlog::Make_TextureList(wstring _FileName, LEAF_ATTACK eid)
{
	INT FRAME = 0;

	while (++FRAME) {
		wstring FileName = _FileName + to_wstring(FRAME) + L".dds";
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { TEX->AddRef();  m_vecBullet[(int)eid].push_back(TEX); }
	}

	return S_OK;
}
HRESULT Cheonlog::Make_EffectTextureList(wstring _FileName, CL_EFFECT eid)
{
	INT FRAME = 0;

	while (++FRAME) {
		wstring FileName = _FileName + to_wstring(FRAME) + L".dds";
		IDirect3DBaseTexture9* TEX = ResourceManager::GetInstance()->Find_Texture(FileName.c_str());
		if (TEX == nullptr) break;
		else { TEX->AddRef();  m_vecEffect[(int)eid].push_back(TEX); }
	}

	return S_OK;
}
INT   Cheonlog::Update_GameObject(const _float& _DT)
{
	if (m_eStatu == CL_DEAD && m_iFrameCnt >= m_vecCheonlogTexture[m_eStatu].size() - 1)
	{

		_vec3 vPos = *Component_Transform->Get_Position();
	
		//dynamic_cast<BossUI*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"BossUI"))->Set_Dead();
		// 광윤 추가 ▼
		dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Set_EnableDisplayHPBar(FALSE);

		Spawner* pObj = Spawner::Create(GRPDEV,TILE_SIDE::TILE_FRONT,TILE_SPAWNER::ITEM_SPAWN1,vPos);
		pObj->Set_ObjectTag(L"SpawnITem");
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::UI, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_ITEM, vPos, FALSE));
		SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(pObj);

		for (_int i = 0; i < (int)LEAF_ATTACK::LEAF_END; ++i)
		{
			for (auto& iter : m_vecOrignBullet[i])
			{
				CollisionManager::GetInstance()->Delete_ColliderObject(iter);
				Safe_Release(iter);
			}
			m_vecOrignBullet[i].clear();
		}
		Set_ObjectDead(TRUE);
	}
	if (Component_Collider->Get_Hp() <= 0)
	{
		CollisionManager::GetInstance()->Delete_ColliderObject(this);

		m_bStartPattern = false;
		m_eStatu = CL_DEAD;
	}

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
		
		if(m_eStatu == CL_DEAD && m_iFrameCnt == m_vecCheonlogTexture[m_eStatu].size() - 1)
			Component_Transform->Set_Pos({ matWorld._41 , -0.4f , matWorld._43 });
		else
			Component_Transform->Set_Pos({ matWorld._41 , 0.8f , matWorld._43 });
	}

	for (size_t i = 0; i < (int)LEAF_ATTACK::LEAF_END; ++i)
	{
		for (auto iter = m_vecOrignBullet[i].begin(); iter != m_vecOrignBullet[i].end();)
		{
			(*iter)->Update_GameObject(_DT);
			if (dynamic_cast<CLAttack*>(*iter)->Get_bPool())
			{
				dynamic_cast<CLAttack*>(*iter)->Set_bPool();
				CollisionManager::GetInstance()->Delete_ColliderObject((*iter));

				m_vecPoolBullet[i].push_back((*iter));
				iter = m_vecOrignBullet[i].erase(iter);
			}
			else
				++iter;
			
		}
			
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
	for (size_t i = 0; i < (int)LEAF_ATTACK::LEAF_END; ++i)
	{
		for (auto& iter : m_vecOrignBullet[i])
			iter->LateUpdate_GameObject(_DT);
	}
}
void Cheonlog::Render_GameObject() {
	if (m_eStatu == SPAWN)
		return;

	
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

    Set_Statu();
    Component_Buffer->Render_Buffer();

    GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT Cheonlog::Component_Initialize() {
    Component_Buffer = ADD_COMPONENT_RECTTEX;
    Component_Transform = ADD_COMPONENT_TRANSFORM;

    Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
    Component_Transform->Set_Scale(3.f, 3.f, 3.f);
    Component_Transform->Set_Pos(0.f, 0.5f, 0.f);
    //좌우반전
    //Component_Transform->Set_Scale(-2.f, 2.f, 2.f);

	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(4.f, 4.f, 4.f);
	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);
	//좌우반전
	//Component_Transform->Set_Scale(-2.f, 2.f, 2.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Hp(12000);

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
	case CL_RUJUMP:
		GRPDEV->SetTexture(0, m_vecCheonlogTexture[m_eStatu][m_iFrameCnt]);
		break;
	case CL_LUJUMP:
		GRPDEV->SetTexture(0, m_vecCheonlogTexture[m_eStatu][m_iFrameCnt]);
		break;
	case CL_DEAD:
		GRPDEV->SetTexture(0, m_vecCheonlogTexture[m_eStatu][m_iFrameCnt]);
		break;
	}
}
void Cheonlog::Change_Statu(const _float& _DT, _int iMaxCnt)
{
	_vec3 vPos,vScale;
	Component_Transform->Get_Info(INFO_POS, &vPos);
	vScale = *Component_Transform->Get_Scale();

	//이동 모션 관련
	switch (m_eStatu)
	{
	case SPAWN:
			Create_Cheonlog(_DT,vPos);
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
		if (m_bBgm)
		{
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_Move_Jump.wav", CHANNELID::SOUND_EFFECT02, 0.4f);
			m_bBgm = false;
		}
		CL_Jump(_DT, iMaxCnt);
		break;
	case CL_RJUMP:
		if (m_bBgm)
		{
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_Move_Jump.wav", CHANNELID::SOUND_EFFECT02, 0.4f);
			m_bBgm = false;
		}
		CL_JumpCenter(_DT, iMaxCnt);
		break;
	case CL_DEAD:
		if (!m_bDeadBgm)
		{
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Chunlog_Pattern3-1.wav", CHANNELID::SOUND_EFFECT02, 0.5f);
			m_bDeadBgm = true;
		}
		m_frameTick += _DT;
		if (m_frameTick > 0.1)
		{
			++m_iFrameCnt;
			vScale -= {0.1f, 0.1f, 0.1f};
			Component_Transform->Set_Scale(vScale);

			m_frameTick = 0;
		}
		if (m_iFrameCnt > iMaxCnt - 1)
		{
			Component_Transform->Set_Pos(vPos.x, -0.4, vPos.z);
			m_iFrameCnt = iMaxCnt - 1;
			m_bDead = true;
		}
		break;
	case CL_RUJUMP:
		if (m_bBgm)
		{
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_Move_Jump.wav", CHANNELID::SOUND_EFFECT02, 0.4f);
			m_bBgm = false;
		}
		CL_Jump(_DT, iMaxCnt);
		break;
	case CL_LUJUMP:
		if (m_bBgm)
		{
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_Move_Jump.wav", CHANNELID::SOUND_EFFECT02, 0.4f);
			m_bBgm = false;
		}
		CL_JumpCenter(_DT, iMaxCnt);
		break;
	}
	
	//공격 관련
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
		if (m_bBgm)
		{
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Chunlog_AntlerSpin.wav", CHANNELID::SOUND_EFFECT02, 0.4f);
			m_bBgm = false;
		}
		AttackLeaf_Third(_DT, vPos);
		break;
	case ATTACK_D:
		AttackLeaf_Four(_DT, vPos);
		break;
	}


}

void Cheonlog::Change_Pattern(const _float& _DT)
{
	if (m_iNextSkill < 3 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
	{
		m_framePattern += _DT;
		if (m_framePattern > 6)
		{
				Reset_Pattern(ATTACK_A, CL_IDELR);
			return;
		}
	}
	
	if (m_iNextSkill == 3 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
	{
		m_framePattern += _DT;
		if (m_framePattern > 9)
		{
			if(m_eCurr == CL_LJUMP)
				Reset_Pattern(IDEL, CL_RJUMP);
			else
				Reset_Pattern(IDEL, CL_LUJUMP);

		}
	}
	if (m_iNextSkill == 4 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
	{
		m_framePattern += _DT;
		if (m_framePattern > 12)
		{
			Reset_Pattern(ATTACK_B, CL_IDELR);
			return;
		}
	}
	if (m_iNextSkill == 5 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
	{
		m_framePattern += _DT;
		if (m_framePattern > 21)
		{

			Reset_Pattern(ATTACK_C, CL_IDELR);
			return;
		}
	}
	if (m_iNextSkill < 8 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
	{
		m_framePattern += _DT;
		if (m_framePattern >25)
		{
			Reset_Pattern(ATTACK_A, CL_IDELR);
			return;
		}
	}
	if (m_iNextSkill == 8 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
	{
		m_framePattern += _DT;
		if (m_framePattern > 30)
		{
			Reset_Pattern(ATTACK_D, CL_IDELR);
			return;
		}
	}
	if (m_iNextSkill >= 9 && m_eCheck == IDEL && m_eStatu == CL_IDELR)
	{
		m_framePattern += _DT;
		if (m_framePattern > 10)
		{
			m_iNextSkill = 0;
			m_framePattern = 0;
		}
	}
}
void Cheonlog::Reset_Pattern(CL_CHECK eCheck, CL_STATU eStatu)
{
	m_iFrameCnt = 0;
	m_bBgm = true;
	m_EndEffect = true;
	m_eCheck = eCheck;
	m_eStatu = eStatu;
	++m_iNextSkill;
}

void Cheonlog::AttackLeaf_First(const _float& _DT, _vec3 vPos)
{
	//뿔 위에있는 수정 + 뿔 주변에있는 전기 이펙트
	Create_Crystal();
	//나뭇잎 쏘기전 이펙트
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
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/No.033_Cheonlog'sHorn_Charge1.wav", CHANNELID::SOUND_EFFECT02, 0.3f);

			Create_Pool(LEAF_ATTACK::LEAF_BOOM_CIRCLE, { vPos.x,-0.2f,vPos.z }, { 0,0,0 },0.2f);
			Create_Pool(LEAF_ATTACK::LEAF_EXPLOSION, { vPos.x  , 2.f, vPos.z }, { 0,0,1 });
			m_EndEffect = false;
		}
		break;
	case 1:
		if (m_EndEffect)
		{
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Chunlog_pattern_range_01.wav", CHANNELID::SOUND_EFFECT02, 0.5f);
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/No.033_Cheonlog'sHorn_Charge1.wav", CHANNELID::SOUND_EFFECT02, 0.3f);
		for (_int i = 0; i < 4; ++i)
		{
			D3DXMatrixRotationY(&matRotY, D3DXToRadian(i * 90));
			D3DXVec3TransformNormal(&vLookReset, &vLook, &matRotY);
		
			vPos += vLookReset * 8;
		
			Create_Pool(LEAF_ATTACK::LEAF_BOOM_CIRCLE, { vPos.x,-0.2f,vPos.z + _float(i * 0.001) }, { 0,0,0 }, 0.08f);
			Create_Pool(LEAF_ATTACK::LEAF_EXPLOSION, { vPos.x  , 2.f, vPos.z }, { 0,0,1 });
			vLookReset = { 0,0,0 };
			vPos = vOrigin;
		}	
			m_EndEffect = false;
		}
		break;
	case 2:
		if (m_EndEffect)
		{
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Chunlog_pattern_range_01.wav", CHANNELID::SOUND_EFFECT02, 0.5f);
			SoundManager::GetInstance()->Play_Sound_Once(L"No.033_Cheonlog'sHorn_Charge1.wav", CHANNELID::SOUND_EFFECT02, 0.3f);
			for (_int i = 0; i < 9; ++i)
			{
				D3DXMatrixRotationY(&matRotY, D3DXToRadian(i * 40));
				D3DXVec3TransformNormal(&vLookReset, &vLook, &matRotY);

				vPos += vLookReset * 16;

				Create_Pool(LEAF_ATTACK::LEAF_BOOM_CIRCLE, { vPos.x,-0.2f,vPos.z + _float(i * 0.001) }, { 0,0,0 }, 0.08f);
				Create_Pool(LEAF_ATTACK::LEAF_EXPLOSION,{ vPos.x  , 2.f, vPos.z },{0,0,1});
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
		
		if (m_iSkillCnt > 30)
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
			++m_iNextSkill;
			m_iSkillDelay = 0;
			m_iFrameCnt = 0;
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
		SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_Lightning.wav", CHANNELID::SOUND_EFFECT02, 0.2f);

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
		++m_iSkillDelay;
		return;
	case 9:
		D3DXMatrixRotationY(&matRotY, D3DXToRadian(180));
		D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
		vPos += vLook * 8;
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x, 6, vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, TRUE));
		SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_Lightning.wav", CHANNELID::SOUND_EFFECT06, 0.2f);
		++m_iSkillDelay;

		// 광윤 추가 ▼
		dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Set_EnableBossTitle(FALSE);

		break;
	}
		
	if (m_frameAttack > 0.3)
	{
		++m_iSkillDelay;
		m_frameAttack = 0;
		if (m_iSkillDelay > 9)
		{
			m_fRotY += 20;
			D3DXMatrixRotationY(&matRotY, D3DXToRadian(100+m_fRotY));
			D3DXVec3TransformNormal(&vLook, &vLook, &matRotY);
			vPos += vLook * 6;
			EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x, 6, vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, FALSE));
			SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_Lightning.wav", CHANNELID::SOUND_EFFECT05, 0.2f);
		}
		
	}
	
		
}
void Cheonlog::Create_Cheonlog_After(const _float& _DT, _vec3 vPos)
{
	_vec3 vLook, vLookReset = { 0,0,0 },vOrigin;
	_matrix matRotY;
	Component_Transform->Get_Info(INFO_LOOK, &vLook);
	vOrigin = { vPos.x, vPos.y ,vPos.z + 6 };
	vPos = vOrigin;
	m_frameAttack += _DT;
	switch (m_iSkillDelay)
	{
	case 5:
		for (_int i = 0; i < 12; ++i)
		{
			D3DXMatrixRotationY(&matRotY, D3DXToRadian(i*30));
			D3DXVec3TransformNormal(&vLookReset, &vLook, &matRotY);
			
			vPos += vLookReset * 4;
			EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::SPAWN_THUNDER, { vPos.x, 6, vPos.z }, TRUE, { 2.3f,11.5f,2.5f }, { 55,0,0 }, 0.02f, { 0,0,1 }, FALSE));
			vLookReset = { 0,0,0 };
			if (i == 11) SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Cheonlog_Pattern_Range_01_04.wav", CHANNELID::SOUND_EFFECT01, 0.5f);
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
	if (m_iSkillDelay > 12)
	{
		// 광윤 추가 ▼
		dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Set_FadeOption(FALSE, 4.f);
		dynamic_cast<MainUI*>(SceneManager::GetInstance()->Get_GameObject(L"MainUI"))->Set_EnableDisplayHPBar(TRUE);

		m_bStartPattern = true; m_eCheck = IDEL;
		m_bSpawn = false;
		dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"))->Set_Tracking_Player(false);

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

        //왼쪽뿔
        vCur = vPos;
        vCur += { 0.0f, 0.5f, 2.7f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::LEFT_HORN, vCur, FALSE));

        //오른쪽뿔
        vCur = vPos;
        vCur += { 2.3f, 0.0f, 3.2f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::RIGHT_HORN, vCur, FALSE));

        //몸통
        vCur = vPos;
        vCur += { -1.2f, 0.2f, 0.8f };
        EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, CLEffect::Create(GRPDEV, CL_EFFECT::CL_BODY, vCur, FALSE));

        m_bCrystal = true;
    }
}

_bool Cheonlog::Create_Leaf(const _float& _DT)
{
	_vec3 vPos, vPlayerPos, vLook, vLookS, vLookReset,vOrigin;
	_matrix RotY,RotX;
	CLAttack* pAttack = nullptr;
	Component_Transform->Get_Info(INFO_POS, &vPos);
	dynamic_cast<Transform*>(SceneManager::GetInstance()->Get_GameObject(L"Player")->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Info(INFO_POS, &vPlayerPos);
	vPos += { 1.1f, 1.5f, 3.7f }; // 현재 뿔 가운데 위치
	//나뭇잎 4개발싸 히히
	vOrigin = vPos;
	vLook = vPlayerPos - vPos;
	switch (m_iSkillCnt)
	{
	case 0:
		D3DXVec3Normalize(&vLook, &vLook);
		D3DXMatrixRotationY(&RotY, D3DXToRadian(3));
		D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
		Create_Pool(LEAF_ATTACK::LEAF_FIRST, vPos, vLookReset);
		vLookReset = {};

		D3DXMatrixIdentity(&RotY);
		D3DXMatrixRotationY(&RotY, D3DXToRadian(-3));
		D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
		Create_Pool(LEAF_ATTACK::LEAF_FIRST, vPos, vLookReset);
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
		Create_Pool(LEAF_ATTACK::LEAF_FIRST, vPos, vLookReset);

		vLookReset = {};

		D3DXMatrixIdentity(&RotY);
		D3DXMatrixRotationY(&RotY, D3DXToRadian(-6));
		D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
		Create_Pool(LEAF_ATTACK::LEAF_FIRST, vPos, vLookReset);

		m_eCheck = IDEL;
		
		_int iRand = rand() % 2;
		if (iRand == 0)
		{
			m_eStatu = CL_LJUMP;
			m_eCurr = CL_LJUMP;
		}
		else
		{
			m_eStatu = CL_RUJUMP;
			m_eCurr = CL_RUJUMP;
		}
			
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


void Cheonlog::Create_Leaf_Third(_vec3 vPos)
{
	_vec3 vPlayerPos, vLook, vLookReset, vOrigin;
	_matrix RotY;
	CLAttack* pAttack = nullptr;

	vPos += { 1.1f, 1.5f, 3.7f };
	++m_iBulletCnt;

	m_fRotY  += 60;
	D3DXVec3Normalize(&vLook, &vLook);
	D3DXMatrixRotationY(&RotY, D3DXToRadian(m_fRotY));
	D3DXVec3TransformNormal(&vLookReset, &vLook, &RotY);
	Create_Pool(LEAF_ATTACK::LEAF_SECOND, vPos, vLookReset);
	
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
	Create_Pool(LEAF_ATTACK::LEAF_THIRD, vPos, vLookReset);
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
	Create_Pool(LEAF_ATTACK::LEAF_FOUR, vPos, vLookReset);
}
void Cheonlog::Create_Pool(LEAF_ATTACK eid, _vec3 vPos, _vec3 vL, _float fFrameSpeed)
{
	dynamic_cast<CLAttack*>(m_vecPoolBullet[(int)eid].back())->Set_Look(vL, vPos,false, fFrameSpeed);
	CollisionManager::GetInstance()->Add_ColliderObject(m_vecPoolBullet[(int)eid].back());
	m_vecOrignBullet[(int)eid].push_back(m_vecPoolBullet[(int)eid].back());
	m_vecPoolBullet[(int)eid].pop_back();
}
void Cheonlog::CL_Jump(const _float& _DT, _int iMaxCnt)
{
		_vec3 vRight,vUp(0,1,0);

	Component_Transform->Get_Info(INFO_POS, &vRight);
	m_frameTick += _DT;
	_int iRand   = rand() % 3;
	_int iRandPM = rand() % 2;

	if(iRandPM == 0) iRand *= -1;

	if(m_eCurr == CL_LJUMP)
		vRight = { -1.f,0.f,(_float)iRand };
	else
		vRight = { 1.f,0.f,(_float)iRand };

	Component_Transform->Move_Pos(&vRight, 8.f, _DT);
	if (m_frameTick > 0.1)
	{
		++m_iFrameCnt;
		m_frameTick = 0;
	}

	if (m_iFrameCnt > iMaxCnt - 1)
	{
		m_iFrameCnt = 0;
		SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Chunlog_moveEnd.wav", CHANNELID::SOUND_EFFECT02, 0.3f);

		m_eStatu = CL_IDELR;
		m_iSkillDelay = 0;
	}
}
void Cheonlog::CL_JumpCenter(const _float& _DT, _int iMaxCnt)
{
	_vec3 vLook, vPos;
	Component_Transform->Get_Info(INFO_POS, &vPos);
	
	m_frameTick += _DT;
	vLook = m_vCenter - vPos;
	
	Component_Transform->Move_Pos(&vLook, 8.f, _DT);
	if (m_frameTick > 0.1)
	{
		++m_iFrameCnt;
		m_frameTick = 0;
	}

	if (m_iFrameCnt > iMaxCnt - 1)
	{
		m_iFrameCnt = 0;
		SoundManager::GetInstance()->Play_Sound_Once(L"CheonLog/Chunlog_moveEnd.wav", CHANNELID::SOUND_EFFECT02, 0.3f);

		m_eStatu = CL_IDELR;
		m_iSkillDelay = 0;
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

	for (_int i = 0; i < (int)LEAF_ATTACK::LEAF_END; ++i)
	{
		for(auto& iter : m_vecBullet[i])
		{
			Safe_Release(iter);
		}
		m_vecBullet[i].clear();
	}
	for (_int i = 0; i < (int)CL_EFFECT::CL_EFFECTEND; ++i)
	{
		for (auto& iter : m_vecEffect[i])
		{
			Safe_Release(iter);
		}
		m_vecEffect[i].clear();
	}
	for (_int i = 0; i < (int)LEAF_ATTACK::LEAF_END; ++i)
	{
		for (auto& iter : m_vecOrignBullet[i])
		{
			Safe_Release(iter);
		}
		m_vecOrignBullet[i].clear();
	}

	for (_int i = 0; i < (int)LEAF_ATTACK::LEAF_END; ++i)
	{
		for (auto& iter : m_vecPoolBullet[i])
		{
			CollisionManager::GetInstance()->Delete_ColliderObject(iter);
			Safe_Release(iter);
		}
		m_vecPoolBullet[i].clear();
	}
    GameObject::Free();
}