#pragma once
#include "GameObject.h"

static int damageFontCnt = 0;
class DamageFont : public GameObject
{
private:
    explicit DamageFont(LPDIRECT3DDEVICE9 _GRPDEV);
    explicit DamageFont(const GameObject& _RHS);
    virtual ~DamageFont();

public:

    virtual			HRESULT		Ready_GameObject(int Damage, _vec3* vec3Pos);
    virtual			INT			Update_GameObject(CONST FLOAT& _DT);
    virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
    virtual			VOID		Render_GameObject();

public:
    static         DamageFont* Create(LPDIRECT3DDEVICE9 _GRPDEV, int Damage, _vec3* vec3Pos);

    BOOL Get_Active() { return _active; }
    VOID Set_Active(BOOL active) { _active = active; }

    VOID Set_Offset(float x, float y) { offsetX = x; offsetY = y; }
    VOID Set_TargetPos(_vec3 pos) { targetPos = pos; }
    VOID Set_Damage(int dmg) { _Damage = dmg; }
    VOID Set_Size(float size) { _Size = size; }

private:
    virtual  void            Free();
    _vec2                    Matrix_Calc(_vec3 vec3Pos);

private:
    FontObject* _FO;
    int     _Damage;
    _vec3  targetPos;
    _vec2   _Pos;
    float   _Size;
    int     _alpha;

    float offsetX;
    float offsetY;

    _float  _lifeTimer;
    bool    _active;



};

