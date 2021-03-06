#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

#include "EventManager.h"
#include "Events.h"
#include <Graphics.h>
#include "BoundingGeometry.h"
#include "RenderManager.h"
#include "WeaponInfo.h"

#define MAX_PROJECTILES			1000

class Projectile
{
	private:
		AssetID				mProjectileAsset;
		AssetID				mSniperProjectileAsset;
		unsigned int		mID;
		unsigned int		mTeamID;
		unsigned int		mPlayerID;
		XMFLOAT3			mPosition;
		XMFLOAT3			mDirection;
		float				mSpeed;
		bool				mIsActive;
		float				mLifeTime;
		BoundingCircle*		mBoundingCircle;
		float				mDamage;
		WeaponType			mWeaponType;
		std::vector<UINT>	mHits;

	public:
		HRESULT			Update( float deltaTime );
		HRESULT			Render();

		void			SetDirection( unsigned int playerID, unsigned int id, unsigned int teamID, XMFLOAT3 startPosition, XMFLOAT3 direction, float speed, float range, float damage, WeaponType weaponType );
		void			SetIsActive( bool isActive );
		bool			IsActive() const;
		void			Reset();
		BoundingCircle*	GetBoundingCircle() const;
		unsigned int	GetID() const;
		unsigned int	GetTeamID() const;
		unsigned int	GetPlayerID() const;
		float			GetDamage() const;
		XMFLOAT3		GetDirection() const;
		XMFLOAT3		GetPosition() const;
		WeaponType		GetWeaponType() const;
		bool			SetHit( UINT hitID );

		HRESULT			Initialize();
		void			Release();

						Projectile();
						~Projectile();
};

#endif