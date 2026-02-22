#include "../Include/PCH.h"

Bullet_Chain::Bullet_Chain(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Bullet_Chain::Bullet_Chain(const GameObject& _RHS)		: GameObject(_RHS)		{}
Bullet_Chain::~Bullet_Chain() {}

HRESULT Bullet_Chain::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	return S_OK;
}
INT	Bullet_Chain::Update_GameObject(const _float& _DT)
{
	
	if (m_tInfo.bTrigger[0])
	{
		if (FAILED(Monster::Set_TextureList(L"Spr_Bullet_Chain01", &m_tInfo)))
		{
			ObjectDead = true;
			return 0;
		}
	}
	else
	{
		if (FAILED(Monster::Set_TextureList(L"Spr_Bullet_Chain02", &m_tInfo)))
		{
			ObjectDead = true;
			return 0;
		}
	}
	
	MYPOS->y = 0.5f;
	Component_Collider->Set_Scale(MYSCALE->x, 1.f * 0.5f, MYSCALE->y * 0.5f);

	m_tInfo.fTimer[0] += _DT;
	if (m_tInfo.fTimer[0] >= 2.f)
	{
		MonsterEffect* pEffect = MonsterEffect::Create(GRPDEV, MONSTER_EFFECT::BULLET_STANDARD_DEATH, *MYPOS, FALSE, 1.2f);

		_vec3 vEffectScale = { MYSCALE->x, MYSCALE->x, MYSCALE->x };
		*static_cast<Transform*>(pEffect->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Scale() = vEffectScale;
		EffectManager::GetInstance()->Append_Effect(EFFECT_OWNER::MONSTER, pEffect);

		ObjectDead = true;
		return 0;
	}

	GameObject::Update_GameObject(_DT);

	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

VOID Bullet_Chain::LateUpdate_GameObject(const _float& _DT) {
	GameObject::LateUpdate_GameObject(_DT);

	Component_Transform->Move_Pos(&m_tInfo.vDirection, m_tInfo.fSpeed, _DT);



	m_tInfo.Textureinfo._frameTick += _DT;

	if (m_tInfo.Textureinfo._frameTick > FRAMETICK)
	{
		m_tInfo.Textureinfo._frameTick = 0.f;
		++m_tInfo.Textureinfo._frame %= m_tInfo.Textureinfo._Endframe;
	}

	Monster::BillBoard(Component_Transform, GRPDEV, { -m_tInfo.vDirection.x ,0.f, -m_tInfo.vDirection.z }, false);
	
}
VOID Bullet_Chain::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());
	
	GRPDEV->SetTexture(0, m_tInfo.Textureinfo._vecTexture[m_tInfo.Textureinfo._frame]);
	Component_Buffer->Render_Buffer();

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT Bullet_Chain::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(0.11f, 0.1f, 1.f);
	Component_Transform->Set_Pos(0.f, 0.5f, 0.f);

	Component_Collider = ADD_COMPONENT_COLLIDER;
	Component_Collider->Set_CenterPos(Component_Transform);
	Component_Collider->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}

Bullet_Chain* Bullet_Chain::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Bullet_Chain* MST = new Bullet_Chain(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create Bullet_Chain.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}
VOID Bullet_Chain::Free()
{
	GameObject::Free();
}