#include "../Include/PCH.h"
#include "Arrow.h"

Arrow::Arrow(LPDIRECT3DDEVICE9 _GRPDEV) : GameObject(_GRPDEV) {}
Arrow::Arrow(const GameObject& _RHS) : GameObject(_RHS) {}
Arrow::~Arrow() {}

HRESULT Arrow::Ready_GameObject(BowType _BOWTYPE, int _LVEL, int arrowAtk, _vec3* _PlayerPOS, _vec2 _arrowDir)
{
    if (FAILED(Component_Initialize())) return E_FAIL;

    Component_Transform->Set_Pos(*_PlayerPOS);
  
    _bowType = _BOWTYPE;
    _speed = 30.f;
    _sumSpeed = 0.f;
    _lifeTime = 0.f;
    _frame = 1;
    _size = 1.f;
    _frameDelay = 0.f;
    _playerPos = { _PlayerPOS->x, _PlayerPOS->y, _PlayerPOS->z };
    _arrowAtk = arrowAtk;
    Component_Collider->Set_Att(100);
    _hp = 1;
    Component_Collider->Set_Hp(1.f);
    _EvilTime = 0.f;
    _evilMoveTime = 0.f;
    _sumEvilSpeed = 0.f;
    _effectDelay = 0.f;
    _ThunderDelay = 0.5f;
    _targetPos = nullptr;
    _target = nullptr;
    _calcSpeed = 0.f;
    _searchDelay = 0.f;
    turnSpeed == D3DXToRadian(2.5f);
    _isReady = false;

    _angle = atan2f(-_arrowDir.y, _arrowDir.x);
    _originAngle = _angle;

    std::random_device rd;
    std::uniform_int_distribution<int> distribution(0, 180);

    {
        switch (_BOWTYPE)
        {
        case BowType::FairyBow :
            if (_LVEL == 3) _type = ArrowType::FairyCharging;
            else _type = ArrowType::FairyArrow;
            _size = 0.8;
            break;
        case BowType::IceBow :
            if (_LVEL == 1) {
                _type = ArrowType::IceArrow_LV1;
                _iceArrowCount++;
                _myArrowCount = _iceArrowCount;
                _iceArrow_Timer = 0.f;
            } 
            else if(_LVEL == 2) _type = ArrowType::IceArrow_LV2;
            else if(_LVEL == 3) _type = ArrowType::IceCharging;
            break;
        case BowType::EvilHeadBow:
            if (_LVEL == 1) {
                _type = ArrowType::EvilHead_Arrow;
            }
            else if (_LVEL == 3) _type = ArrowType::EvilHeadCharging;
            break;
        case BowType::WindBow:
            if (_LVEL == 1) _type = ArrowType::Wind_Arrow;
            else if(_LVEL == 3) _type = ArrowType::WindCharging;
            break;
        case BowType::AtomicBow:
            _type = ArrowType::Atomic_Arrow;
            _speed = 0.f;
            _AtomicAccel = 40.f;
            _atomicRange = distribution(rd) % 15 + 5.f;
            _arrowLength = 0.f;
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

    if (_type == ArrowType::Atomic_Arrow) {
        *_playerRange = 1.f;
        *_playerArrowSize = 1.f;
        *_playerArrowSpeed = 1.f;
    }

    CollisionManager::GetInstance()->Add_ColliderObject(this);

    return S_OK;
}

INT Arrow::Update_GameObject(const _float& _DT)
{
    GameObject::Update_GameObject(_DT);

    CameraObject* Camera = nullptr;

    _iceArrow_Timer += _DT;

    if (_myArrowCount % 3 + 1 == 3) {
        if (_iceArrow_Timer < 0.1f) return S_OK;
    }
    else if (_myArrowCount % 3 + 1 == 2) {
        if (_iceArrow_Timer < 0.2f) return S_OK;
    }

    _vec3		upDir, rightDir;
    upDir = { 0.f, 0.f, 1.f };
    rightDir = { 1.f, 0.f, 0.f };

    _frameDelay += _DT;
    _evilMoveTime += _DT;
    _effectDelay += _DT;

    // LifeTime
    _lifeTime += _DT;
    float maxLifeTime = 1.f;
    switch (_type)
    {
    case ArrowType::FairyCharging:
        maxLifeTime = 2.f;
        break;
    case ArrowType::IceArrow_LV1:
        maxLifeTime = 0.4f;
        break;
    case ArrowType::IceCharging:
        maxLifeTime = 0.6f;
        break;
    case ArrowType::EvilHead_Arrow:
        maxLifeTime = 1.f;
        break;
    case ArrowType::EvilHeadCharging:
        maxLifeTime = 5.f;
        break;
    case ArrowType::Wind_Arrow:
        maxLifeTime = 8.f;
        break;
    case ArrowType::WindCharging:
        maxLifeTime = 0.2f;
        break;
    case ArrowType::Atomic_Arrow:
        maxLifeTime = 20.f;
        break;
    }
    maxLifeTime = maxLifeTime * (*_playerRange) / (*_playerArrowSpeed);
    if (_lifeTime > maxLifeTime || Component_Collider->Get_Hp() <= 0) {
        _vec3 Size = { 2.f, 2.f, 2.f };
        Size *= (*_playerArrowSize);
        _vec3 effectPos = *Component_Transform->Get_Position();

        switch (_type) {
        case ArrowType::FairyCharging:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::FAIRY_HITEFFECT, &effectPos, 0.5f, Size, false);
            effectPos.z += 2.5f;
            Size = { 5.f, 5.f, 5.f };
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::WIND_SPIRIT, &effectPos, 0.5f, Size, false);
            SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Wind_Bow/Weapon_52_Storm.wav", CHANNELID::SOUND_EFFECT05, 0.7f);
            break;
        case ArrowType::IceArrow_LV1:
            Size = { 2.5f, 2.5f, 2.5f };
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ICE_BOMB, &effectPos, 0.2f, Size, false);
            break;
        case ArrowType::IceCharging:
            Size = { 2.5f, 2.5f, 2.5f };
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ICE_HITEFFECT, &effectPos, 0.3f, Size, false);
            break;
        case ArrowType::EvilHead_Arrow:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_HITEFFECT, &effectPos, 0.5f, Size, false);
            break;
        case ArrowType::EvilHeadCharging:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_HITEFFECT, &effectPos, 0.5f, Size, false);
            SoundManager::GetInstance()->Play_Sound_Once(L"Bow/EvilHead_Bow/Weapon_67_Lightning_Fire.wav", CHANNELID::SOUND_EFFECT05, 0.7f);
            break;
        case ArrowType::Wind_Arrow:
            Size = { 1.5f, 1.5f, 1.5f };
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::WIND_HITEFFECT, &effectPos, 0.5f, Size, false);
            // TODO:이펙트는 정상적으로 잘 나오나, 소리가 이상하게 늦게 ㅇ나옴. 이펙트가 정상적으로 끝나는지 확인해야 할 듯.
            SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Wind_Bow/Weapon_67_WindSword_ChargedFire.wav", CHANNELID::SOUND_EFFECT05, 0.7f);
            break;
        case ArrowType::WindCharging:
            Size = { 7.f, 7.f, 7.f };
            effectPos.y += 2.f;
            effectPos.z += 3.f;
            //PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::PAREND, &effectPos, 0.8f, Size, false);
            {
                PlayerEffect* effect = nullptr;

               effect = PlayerEffect::Create(GRPDEV, PLAYER_SKILL::PAREND, &effectPos, false, 0.8f, Size, false);

                TCHAR arrowTag[128] = L"";
                wsprintfW(arrowTag, L"PlayerArrow_%d", _arrowCount++);

                effect->Set_ObjectTag(arrowTag);
                effect->Set_ObjectType(GAMEOBJECT_TYPE::OBJECT_PLAYER);
                effect->Set_ObjectTag(L"PlayerArrow");

                SceneManager::GetInstance()->Get_CurrentScene()->Get_Layer(LAYER_TYPE::LAYER_DYNAMIC_OBJECT)->Add_GameObject(effect);
            }
            Camera = dynamic_cast<CameraObject*>(SceneManager::GetInstance()->Get_CurrentScene()->
                Get_GameObject(L"Camera"));
            Camera->Camera_Shaking(30.f, 1.f);
            break;
        default:
            break;
        }
        CollisionManager::GetInstance()->Delete_ColliderObject(this);

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

        // Size
        switch (_type)
        {
        case ArrowType::FairyArrow:
            _size = 1.f;
            break;
        case ArrowType::EvilHead_Arrow:
            _size = 1.f;
            break;
        case ArrowType::Wind_Arrow:
            _size = 1.5f;
            break;
        case ArrowType::WindCharging:
            _size = 2.f;
            break;
        default:
            _size = 1.2f;
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

        // windArrow Angle
        _matrix matWorld;
        if (_type == ArrowType::Wind_Arrow) {
            _searchDelay += _DT;

            turnSpeed = D3DXToRadian(3.f);
            Search_Target_Object(30.f);

            float TargetAngle = 0.f;
            if (_target != nullptr && _targetPos != nullptr) {
                _vec3 dir = *_targetPos - *Component_Transform->Get_Position();
                D3DXVec3Normalize(&dir, &dir);
                TargetAngle = atan2f(-dir.z, dir.x);

                float delta = TargetAngle - _angle;

                while (delta > D3DX_PI)  delta -= D3DX_PI * 2.f;
                while (delta < -D3DX_PI) delta += D3DX_PI * 2.f;


                if (fabs(delta) < turnSpeed)
                {
                    _angle = TargetAngle;
                }
                else
                {
                    _angle += (delta > 0.f ? turnSpeed : -turnSpeed);
                }
            }
            
            _matrix matRotZ;
            D3DXMatrixRotationZ(&matRotZ, _angle);
            matWorld = matSize * matRotZ * matBillboard;
        }
        else {
            _matrix matRotZ;
            D3DXMatrixRotationZ(&matRotZ, _originAngle);
            if (_type == ArrowType::FairyCharging) D3DXMatrixRotationZ(&matRotZ, 0.f);
            matWorld = matSize * matRotZ * matBillboard;
        }

        // Speed
        switch (_type)
        {
        case ArrowType::FairyCharging:
            _speed = 10.f;
            break;
        case ArrowType::IceArrow_LV1:
            _speed = 60.f;
            break;
        case ArrowType::IceCharging:
            _speed = 40.f;
            break;
        case ArrowType::EvilHead_Arrow:
            _speed = 20.f;
            break;
        case ArrowType::EvilHeadCharging:
            _speed = 5.f - _evilMoveTime * 0.7f;
            break;
        case ArrowType::Wind_Arrow:
            _speed = 40.f;
            break;
        case ArrowType::WindCharging:
            _speed = 80.f;
            break;
        case ArrowType::Atomic_Arrow:
            _vec2 tempLength = { _playerPos.x - (*Component_Transform->Get_Position()).x , _playerPos.z - (*Component_Transform->Get_Position()).z };
            _arrowLength = D3DXVec2Length(&tempLength);
            if (_arrowLength <= _atomicRange * 0.5) _speed += _AtomicAccel * _DT * 2.f;
            else _speed -= _AtomicAccel * _DT * 2.f;
            if(_arrowLength >= _atomicRange || _speed < 0.f) _speed = 0.f;
            break;
        }

        // 움직임
        _vec3* curPos = Component_Transform->Get_Position();
        if (_type == ArrowType::EvilHead_Arrow) {
            _sumSpeed += _DT * _speed * (*_playerArrowSpeed);
            matWorld._41 = _playerPos.x + _sumSpeed * cosf(_angle);
            matWorld._42 = 0.5f;
            matWorld._43 = _playerPos.z - _sumSpeed * sinf(_angle);
        }
        else {
            _calcSpeed = _DT * _speed * (*_playerArrowSpeed);
            matWorld._41 = (*curPos).x + _calcSpeed * cosf(_angle);
            matWorld._42 = 0.5f;
            matWorld._43 = (*curPos).z - _calcSpeed * sinf(_angle);
        }

        if (_type == ArrowType::FairyCharging) {
            _targetPos = nullptr;
            Search_Target();
            if (_targetPos != nullptr) {
                _vec3 dir = *_targetPos - *Component_Transform->Get_Position();
                D3DXVec3Normalize(&dir, &dir);
                _angle = atan2f(-dir.z, dir.x);

                matWorld._41 = (*curPos).x + _calcSpeed * cosf(_angle);
                matWorld._42 = 0.5f;
                matWorld._43 = (*curPos).z - _calcSpeed * sinf(_angle);
            }
        }

        // ÀÌµ¿
        switch (_type)
        {
        case ArrowType::EvilHead_Arrow:
            _evilMoveTime += _DT;

            float wavePower = 1.5f;
            float waveSpeed = 5.f * (*_playerArrowSpeed);

            float wave = sinf(_evilMoveTime * waveSpeed) * wavePower;

            if (_evilCount % 2 == 1) {
                matWorld._41 += -sinf(_angle) * wave;
                matWorld._43 += -cosf(_angle) * wave;
            }
            else {
                matWorld._41 -= -sinf(_angle) * wave;
                matWorld._43 -= -cosf(_angle) * wave;
            }
            _evilCount++;
            break;
        }
        if (_type == ArrowType::WindCharging) matWorld._42 = 2.f;
        if(_type == ArrowType::Atomic_Arrow) matWorld._42 = 0.7f;

        Component_Transform->Set_World(&matWorld);
        Component_Transform->Set_Pos({ matWorld._41 , matWorld._42 , matWorld._43 });

    }

    // 날라갈때 이펙트
    {
        _vec3 Size = { 0.1f, 0.1f, 0.1f };
        _vec3 effectPos = *Component_Transform->Get_Position();
        switch (_type) {
        case ArrowType::FairyArrow:
            PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::GREEN_SHADER, &effectPos, 0.1f, Size, false);
            break;
        case ArrowType::IceArrow_LV1:
            if (_effectDelay > 0.1f) {
                PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ICE_SHADER, &effectPos, 0.1f, Size, false);
                _effectDelay = 0.f;
            }
            break;
        case ArrowType::IceCharging:
            if (_effectDelay > 0.1f) {
                Size = { 1.f, 1.f, 1.f };
                PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ICE_THORN, &effectPos, 0.4f, Size, false);
                PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ICE_SHADER, &effectPos, 0.8f, Size, false);
                SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Ice_Bow/Weapon_14_2_IceThorns.wav", CHANNELID::SOUND_EFFECT05, 0.35f);
                _effectDelay = 0.f;
            }
            break;
        case ArrowType::EvilHeadCharging:
            _ThunderDelay += _DT;
            if (_effectDelay > 0.5f) {
                _vec3 Size = { 1.5f, 1.5f, 1.5f };
                Size *= (*_playerArrowSize);
                PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_WAVE_EFFECT, &effectPos, 0.5f, Size, false);
                _effectDelay = 0.f;
            }
            if (_ThunderDelay > 1.f) {
                Search_Target_Object(20.f);
                _vec3 Size = { 5.f, 10.f, 5.f };
                Size *= (*_playerArrowSize);
                _targetPos = nullptr;
                if (_targetPos != nullptr) {
                    effectPos = *_targetPos;
                }
                effectPos.y += 2.f;
                effectPos.z += 5.f;
                PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::EVIL_THUNDER, &effectPos, 0.8f, Size, false);
                SoundManager::GetInstance()->Play_Sound_Once(L"Bow/EvilHead_Bow/Hit_Lightning_Strike.wav", CHANNELID::SOUND_EFFECT05, 0.7f);
                _ThunderDelay = 0.f;
                
                if (_target != nullptr) {
                    int targetHp = static_cast<Collider*>(_target->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER))->Get_Hp();
                    static_cast<Collider*>(_target->Get_Component(COMPONENT_TYPE::COMPONENT_COLLIDER))->Set_Hp(targetHp - Component_Collider->Get_Att() * 3.f);
                }
            }
            break;
        case ArrowType::Wind_Arrow:
            Size = { 0.3f, 0.3f, 0.3f };
            break;
        }
    }
    
    if (_arrowLength > _atomicRange * 0.5 && _speed == 0.f && !_isReady) {
        _vec3 Size = { 4.f, 4.f, 4.f };
        PLAY_PLAYER_EFFECT_ONCE(PLAYER_SKILL::ATOMIC_READY, Component_Transform->Get_Position() , 0.5f, Size, false);
        SoundManager::GetInstance()->Play_Sound_Once(L"Bow/Wind_Bow/Weapon_67_WindSword_ChargedFire.wav", CHANNELID::SOUND_EFFECT05, 0.7f);
        ObjectDead = true;
    }
        
    
    RenderManager::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return 0;
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
        wsprintfW(FileName, L"IceArrow_Charging%d.png", _frame);
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
        wsprintfW(FileName, L"IRA_Arrow%d.png", _frame);
        break;
    case ArrowType::WindCharging:
        wsprintfW(FileName, L"IRA_Arrow_Standard.png");
        break;
    case ArrowType::Atomic_Arrow:
        if (_frame > 9) _frame = 1;

        if (_arrowLength > _atomicRange * 0.5f && _speed < 23.f) {
            _frame -= 2;
            if (_frame < 1) _frame = 1;
        }
        else if(_frame > 5) _frame = 5;

        wsprintfW(FileName, L"Atomic_Arrow%d.png", _frame);
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
                _hp = 0;
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
    if (_type == ArrowType::Atomic_Arrow) return FALSE;

    wstring Tag = _Other->Get_ObjectTag();
    int hp = Component_Collider->Get_Hp();
    int atk = COLLIDER(_Other)->Get_Att();

    if (Tag == L"Monster") {
        atk = 1.f;
        if (_type == ArrowType::EvilHeadCharging) return TRUE;
        Component_Collider->Set_Hp(hp - atk);

        DamageFontManager::GetInstance()->Add_DamageFont(_Other, (int)Component_Collider->Get_Att());

        return TRUE;
    }

    else if (Tag == L"CheonLog") {
        atk = 1.f;
        Component_Collider->Set_Hp(hp - atk);
        //COLLIDER(_Other)->Set_Hp(COLLIDER(_Other)->Get_Hp() - Component_Collider->Get_Att());
        COLLIDER(_Other)->Set_Hp(COLLIDER(_Other)->Get_Hp() - 1.f);
        if (_type == ArrowType::EvilHeadCharging) return TRUE;
       DamageFontManager::GetInstance()->Add_DamageFont(_Other, Component_Collider->Get_Att());

        return TRUE;
    }
    else if (_Other->Get_ObjectTag() == L"Docheol") {
        atk = 1.f;
        COLLIDER(_Other)->Set_Hp(COLLIDER(_Other)->Get_Hp() - COLLIDER(_Other)->Get_Att());
        if (_type == ArrowType::EvilHeadCharging) return TRUE;
        Component_Collider->Set_Hp(hp - atk);
        DamageFontManager::GetInstance()->Add_DamageFont(_Other, Component_Collider->Get_Att());

        return TRUE;
    }

    return FALSE;
}

BOOL Arrow::OnCollisionStay(GameObject* _Other)
{
    return FALSE;
}

VOID Arrow::Free()
{
    GameObject::Free();
}

void Arrow::Search_Target(float length)
{
    _targetPos = SceneManager::GetInstance()->Get_CurrentScene()->Search_Target(Component_Transform->Get_Position(), length, L"Monster");
}

void Arrow::Search_Target_Object(float length)
{
    _target = SceneManager::GetInstance()->Get_CurrentScene()->Search_Target_Object(Component_Transform->Get_Position(), length, L"Monster");
    if(_target != nullptr)
        _targetPos = static_cast<Transform*>(_target->Get_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_Position();
}
