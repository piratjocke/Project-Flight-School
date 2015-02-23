#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "RemotePlayer.h"
#include "Map.h"
#include "BaseState.h"
#include "Player.h"
#include "MapNodeManager.h"
#include "Font.h"
#include "Enemy.h"
#include "ClientShip.h"
#include "Image.h"
#include "EnemySpawn.h"
#include "RemoteEnemy.h"
#include "EnemyAnimationManager.h"
#include "Gui.h"
#include "SoundBufferHandler.h"
#include "EnergyCell.h"

//TestUpgradeWindow
#include "UpgradeShipWindow.h"


#define MAX_REMOTE_PLAYERS		14 //There is only 14 colorIDs.
#define COLLISION_CHECK_OFFSET	1	// 0 == Every frame
#define TOSERVER_OFFSET			5000

class PlayState : public BaseState
{
	// Class members
	private:
		float			mFPS;
		AnimationTrack	mTestAnimation;
		AssetID			mTestStaticAsset;
		Map*			mWorldMap;

		//Collision
		unsigned int	mFrameCounter;

		//Game Data
		Player*						mPlayer;
		std::vector<RemotePlayer*>	mRemotePlayers;
		Projectile**				mProjectiles;				// A collection of the games projectiles
		int							mNrOfActiveProjectiles;
		Font						mFont;
		EnemyAnimationManager*		mEnemyAnimationManager;
		RemoteEnemy**				mEnemies;
		XMFLOAT3*					mSpawners;
		AssetID						mSpawnModel;
		bool						mEnemyListSynced;
		bool						mServerInitialized;

		RADAR_UPDATE_INFO			mRadarObjects[MAX_RADAR_OBJECTS];
		float						mAlliesHP[MAX_REMOTE_PLAYERS / 2];
		Gui*						mGui;

		ClientShip*					mFriendShip;	// A pointer to the Client's own ship, used to show the remaining HP of it
		ClientShip*					mEnemyShip;		// A pointer to the other team's ship, used for interaction with that
		EnergyCell**				mEnergyCells;

		//TestSound
		int							m3DSoundAsset;
		int							mSoundAsset;
		int							mStreamSoundAsset;
	
	protected:
	public:

	// Class functions
	private:
		void			EventListener( IEventPtr newEvent );
		void			SyncEnemy( unsigned int id, EnemyState state, EnemyType type, XMFLOAT3 position, XMFLOAT3 direction );
		void			SyncSpawn( unsigned int id, XMFLOAT3 position );
		void			BroadcastProjectileDamage( unsigned int playerID, unsigned int projectileID );						// Tell server that local  player has taken damage
		void			BroadcastMeleeDamage( unsigned playerID, float damage, float knockBack, XMFLOAT3 direction );
		void			BroadcastEnemyProjectileDamage( unsigned int shooterID, unsigned int projectileID, unsigned int enemyID, float damage );
		void			BroadcastEnemyMeleeDamage( unsigned enemyID, float damage, float knockBack, XMFLOAT3 direction );
		void			FireProjectile( unsigned int id, unsigned int projectileID, XMFLOAT3 position, XMFLOAT3 direction, float speed, float range, float damage );
		void			CheckPlayerCollision();
		void			CheckProjectileCollision();
		void			CheckMeeleCollision();
		void			HandleDeveloperCameraInput();
		void			HandleRemoteProjectileHit( unsigned int id, unsigned int projectileID );
		void			UpdateProjectiles( float deltaTime );
		void			UpdateEnemyPosition( unsigned int id, XMFLOAT3 position, XMFLOAT3 direction, bool isAlive );
		void			RenderProjectiles();
		void			SetEnemyState( unsigned int id, EnemyState state );
	

	protected:
	public:
		virtual HRESULT Update( float deltaTime );
		virtual HRESULT Render();
		virtual void	OnEnter();
		virtual void	OnExit();
		virtual void	Reset();

		virtual HRESULT Initialize();
		virtual void	Release();
						PlayState();
		virtual			~PlayState();
};

#endif