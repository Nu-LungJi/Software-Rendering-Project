#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"
namespace Engine
{
	typedef struct tagVertexColor
	{
		_vec3		vPosition;			
		_ulong		dwColor;
	
	}VTXCOL;

	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexTexture
	{
		_vec3		vPosition;
		_vec3		vNormal;
		_ulong		dwColor;
		_vec2		vTexUV;
	}VTXTEX;

	const _ulong	FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE |D3DFVF_TEX1;

	typedef struct tagVertexCubeTexture
	{
		_vec3		vPosition;
		_ulong      dwColor;
		_vec2		vTexUV;
	}VTXCUBE;

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1; // 텍스처의 UV 좌표 값을 FLOAT형 3개로 표현하겠다는 매크로(괄호안의 숫자 0의 의미는 본래 버텍스에 텍스쳐 UV값이 여러개가 올 수 있는데 그중 0번째 값을 지정하겠다는 의미)
	
	typedef struct tagIndex16
	{
		_ushort  _0;
		_ushort  _1;
		_ushort  _2;

	}INDEX16;

	typedef struct tagIndex32
	{
		_ulong	_0;
		_ulong	_1;
		_ulong	_2;

	}INDEX32;	
	
	//파티클용
	typedef struct Attribute
	{
		_vec3		vPosition; // 월드 스페이스 내 파티클 위치
		_vec3		vVelocity;  // 파티클 속도 보통은 초당 이동단위로 기록
		_vec3		vAcceleration; // 파티클의 가속 보통은 초당 단위로 기록
		_float		fLifeTime;   // 파티클이 소멸할때까지 유지되는 시간
		_float		fAge;		 // 파티클의 현재 나이
		_ulong		dwColor;	 // 파티클의 컬러
		_ulong		dwColorFade; // 파티클의 컬러가 시간의 흐름에 따라 퇴색하는 방법
		_bool		bIsAlive;  // 파티클이 생존한 경우 True 소멸한 경우 False

		Attribute()
		{
			ZeroMemory(&vPosition, sizeof(_vec3));
			ZeroMemory(&vVelocity, sizeof(_vec3));
			ZeroMemory(&vAcceleration, sizeof(_vec3));
			fLifeTime = 0;
			fAge = 0;
			dwColor = 0;
			dwColorFade = 0;
			bIsAlive = false;
		}

	}ATTR;

	typedef struct BoundingBox
	{
		_vec3		vMin;
		_vec3		vMax;


		_bool isPointInside(_vec3& p)
		{
			if (p.x >= vMin.x &&
				p.y >= vMin.y &&
				p.z >= vMin.z &&
				p.x <= vMax.x &&
				p.y <= vMax.y &&
				p.z <= vMax.z)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}_Bound;
}


#endif // Engine_Struct_h__
