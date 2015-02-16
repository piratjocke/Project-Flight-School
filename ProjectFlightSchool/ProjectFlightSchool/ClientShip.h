#ifndef CLIENTSHIP_H
#define CLIENTSHIP_H

#include "RenderManager.h"
#include "ServerShip.h"

class ClientShip : public ServerShip
{
	private:
		// Turret? Buffcircle?
		AssetID mAssetID;
		BoundingCircle* mHitCircle;

	protected:
	public:

	private:
		void CalcShieldLevel();

		void RemoteUpdateShip( IEventPtr eventPtr );
		void RemoteChangeShipLevels( IEventPtr eventPtr );

		void Reset( UINT id, UINT teamID, XMFLOAT3 pos, XMFLOAT3 dir );

	protected:
	public:
		UINT	GetID() const;
		UINT	GetTeamID() const;

		void	CalculatePlayerRespawnPosition( IEventPtr eventPtr );
		bool	Intersect( BoundingCircle* entity ); // Will check for intersects with damaging stuff
		void	Update( float deltaTime );
		void	Render();
		void	Initialize( UINT id, UINT teamID, XMFLOAT3 pos, XMFLOAT3 dir );
		void	Release();
				ClientShip();
		virtual	~ClientShip();
};
#endif