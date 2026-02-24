#include "../Include/PCH.h"

Alert::Alert(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Alert::Alert(const GameObject& _RHS) : GameObject(_RHS) {}
Alert::~Alert() {}

HRESULT Alert::Ready_GameObject() {
	if (FAILED(Component_Initialize())) return E_FAIL;
	
	return S_OK;
}
INT	Alert::Update_GameObject(const _float& _DT)
{

	// <플레이어 업데이트 시점>
	GameObject::Update_GameObject(_DT);

	m_tInfo.fTimer[0] += _DT;

	if (!m_tInfo.bTrigger[0])
	{
		MYPOS->y = 0.002f;
		if (m_tInfo.pGameObj[2] == nullptr)
		{
			m_tInfo.pGameObj[2] = Monster::Create<Alert>(GRPDEV, { MYPOS->x, MYPOS->y - 0.001f, MYPOS->z });

			if (m_tInfo.pGameObj[2] == nullptr)
			{
				ObjectDead = true;
				return -1;
			}

			Alert* pAlert = static_cast<Alert*>(m_tInfo.pGameObj[2]);

			pAlert->Get_Info()->pGameObj[0] = m_tInfo.pGameObj[0];
			pAlert->Get_Info()->pGameObj[1] = m_tInfo.pGameObj[1];
			pAlert->Get_Info()->bTrigger[0] = true;

			pAlert->Get_Info()->fTimer[1] = m_tInfo.fTimer[1];
			_vec3 vScale = *SCALE(m_tInfo.pGameObj[1]);
			vScale.y = vScale.x;

			*SCALE(m_tInfo.pGameObj[2]) = vScale;

			Monster::Add_Monster_to_Scene(m_tInfo.pGameObj[2], L"Alert", GAMEOBJECT_TYPE::MONSTER_EFFECT);
		}
	}


	if (m_tInfo.fTimer[0] >= m_tInfo.fTimer[1])
	{
		ObjectDead = true;
	}

	if (ObjectDead)
	{
		if (m_tInfo.pGameObj[2] != nullptr)
		{
			m_tInfo.pGameObj[2]->Set_ObjectDead(true);
			m_tInfo.pGameObj[2] = nullptr;
		}

		return -1;
	}


	RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}
VOID Alert::LateUpdate_GameObject(const _float& _DT) {

	GameObject::LateUpdate_GameObject(_DT);

	AlphaZValue = Monster::BillBoard(Component_Transform, GRPDEV,{1.f,0.f,0.f},0);
}
VOID Alert::Render_GameObject() {
	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

	if(!ObjectDead)
	{
		GRPDEV->SetTexture(0, ResourceManager::GetInstance()->Find_Texture(L"AlertCircle.png"));
		Component_Buffer->Render_Buffer();
	}

	GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
HRESULT Alert::Component_Initialize() {

	Component_Buffer = ADD_COMPONENT_RECTTEX;
	Component_Transform = ADD_COMPONENT_TRANSFORM;

	Component_Transform->Set_Pos(10.f, 0.5f, 0.f);
	Component_Transform->Set_Rotation(0.f, 0.f, 0.f);
	Component_Transform->Set_Scale(0.5f, 0.313f, 1.f);

	return S_OK;
}
Alert* Alert::Create(LPDIRECT3DDEVICE9 _GRPDEV) {
	Alert* MST = new Alert(_GRPDEV);
	if (FAILED(MST->Ready_GameObject())) {
		MSG_BOX("Cannot Create Alert.");
		Safe_Release(MST);
		return nullptr;
	}
	return MST;
}

VOID Alert::Free() {

	GameObject::Free();
}
