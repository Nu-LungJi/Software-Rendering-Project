#pragma once
#include "GameObject.h"

class BossEffect : public GameObject {
private:
	explicit BossEffect(LPDIRECT3DDEVICE9 _GRPDEV);
	explicit BossEffect(CONST GameObject& _RHS);
	virtual ~BossEffect();

public:
	virtual	HRESULT		Ready_GameObject() { return S_OK; }
	virtual	INT			Update_GameObject(const _float& _DT);
	virtual	VOID		LateUpdate_GameObject(const _float& _DT);
	virtual	VOID		Render_GameObject();

	virtual BOOL		OnCollisionEnter(GameObject* _Other);
	virtual BOOL		OnCollisionStay(GameObject* _Other);
	virtual BOOL		OnCollisionExit(GameObject* _Other);

public:
	HRESULT				Ready_Effect(BOSS_EFFECT _SKILLTYPE, _vec3* _BossPOS, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _Size, wstring _TAG, _vec3 _Rot);
	HRESULT				Make_TextureList(wstring _FileName);
	VOID				Set_EffectRotation(FLOAT _X, FLOAT _Y, FLOAT _Z);
	static	BossEffect* Create(LPDIRECT3DDEVICE9 _GRPDEV, wstring _TAG, BOSS_EFFECT _SKILLTYPE, _vec3* _BossPOS, BOOL _Repeatable, FLOAT _PlayTime, _vec3 _Size = { 1.f, 1.f, 1.f }, _vec3 _Rot = { 0.f, 0.f, 0.f });
private:
	HRESULT				Component_Initialize();
	virtual	VOID		Free();
private:
	BOSS_EFFECT		SKILL_TYPE;
	INT				ENDFRAME;
	_vec3			EffectSize;
	vector<IDirect3DBaseTexture9*>	TextureList;

	INT				TextureIndex;
	FLOAT			FrameTick;
	FLOAT			PlayTime;
	BOOL			Repeatable;

	Buffer*			Component_Buffer;
	Transform*		Component_Transform;
	Texture*		Component_Texture;
	Collider*		Component_Collider;
};

