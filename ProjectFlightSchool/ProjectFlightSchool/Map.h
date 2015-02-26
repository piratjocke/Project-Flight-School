#ifndef MAP_H
#define MAP_H
#include "MapSection.h"
#include "BoundingGeometry.h"
#include "NodeGraph.h"
#define MAX_NODES 1000

static enum MapSize
{
	LARGE = 1200,
	MEDIUM = 600,
	SMALL = 300
};

class Map
{
	private:
		MapNodeInstance***	mBuildMap;
		float				mVertexSpacing;
		UINT				mNrOfNodes;
		UINT				mMapId;

	protected:
	public:
		UINT mMapDim;
	private:
	protected:
	public:
		HRESULT				Render( float deltaTime, Player* player );

		void				OnLoadLevel( IEventPtr E1 );
		//std::vector<DirectX::XMFLOAT2> GetPath( XMFLOAT3 start, XMFLOAT3 goal );
		UINT				GetMapDim() const;
		UINT				GetMapWidth() const;
		UINT				GetMapHeight() const;
		UINT				GetMapHalfWidth() const;
		UINT				GetMapHalfHeight() const;
		float				GetHeight( DirectX::XMFLOAT3 pos );
		UINT				GetNrOfNodes() const;
		MapNodeInstance***	GetNodeMap() const;
		MapNodeInstance*	GetNodeInstance( int x, int z );

		HRESULT				Initialize( UINT mapDim );
		void				Release();
		Map();

		~Map();
};
#endif