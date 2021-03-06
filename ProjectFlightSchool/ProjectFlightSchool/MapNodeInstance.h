#ifndef MAPNODEINSTANCE_H
#define MAPNODEINSTANCE_H
#include <DirectXMath.h>
#include <Windows.h>
#include <vector>
#include "BoundingGeometry.h"
#include "Navmesh.h"
#include "Pathfinder.h"
#include "RenderManager.h"

//forward declaration

class MapNode;
class MapNodeInstance
{
public:

	private:
		int mInstanceID;
		int mNodeID;
		int mSizeX;
		int mSizeY;

		MapNode* mNode;

		DirectX::XMFLOAT3 mPos;
		DirectX::XMFLOAT3 mOrigin;
		DirectX::XMFLOAT4X4 mWorld;

		Navmesh* mNavMesh;
		std::vector<DirectX::XMFLOAT3> mEdgePoints;

		UINT		 mNavTriangleCount;

		PointLight*		mPointLights;
		UINT			mPointLightCount;

	protected:
	public:
		bool			mLightsRegistered;

	private:
		void				GetNavigationData();
	protected:
	public:

		HRESULT				Update( float deltaTime );
		HRESULT				Render( float environmentTimer );

		Navmesh*			GetNavMesh() const;

		DirectX::XMFLOAT3	GetClosestEdgePoint( DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 goal );
		DirectX::XMFLOAT3	GetPos() const;
		void				SetPos( DirectX::XMFLOAT3 pos );

		int					GetNodeSizeX();
		int					GetNodeSizeY();

		void				SetNodeID( int ID );
		int					GetNodeID() const;

		void				SetInstanceID( int ID );
		int					GetInstanceID() const;

		void				ReleaseInstance();

		DirectX::XMFLOAT3	GetOrigin() const;
		
		MapNode*			GetMapNode() const;
		void				SetMapNode( MapNode* mapNode );

		void				ResetLights();

		HRESULT				Initialize();
		void				Release();
	
							MapNodeInstance();
		virtual				~MapNodeInstance();
};
#endif