#include "MapNode.h"
#include "MapNodeInstance.h"
#include "RenderManager.h"

HRESULT	MapNode::Render( float deltaTime, XMFLOAT4X4 parentWorld )
{
	RenderManager::GetInstance()->AddNodeGridToList( mGrid, mNavVertexCount, mBlendMap, parentWorld );

	for( int i = 0; i < (int)mStaticAssetCount; i++ )
	{
		mStaticAssets[i].Render( deltaTime, parentWorld );
	}
	return S_OK;
}
XMFLOAT3* MapNode::GetNavData() const
{
	return mNavData;
}
UINT MapNode::GetNavVertexCount() const
{
	return mNavVertexCount;
}
StaticVertex* MapNode::GetGrid() const
{
	return mGrid;
}
UINT MapNode::GetVertexCount() const
{
	return mVertexCount;
}
UINT MapNode::GetGridWidth() const
{
	return mGridWidth;
}
UINT MapNode::GetGridHeight() const
{
	return mGridHeight;
}
XMFLOAT3 MapNode::GetOrigin() const
{
	return mOrigin;
}
GameObject* MapNode::GetStaticAssets() const
{
	return mStaticAssets;
}
HRESULT	MapNode::Update( float deltaTime )
{
	return S_OK;
}
MapNodeInstance* MapNode::GetMapNodeInstance()
{
	MapNodeInstance* result = nullptr;
	bool instanceFound = false;
	for( int i = 0; i < (int)INSTANCE_COUNT && !instanceFound; i++ )
	{
		if( mInstances[i].GetInstanceID() == -1 )
		{
			result = &mInstances[i];
			result->SetMapNode( this );
			result->SetInstanceID( i );
			printf( "Instance number: %d\n", i);
			instanceFound = true;
		}
	}
	return result;
}
void MapNode::ReleaseInstance( int InstanceID )
{
	for( int i = 0; i < INSTANCE_COUNT; i++ )
	{
		if( mInstances[i].GetInstanceID() == InstanceID )
		{
			mInstances[i].SetInstanceID( -1 );
		}
	}
}
HRESULT	MapNode::Initialize( MapNodeInfo initInfo )
{
	mGrid				= new StaticVertex[initInfo.vertexCount];
	//BUGHAXX
	mName				= initInfo.name;
	mNavData			= initInfo.navData;
	mNavVertexCount		= initInfo.navVertexCount;
	mGridWidth			= initInfo.gridWidth;
	mGridHeight			= initInfo.gridHeight;
	//Handles deleting assets
	mStaticAssets		= initInfo.staticAssets;
	mStaticAssetCount	= initInfo.staticAssetCount;
	mVertexCount		= initInfo.vertexCount;
	mBlendMap			= initInfo.blendMap;
	
	DirectX::XMVECTOR scale;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR translation;

	DirectX::XMMatrixDecompose( &scale, &rotation, &translation, XMLoadFloat4x4( &initInfo.anchor ) );

	XMStoreFloat3( &mOrigin, translation );

	mOrigin.z = -mOrigin.z;

	for( int i = 0; i < (int)mNavVertexCount; i++ )
	{
		//mGrid[i].position[0]	= initInfo.grid[i].position[0];
		//mGrid[i].position[1]	= initInfo.grid[i].position[1];
		//mGrid[i].position[2]	= initInfo.grid[i].position[2];

		mGrid[i].position[0]	= mNavData[i].x;
		mGrid[i].position[1]	= mNavData[i].y;
		mGrid[i].position[2]	= mNavData[i].z;

		//mGrid[i].normal[0]	= initInfo.grid[i].normal[0];
		//mGrid[i].normal[1]	= initInfo.grid[i].normal[1];
		//mGrid[i].normal[2]	= initInfo.grid[i].normal[2];

		mGrid[i].normal[0]	= 1.0f;
		mGrid[i].normal[1]	= 1.0f;
		mGrid[i].normal[2]	= 1.0f;

		mGrid[i].tangent[0]	= 1.0f;
		mGrid[i].tangent[1]	= 1.0f;
		mGrid[i].tangent[2]	= 1.0f;

		mGrid[i].uv[0] = mGrid[i].position[0] / (float)mGridWidth + 0.5f;
		mGrid[i].uv[1] = -mGrid[i].position[2] / (float)mGridHeight + 0.5f;
	}
	return S_OK;
}
void MapNode::Release()
{
	if( mNavData )
		delete[] mNavData;
	if( mGrid )
		delete[] mGrid;
	if( mStaticAssets )
		delete[] mStaticAssets;
	for( int i = 0; i < INSTANCE_COUNT; i++ )
	{
		mInstances[i].Release();
	}
}
MapNode::MapNode()
{
	mGrid			= nullptr;
	mVertexCount	= 0;
	mGridWidth		= 0;
	mGridHeight		= 0;
	mOrigin			= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mStaticAssets	= nullptr;
}
MapNode::~MapNode()
{
}
