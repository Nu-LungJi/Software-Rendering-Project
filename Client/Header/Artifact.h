#pragma once
#include "GameObject.h"

class Artifact : public GameObject
{
private:
	explicit Artifact(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit Artifact(const GameObject& _RHS);
	virtual ~Artifact();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			INT			Update_GameObject(const _float& _DT);
	virtual			VOID		LateUpdate_GameObject(const _float& _DT);
	virtual			VOID		Render_GameObject();

public:
	static Artifact* Create(LPDIRECT3DDEVICE9 _GRPDEV);

	VOID	Artifact_Effect();

	int		Get_ItemIdx() { return itemIdx; }
	VOID	Set_ItemIdx(int idx) { itemIdx = idx; }

private:
	virtual VOID Free();

private:
	int		itemIdx;
};

