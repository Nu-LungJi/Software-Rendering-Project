#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include "Sprite.h"

BEGIN(Engine)

struct ENGINE_DLL FontObject {
	_vec2		Position;
	wstring		Text;
	_int		TextScale;
	_int		TextWeight;
	wstring		FontTag;
	wstring		FontType;
	BOOL		Visible;
	D3DCOLOR	TextColor;
	ID3DXFont*	DXFont;
	BOOL		Active;

	DWORD		FORMAT;

	FontObject(_vec2 _Position, wstring _Text, _int _TextScale, _int _TextWeight, wstring _FontTag, wstring _FontType, D3DCOLOR _Color, BOOL _Visible, DWORD _FORMAT)
		: Position(_Position), Text(_Text), TextScale(_TextScale), TextWeight(_TextWeight), FontTag(_FontTag),
		FontType(_FontType), Visible(_Visible), TextColor(_Color), DXFont(nullptr), FORMAT(_FORMAT), Active(TRUE){
	}

	VOID	Set_Text(wstring _TXT) { Text = _TXT; }
	wstring	Get_Text() { return Text; }

	VOID	Set_Pos(_float _PosX, _float _PosY) { Position.x = _PosX; Position.y = _PosY; }
	_vec2	Get_Pos() { return Position; }

	VOID	Set_Color(INT _A, INT _R, INT _G, INT _B) { TextColor = D3DCOLOR_ARGB(_A, _R, _G, _B); }
	FLOAT	Set_Color() { return TextColor; }

	VOID	Set_Visible(BOOL _Vis) { Visible = _Vis; }
	BOOL	Set_Visible() { return Visible; }

	VOID	Set_Active(BOOL isActive) { Active = isActive; }
	BOOL	Get_Active() { return Active; }
};

class ENGINE_DLL UIManager : public Base {
	DECLARE_SINGLETON(UIManager)
private:
	explicit UIManager();
	virtual ~UIManager();

public:
	HRESULT		Ready_UIManager(LPDIRECT3DDEVICE9 _GRPDEV);
	INT			Update_UIManager(CONST FLOAT& _DT);
	VOID		LateUpdate_UIManager(CONST FLOAT& _DT);
	VOID		Render_UIManager(LPDIRECT3DDEVICE9 _GRPDEV);

public:
	FontObject* Add_FontSprite(LPDIRECT3DDEVICE9 _GRPDEV, wstring _Text, _vec2 _Position, _int _TextScale, wstring _FontTag,
		wstring _FontType, D3DCOLOR _Color = D3DCOLOR_ARGB(255, 255, 255, 255), _int TextWeight = 100, BOOL _Visible = TRUE, DWORD FORMAT = DT_CENTER);

	FontObject* Find_FontObject(wstring _Text);

	VOID      Delete_FontObject(FontObject* obj);


	VOID		Render_FontObjects();

	VOID		Update_DamageFont(LPDIRECT3DDEVICE9 _GRPDEV, FontObject* FO);

public:
	BOOL		Get_Active()				{ return isActive;		}
	VOID		Set_Active(BOOL _isActive)	{ isActive = _isActive; }

	ItemINFO*	Find_Item(wstring _TAG);
private:

	ID3DXSprite* DXSprite;

	map<CONST TCHAR*, ItemINFO*>	ItemList;

	map<CONST TCHAR*, FontObject*>	FontList;

	BOOL		isActive;

private:
	virtual VOID Free();
};

END