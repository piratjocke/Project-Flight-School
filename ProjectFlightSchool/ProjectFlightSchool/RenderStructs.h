#ifndef RENDERSTRUCTS_H
#define RENDERSTRUCTS_H

#include <DirectXMath.h>

static const UINT MAX_AMOUNT_OF_OBJECT3D	= 5000;
static const UINT MAX_AMOUNT_OF_OBJECT2D	= 5000;
static const UINT MAX_AMOUNT_OF_ANIM3D		= 100;
static const UINT MAX_AMOUNT_OF_PLANES		= 2000;

struct Object3dInfo
{
	UINT				mAssetId;
	DirectX::XMFLOAT4X4	mWorld;
};

struct Anim3dInfo
{
	UINT				mModelId;
	/*UINT				mAnimId;
	float*				mAnimTime;
	DirectX::XMFLOAT3	mPosition;*/
	DirectX::XMFLOAT4X4	mWorld;
	DirectX::XMFLOAT4X4	mBoneTransforms[NUM_SUPPORTED_JOINTS];
};

struct Object2dInfo
{
	UINT				mAssetId;
	DirectX::XMFLOAT2	mTopLeftCorner;
	DirectX::XMFLOAT2	mWidthHeight;

};

struct PlaneInfo
{
	UINT				mAssetId;
	DirectX::XMFLOAT3	mTopTriangle;
	DirectX::XMFLOAT3	mBottomTriangle;
};


struct RenderLists
{
	Object3dInfo*	object3d;
	UINT			sizeOfObject3dList;
	Anim3dInfo&		anim3d;
	UINT			sizeOfAnim3dList;
	Object2dInfo*	object2d;
	UINT			sizeOfObject2dList;
	PlaneInfo*		plane;
	UINT			sizeOfPlaneList;
};

#endif