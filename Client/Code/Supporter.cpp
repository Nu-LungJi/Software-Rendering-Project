#include "../Include/PCH.h"
#include "Supporter.h"

Supporter::Supporter(LPDIRECT3DDEVICE9 _GRPDEV)	: GameObject(_GRPDEV)	{}
Supporter::Supporter(CONST GameObject& _RHS)	: GameObject(_RHS)		{}
Supporter::~Supporter()													{}

HRESULT	Supporter::Ready_GameObject(){ return S_OK; }
INT		Supporter::Update_GameObject(CONST FLOAT& _DT) { return 0; }
VOID	Supporter::LateUpdate_GameObject(CONST FLOAT& _DT){}
VOID	Supporter::Render_GameObject(){}

BOOL	Supporter::OnCollisionEnter(GameObject* _Other)	{return TRUE;}
BOOL	Supporter::OnCollisionStay(GameObject* _Other)	{return TRUE;}
BOOL	Supporter::OnCollisionExit(GameObject* _Other)	{return TRUE;}

HRESULT	Supporter::Component_Initialize()	{ return S_OK; }
HRESULT Supporter::Texture_Initialize()		{ return S_OK; }

Supporter* Supporter::Create(LPDIRECT3DDEVICE9 _GRPDEV) { return nullptr; }

VOID	Supporter::Free(){}