#include "../Include/PCH.h"
#include "Arrow.h"

Arrow::Arrow(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Arrow::Arrow(const GameObject& _RHS) : GameObject(_RHS) {}
Arrow::~Arrow() {}

HRESULT Arrow::Ready_GameObject(BowType _BOWTYPE, int _LVEL, int arrowAtk, _vec3* _PlayerPOS, _vec2 _arrowDir)
{
    if (FAILED(Component_Initialize())) return E_FAIL;

    _speed = 15.f;
    _sumSpeed = 0.f;
    _lifeTime = 0.f;
    _frame = 1;
    _size = 1.f;
    _frameDelay = 0.f;
    _playerPos = { _PlayerPOS->x, _PlayerPOS->y, _PlayerPOS->z };
    _arrowAtk = arrowAtk;
    _hp = 1;
    _EvilTime = 0.f;
    _evilMoveTime = 0.f;
    _sumEvilSpeed = 0.f;

    _angle = atan2f(-_arrowDir.y, _arrowDir.x);
    _originAngle = _angle;

    {
        switch (_BOWTYPE)
        {
        case BowType::FairyBow :
            if (_LVEL == 3) _type = ArrowType::FairyCharging;
            else _type = ArrowType::FairyArrow;
            _size = 0.8;
            break;
        case BowType::IceBow :
            if(_LVEL == 1) _type = ArrowType::IceArrow_LV1;
            else if(_LVEL == 2) _type = ArrowType::IceArrow_LV2;
            else if(_LVEL == 3) _type = ArrowType::IceCharging;
            break;
        case BowType::EvilHeadBow:
            _type = ArrowType::EvilHead_Arrow;
            _size = 0.6;
            break;
        case BowType::WindBow:
            _type = ArrowType::Wind_Arrow;
            break;
        default:
            _type = ArrowType::FairyArrow;
            break;
        }
    }

    return S_OK;
}

INT Arrow::Update_GameObject(const _float& _DT)
{
    GameObject::Update_GameObject(_DT);
    RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (_hp <= 0){
        TCHAR FileName[128] = L"";
        _vec3 Size = { 1.f, 1.f, 1.f };

        switch (_type) {
        case ArrowType::FairyArrow:
            // PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::FAIRY_HITEFFECT, &effectPos, 0.5f, Size);
            break;
        default:
            break;
        }
        return -1;
    }
        

    _vec3		upDir, rightDir;
    upDir = { 0.f, 0.f, 1.f };
    rightDir = { 1.f, 0.f, 0.f };
    _frameDelay += _DT;
    _evilMoveTime += _DT;
    // 죽을때
    _lifeTime += _DT;
    float maxLifeTime = 1.f;
    switch (_type)
    {
    case ArrowType::FairyCharging:
        maxLifeTime = 3.f;
        break;
    case ArrowType::IceCharging:
        maxLifeTime = 3.f;
        break;
    case ArrowType::EvilHead_Arrow:
        maxLifeTime = 3.f;
        break;
    }
    if (_lifeTime > maxLifeTime)
        return -1;
        
    // 메트릭스
    {
        CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));
        _vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
        D3DXVec3Normalize(&cameraDir, &cameraDir);

        _vec3 eye = { 0.f, 0.f, 0.f };
        _vec3 at = cameraDir;
        _vec3 up = { 0.f, 1.f, 0.f };

        _matrix matSize;
        D3DXMatrixIdentity(&matSize);
        D3DXMatrixScaling(&matSize, _size, _size, _size);

        _matrix matBillboard;
        D3DXMatrixLookAtLH(&matBillboard, &eye, &at, &up);
        D3DXMatrixInverse(&matBillboard, nullptr, &matBillboard);

        // 회전
        if (_type == ArrowType::IceCharging) _originAngle += _DT * D3DXToRadian(1.f) * 600;
        _matrix matRotZ;

        // z축 회전
        D3DXMatrixRotationZ(&matRotZ, _originAngle);
        if(_type == ArrowType::FairyCharging) D3DXMatrixRotationZ(&matRotZ, 0.f);
        _matrix matWorld = matSize * matRotZ * matBillboard;

        // 속도
        switch (_type)
        {
        case ArrowType::FairyCharging:
            _speed = 5.f;
            break;
        case ArrowType::IceCharging:
            _speed = 5.f;
            break;
        case ArrowType::EvilHead_Arrow:
            _speed = 10.f;
            break;
        }
        _sumSpeed += _DT * _speed;
        matWorld._41 = _playerPos.x + _sumSpeed * cosf(_angle);
        matWorld._42 = _playerPos.y;
        matWorld._43 = _playerPos.z - _sumSpeed * sinf(_angle);

        // 이동
        switch (_type)
        {
        case ArrowType::EvilHead_Arrow:
            _evilMoveTime += _DT;

            float wavePower = 1.f;
            float waveSpeed = 5.f;

            float wave = sinf(_evilMoveTime * waveSpeed) * wavePower;

            if (_evilCount % 2 == 1) {
                matWorld._41 += -sinf(_angle) * wave;
                matWorld._43 += -cosf(_angle) * wave;
            }
            else{
                matWorld._41 -= -sinf(_angle) * wave;
                matWorld._43 -= -cosf(_angle) * wave;
            }
            _evilCount++;
            break;
        }

        Component_Transform->Set_World(&matWorld);
        Component_Transform->Set_Pos({ matWorld._41 , matWorld._42 , matWorld._43 });
    }

    return S_OK;
}

