#include"GameObject.h"

class SkyBox: public GameObject
{
private:
    explicit SkyBox();
    explicit SkyBox(LPDIRECT3DDEVICE9 _GRPDEV);
    explicit SkyBox(const GameObject& ths);
    virtual ~SkyBox();

public:

    virtual			HRESULT		Ready_GameObject();
    virtual			INT			Update_GameObject(CONST FLOAT& _DT);
    virtual			VOID		LateUpdate_GameObject(CONST FLOAT& _DT);
    virtual			VOID		Render_GameObject();

    Transform*  Get_TransCom() { return m_pTransform; }
    Buffer*     Get_Buffer() { return m_pBuffer; }
    static  SkyBox*     Create(LPDIRECT3DDEVICE9 GRPDEV);
private:
    HRESULT			Component_Initialize();

    Buffer*         m_pBuffer;
    Transform*      m_pTransform;
    IDirect3DTexture9* m_pTexture;
private:
    virtual  void            Free();

};

