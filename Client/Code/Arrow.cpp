#include "../Include/PCH.h"
#include "Arrow.h"

Arrow::Arrow(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Arrow::Arrow(const GameObject& _RHS) : GameObject(_RHS) {}
Arrow::~Arrow() {}

HRESULT Arrow::Ready_GameObject(BowType _BOWTYPE, int _LVEL, int arrowAtk, _vec3* _PlayerPOS, _vec2 _arrowDir)
{
    if (FAILED(Component_Initialize())) return E_FAIL;

    _bowType = _BOWTYPE;
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
    _effectDelay = 0.f;
    _ThunderDelay = 0.5f;

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
            if (_LVEL == 1) {
                _size = 0.6;
                _type = ArrowType::EvilHead_Arrow;
            }
            else if (_LVEL == 3) _type = ArrowType::EvilHeadCharging;
            break;
        case BowType::WindBow:
            _type = ArrowType::Wind_Arrow;
            break;
        default:
            _type = ArrowType::FairyArrow;
            break;
        }
    }

    if (_type == ArrowType::IceCharging) _size = 1.5f;

    Player* player = dynamic_cast<Player*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Player"));

    _playerRange = player->Get_Range();
    _playerArrowSize = player->Get_ArrowSize();
    _playerArrowSpeed = player->Get_ArrowSpeed();

    return S_OK;
}

INT Arrow::Update_GameObject(const _float& _DT)
{
    GameObject::Update_GameObject(_DT);

    if (_hp <= 0){
        _vec3 Size = { 2.f, 2.f, 2.f };
        _vec3 effectPos = *Component_Transform->Get_Position();
        Size *= (*_playerArrowSize);
        switch (_type) {
        case ArrowType::FairyArrow:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ARROW_HITEFFECT, &effectPos, 0.5f, Size, false);
            break;
        case ArrowType::FairyCharging:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::FAIRY_HITEFFECT, &effectPos, 0.5f, Size, false);
            break;
        case ArrowType::IceCharging:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ICE_HITEFFECT, &effectPos, 0.5f, Size, false);
            break;
        case ArrowType::EvilHead_Arrow:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_HITEFFECT, &effectPos, 0.5f, Size, false);
            break;
        case ArrowType::EvilHeadCharging:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_HITEFFECT, &effectPos, 0.5f, Size, false);
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
    _effectDelay += _DT;

    // ¾ÆÀÌ½º Â÷Â¡ ¿¡·Î¿ì ÀÌÆåÆ®
    if (_type == ArrowType::IceCharging) {
        if (_effectDelay > 0.1f) {
            _vec3 Size = { 1.f, 1.f, 1.f };
            _vec3 effectPos = *Component_Transform->Get_Position();
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ICE_THORN, &effectPos, 0.4f, Size, false);
            _effectDelay = 0.f;
        }
    }
    // ¿¡ºô Â÷Â¡ ¿¡·Î¿ì ÀÌÆåÆ®
    if (_type == ArrowType::EvilHeadCharging) {
        _ThunderDelay += _DT;

        if (_effectDelay > 0.5f) {
            _vec3 Size = { 1.5f, 1.5f, 1.5f };
            Size *= (*_playerArrowSize);
            _vec3 effectPos = *Component_Transform->Get_Position();
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_WAVE_EFFECT, &effectPos, 0.5f, Size, false);
            _effectDelay = 0.f;
        }
        if (_ThunderDelay > 1.5f) {
            _vec3 Size = { 3.f, 6.f, 3.f };
            Size *= (*_playerArrowSize);
            _vec3 effectPos = *Component_Transform->Get_Position();
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_THUNDER, &effectPos, 1.f, Size, false);
            _ThunderDelay = 0.f;
        }
    }

    // Á×À»¶§
    _lifeTime += _DT;
    float maxLifeTime = 1.f;
    switch (_type)
    {
    case ArrowType::FairyCharging:
        maxLifeTime = 2.f;
        break;
    case ArrowType::IceCharging:
        maxLifeTime = 1.f;
        break;
    case ArrowType::EvilHead_Arrow:
        maxLifeTime = 1.f;
        break;
    case ArrowType::EvilHeadCharging:
        maxLifeTime = 3.f;
        break;
    }
    maxLifeTime = maxLifeTime * (*_playerRange) / (*_playerArrowSpeed);
    if (_lifeTime > maxLifeTime) {
        _vec3 Size = { 2.f, 2.f, 2.f };
        Size *= (*_playerArrowSize);
        _vec3 effectPos = *Component_Transform->Get_Position();

        switch (_type) {
        case ArrowType::FairyCharging:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::FAIRY_HITEFFECT, &effectPos, 0.5f, Size, false);
            break;
        case ArrowType::IceCharging:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ICE_HITEFFECT, &effectPos, 0.5f, Size, false);
            break;
        case ArrowType::EvilHead_Arrow:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_HITEFFECT, &effectPos, 0.5f, Size, false);
            break;
        case ArrowType::EvilHeadCharging:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_HITEFFECT, &effectPos, 0.5f, Size, false);
            break;
        default:
            break;
        }

        return -1;
    }
        
        
    // ¸ÞÆ®¸¯½º
    {
        CameraObject* Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->Get_GameObject(L"Camera"));
        _vec3 cameraDir = *(Camera->Get_EyeVec()) - *(Camera->Get_AtVec());
        D3DXVec3Normalize(&cameraDir, &cameraDir);

        _vec3 eye = { 0.f, 0.f, 0.f };
        _vec3 at = cameraDir;
        _vec3 up = { 0.f, 1.f, 0.f };

        switch (_bowType)
        {
        case BowType::FairyBow:
            _size = 0.8f;
            break;
        case BowType::EvilHeadBow:
            _size = 0.6f;
            break;
        default:
            _size = 1.f;
            break;
        }
        _size *= (*_playerArrowSize);
        Component_Collider->Set_Scale(_size * 0.5f, _size * 0.5f, _size * 0.5f);
        _matrix matSize;
        D3DXMatrixIdentity(&matSize);
        D3DXMatrixScaling(&matSize, _size, _size, _size);

        _matrix matBillboard;
        D3DXMatrixLookAtLH(&matBillboard, &eye, &at, &up);
        D3DXMatrixInverse(&matBillboard, nullptr, &matBillboard);

        // È¸Àü
        //if (_type == ArrowType::IceCharging) _originAngle += _DT * D3DXToRadian(1.f) * 400;
        _matrix matRotZ;

        // zÃà È¸Àü
        D3DXMatrixRotationZ(&matRotZ, _originAngle);
        if(_type == ArrowType::FairyCharging) D3DXMatrixRotationZ(&matRotZ, 0.f);
        _matrix matWorld = matSize * matRotZ * matBillboard;

        // ¼Óµµ
        switch (_type)
        {
        case ArrowType::FairyCharging:
            _speed = 5.f;
            break;
        case ArrowType::IceCharging:
            _speed = 15.f;
            break;
        case ArrowType::EvilHead_Arrow:
            _speed = 10.f;
            break;
        case ArrowType::EvilHeadCharging:
            _speed = 5.f - _evilMoveTime * 1.f;
            break;
        }
        _sumSpeed += _DT * _speed * (*_playerArrowSpeed);
        matWorld._41 = _playerPos.x + _sumSpeed * cosf(_angle);
        matWorld._42 = _playerPos.y;
        matWorld._43 = _playerPos.z - _sumSpeed * sinf(_angle);

        // ÀÌµ¿
        switch (_type)
        {
        case ArrowType::EvilHead_Arrow:
            _evilMoveTime += _DT;

            float wavePower = 1.f;
            float waveSpeed = 5.f * (*_playerArrowSpeed);

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
    RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return S_OK;
}