VOID Arrow::LateUpdate_GameObject(const _float& _DT)
{
    GameObject::LateUpdate_GameObject(_DT);
}

VOID Arrow::Render_GameObject()
{
    GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    GRPDEV->SetTransform(D3DTS_WORLD, Component_Transform->Get_World());

    SetGrahpic();

    Component_Buffer->Render_Buffer();

    GRPDEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    return VOID();
}

HRESULT Arrow::Component_Initialize()
{
    Component_Buffer = ADD_COMPONENT_RECTTEX;
    Component_Transform = ADD_COMPONENT_TRANSFORM;

    //Component_Collider = ADD_COMPONENT_COLLIDER;					
    //Component_Collider->Set_CenterPos(Component_Transform);			
    //Component_Collider->Set_Scale(0.5f, 0.5f, 0.5f);			

    return S_OK;
}

void Arrow::SetGrahpic()
{
    TCHAR FileName[128] = L"";


    if (_frameDelay > 0.05f) { _frame++; _frameDelay = 0; }
       
    switch (_type) {
    case ArrowType::FairyArrow:
        _frame = 1;
        wsprintfW(FileName, L"FairyArrow.png");
        break;
    case ArrowType::FairyCharging:
        if (_frame > 6) _frame = 1;
        wsprintfW(FileName, L"Fairy_Charging%d.png", _frame);
        break;
    case ArrowType::IceArrow_LV1:
        if (_frame > 6) _frame = 1;
        wsprintfW(FileName, L"IceArrow_%d.png", _frame);
        break;
    case ArrowType::IceCharging:
        wsprintfW(FileName, L"IceChargeShot.png");
        break;
    case ArrowType::EvilHead_Arrow:
        if (_frame > 7) _frame = 1;
        wsprintfW(FileName, L"EvilHead_Arrow%d.png", _frame);
        break;
    case ArrowType::Wind_Arrow:
        if (_frame > 6) _frame = 1;
        wsprintfW(FileName, L"Wind_Arrow%d.png", _frame);
        break;
    default:
        if (_frame > 6) _frame = 1;
        wsprintfW(FileName, L"IceArrow_LV1_%d.png", _frame);
        break;
    }
    
    GRPDEV->SetTexture(0, (ResourceManager::GetInstance()->Find_Texture(FileName)));
}

Arrow* Arrow::Create(LPDIRECT3DDEVICE9 _GRPDEV, BowType _BOWTYPE, int _LVEL, int arrowAtk, _vec3* _PlayerPOS, _vec2 _arrowDir)
{
    Arrow* arrow = new Arrow(_GRPDEV);
    if (FAILED(arrow->Ready_GameObject(_BOWTYPE, _LVEL, arrowAtk, _PlayerPOS, _arrowDir))) {
        MSG_BOX("Cannot Create arrow.");
        Safe_Release(arrow);
        return nullptr;
    }
    return arrow;
}

BOOL Arrow::OnCollisionEnter(GameObject* _Other)
{
    if (_Other->Get_ObjectTag() == L"Bat")
    {
        _hp = 0;
    }

    return 0;
}

VOID Arrow::Free()
{
    GameObject::Free();
}