VOID Arrow::LateUpdate_GameObject(const _float& _DT)
{
    GameObject::LateUpdate_GameObject(_DT);

    Destory_Tile();
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

    Component_Collider = ADD_COMPONENT_COLLIDER;					
    Component_Collider->Set_CenterPos(Component_Transform);			
    Component_Collider->Set_Scale(0.5f, 0.5f, 0.5f);			

    return S_OK;
}

void Arrow::SetGrahpic()
{
    TCHAR FileName[128] = L"";

    // ÀÌÆåÆ® ¼Óµµ
    float effectSpeed = 0.05f;
    switch (_type) {
    case ArrowType::FairyCharging:
        effectSpeed = 0.2f;
        break;
    case ArrowType::IceCharging:
        effectSpeed = 0.05f;
        break;
    }
    if (_frameDelay > effectSpeed) { _frame++; _frameDelay = 0; }
       
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
        if (_frame > 4) _frame = 1;
        wsprintfW(FileName, L"IceChargeShot%d.png", _frame);
        break;
    case ArrowType::EvilHead_Arrow:
        if (_frame > 7) _frame = 1;
        wsprintfW(FileName, L"EvilHead_Arrow%d.png", _frame);
        break;
    case ArrowType::EvilHeadCharging:
        if (_frame > 8) _frame = 1;
        wsprintfW(FileName, L"EvilChargeShot%d.png", _frame);
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

void Arrow::Destory_Tile()
{
    _vec3 vPos, vScale, vTileScale, vTilePos{ 0,0,0 };
   
    Transform* pTile = nullptr;
    vPos = *Component_Transform->Get_Position();
    vScale = *Component_Transform->Get_Scale();
    for (auto& iter : TileManager::GetInstance()->Get_DestoryTile_List())
    {
        if (dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_TileStateName() == TILE_STATE::STATE_DESTORY||
            dynamic_cast<TileInfo*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TILEINFO))->Get_TileStateName() == TILE_STATE::STATE_BOOM)
        {
            pTile = dynamic_cast<Transform*>(iter->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
            vTilePos   =  *pTile->Get_Position();
          
            if (vPos.x > vTilePos.x - 1 && vPos.x < vTilePos.x + 1 && vPos.z > vTilePos.z -1 && vPos.z < vTilePos.z + 1 )
            {
                Set_ObjectDead(TRUE);
                dynamic_cast<CXZTile*>(iter)->Set_Destory();
            }
        }
    }
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


    return 0;
}

BOOL Arrow::OnCollisionStay(GameObject* _Other)
{
    if (_Other->Get_ObjectTag() == L"Monster")
    {
        _hp = 0;
    }
    return 0;
}

VOID Arrow::Free()
{
    GameObject::Free();
}
