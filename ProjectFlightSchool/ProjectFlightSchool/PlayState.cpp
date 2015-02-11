#include "PlayState.h"

///////////////////////////////////////////////////////////////////////////////
//									PRIVATE
///////////////////////////////////////////////////////////////////////////////

void PlayState::EventListener( IEventPtr newEvent )
{
	if ( newEvent->GetEventType() == Event_Local_Joined::GUID ) // Add a remote player to the list when they connect
	{
		std::shared_ptr<Event_Local_Joined> data = std::static_pointer_cast<Event_Local_Joined>( newEvent );
		if ( mPlayer != nullptr )
		{
			mPlayer->SetID( data->ID() );
			mPlayer->SetTeam( data->TeamID() );

			//TestSound
			SoundBufferHandler::GetInstance()->Play( mSoundAsset );
		}
	}

	else if ( newEvent->GetEventType() == Event_Remote_Joined::GUID ) // Add a remote player to the list when they connect
	{
		std::shared_ptr<Event_Remote_Joined> data = std::static_pointer_cast<Event_Remote_Joined>( newEvent );
		mRemotePlayers.push_back( new RemotePlayer() );
		mRemotePlayers.at(mRemotePlayers.size() - 1)->Initialize();
		mRemotePlayers.at(mRemotePlayers.size() - 1)->RemoteInit( data->ID(), data->TeamID() );
	}
	else if ( newEvent->GetEventType() == Event_Remote_Left::GUID ) // Remove a remote player from the list when they disconnect
	{
		std::shared_ptr<Event_Remote_Left> data = std::static_pointer_cast<Event_Remote_Left>( newEvent );
		for( unsigned int i = 0; i < mRemotePlayers.size(); i++ )
		{
			if( !mRemotePlayers.at(i) )
			{
				continue;
			}
			else if( data->ID() == mRemotePlayers.at(i)->GetID() )
			{
				mRemotePlayers.at(i)->Release();
				SAFE_DELETE( mRemotePlayers.at(i) );
				std::swap( mRemotePlayers.at(i), mRemotePlayers.at(mRemotePlayers.size() - 1) );
				mRemotePlayers.pop_back();
				break;
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Remote_Damaged::GUID )
	{
		// Damage remote player
		std::shared_ptr<Event_Remote_Damaged> data = std::static_pointer_cast<Event_Remote_Damaged>(newEvent);
		HandleRemoteProjectileHit( data->ID(), data->ProjectileID() );
	}
	else if ( newEvent->GetEventType() == Event_Remote_Fired_Projectile::GUID )
	{
		// Fire projectile
		std::shared_ptr<Event_Remote_Fired_Projectile> data = std::static_pointer_cast<Event_Remote_Fired_Projectile>(newEvent);
		FireProjectile( data->ID(), data->ProjectileID(), data->BodyPos(), data->Direction(), data->Speed(), data->Range() );

		//TestSound
		SoundBufferHandler::GetInstance()->Play3D( m3DSoundAsset , data->BodyPos());
		
		// Request Muzzle Flash from Particle Manager
		mParticleManager->RequestParticleSystem( data->ID(), MuzzleFlash, data->BodyPos(), data->Direction() );
	}
	else if ( newEvent->GetEventType() == Event_Server_Create_Enemy::GUID )
	{
		std::shared_ptr<Event_Server_Create_Enemy> data = std::static_pointer_cast<Event_Server_Create_Enemy>( newEvent );
		SyncEnemy( data->ID(), (EnemyState)data->State(), (EnemyType)data->Type(), data->Position(), data->Direction() );
	}
	else if ( newEvent->GetEventType() == Event_Server_Update_Enemy::GUID )
	{
		std::shared_ptr<Event_Server_Update_Enemy> data = std::static_pointer_cast<Event_Server_Update_Enemy>( newEvent );
		UpdateEnemyPosition( data->ID(), data->Position(), data->Direction(), data->IsAlive() );
	}
	else if ( newEvent->GetEventType() == Event_Server_Enemies_Created::GUID )
	{
		mEnemyListSynced = true;
	}
	else if ( newEvent->GetEventType() == Event_Connect_Server_Success::GUID )
	{
		mServerInitialized = true;
	}
	else if ( newEvent->GetEventType() == Event_Server_Sync_Spawn::GUID )
	{
		std::shared_ptr<Event_Server_Sync_Spawn> data = std::static_pointer_cast<Event_Server_Sync_Spawn>( newEvent );
		SyncSpawn( data->ID(), data->Position() );
	}
	else if ( newEvent->GetEventType() == Event_Server_Sync_Enemy_State::GUID )
	{
		std::shared_ptr<Event_Server_Sync_Enemy_State> data = std::static_pointer_cast<Event_Server_Sync_Enemy_State>( newEvent );
		SetEnemyState( data->ID(), (EnemyState)data->State() );
	}
}

void PlayState::SyncEnemy( unsigned int id, EnemyState state, EnemyType type, XMFLOAT3 position, XMFLOAT3 direction )
{
	mEnemyListSynced = false;
	mEnemies[id]->SetID( id );
	mEnemies[id]->SetEnemyType( type );
	mEnemies[id]->SetModelID( mEnemyAnimationManager->GetModel( mEnemies[id]->GetEnemyType() ), mEnemyAnimationManager->GetDefaultAnimation( mEnemies[id]->GetEnemyType() ) );
	mEnemies[id]->SetAnimation( mEnemyAnimationManager->GetAnimation( mEnemies[id]->GetEnemyType(), state ) );
	mEnemies[id]->SetPosition( position );
	mEnemies[id]->SetDirection( direction );
	mEnemies[id]->SetSynced( true );

	if( id == (MAX_NR_OF_ENEMIES-1) )
		mEnemyListSynced = true;
}

void PlayState::SyncSpawn( unsigned int id, XMFLOAT3 position )
{
	mSpawners[id] = position;
}

// Tell server that local  player has taken damage
void PlayState::BroadcastProjectileDamage( unsigned int playerID, unsigned int projectileID )
{
	IEventPtr E1( new Event_Client_Damaged( playerID, projectileID ) );
	Client::GetInstance()->SendEvent( E1 );
}

void PlayState::BroadcastMeleeDamage( unsigned playerID, float damage, float knockBack, XMFLOAT3 direction )
{
	IEventPtr E1( new Event_Client_Melee_Hit( playerID, damage, knockBack, direction ) );
	Client::GetInstance()->SendEvent( E1 );
}

void PlayState::BroadcastEnemyMeleeDamage( unsigned enemyID, float damage, float knockBack, XMFLOAT3 direction )
{
	IEventPtr E1( new Event_Client_Enemy_Attack( mPlayer->GetID(), enemyID, damage, knockBack, direction, mPlayer->GetLoadOut()->meleeWeapon->stun ) );
	Client::GetInstance()->SendEvent( E1 );
}

void PlayState::BroadcastEnemyProjectileDamage( unsigned int shooterID, unsigned int projectileID, unsigned int enemyID, float damage )
{
	IEventPtr E1( new Event_Client_Projectile_Damage_Enemy( shooterID, projectileID, enemyID, damage ) );
	Client::GetInstance()->SendEvent( E1 );
}

void PlayState::FireProjectile( unsigned int id, unsigned int projectileID, XMFLOAT3 position, XMFLOAT3 direction, float speed, float range )
{
	mProjectiles[mNrOfActiveProjectiles]->SetDirection( id, projectileID, position, direction, speed, range );
	mNrOfActiveProjectiles++;
}

//void PlayState::FireProjectile( unsigned int id, unsigned int projectileID, XMFLOAT3 position, XMFLOAT3 direction, float speed, float range )
//{
//	//mNrOfProjectilesFired = mNrOfProjectilesFired % MAX_PROJECTILES;
//	mProjectiles[mNrOfActiveProjectiles]->SetDirection( id, projectileID, position, direction, speed, range );
//	//mProjectiles[mNrOfProjectilesFired % MAX_PROJECTILES]->SetIsActive( true );
//	//mNrOfProjectilesFired++;
//}

//void PlayState::BroadcastEnemyMeleeDamage( unsigned enemyID, float damage, float knockBack, XMFLOAT3 direction )
//{
//	IEventPtr E1( new Event_Client_Enemy_Attack( mPlayer->GetID(), enemyID, damage, knockBack, direction, mPlayer->GetLoadOut()->meleeWeapon->stun ) );
//	Client::GetInstance()->SendEvent( E1 );
//}

void PlayState::CheckPlayerCollision()
{	
	if( mRemotePlayers.size() > 0 )
	{
		if( mRemotePlayers.at(0)->GetID() != 0 )
		{
			for (size_t i = 0; i < mRemotePlayers.size(); i++)
			{
				//if( mPlayer->GetBoundingBox()->Intersect( mRemotePlayers.at(i)->GetBoundingBox() ) )		// BoundingBox
				if (mPlayer->GetBoundingCircle()->Intersect(mRemotePlayers.at(i)->GetBoundingCircle()))		// BoundingCircle
				{
					//Direction
					XMVECTOR remoteToPlayerVec = (XMLoadFloat3(&mPlayer->GetBoundingCircle()->center) -
						XMLoadFloat3(&mRemotePlayers.at(i)->GetBoundingCircle()->center));

					//Normalize direction
					remoteToPlayerVec = XMVector4Normalize(remoteToPlayerVec);

					//Length of new vector
					float vectorLength = (mRemotePlayers.at(i)->GetBoundingCircle()->radius + mPlayer->GetBoundingCircle()->radius) + 0.0001f;

					//New position of player			 
					XMVECTOR playerPosition = XMLoadFloat3(&mRemotePlayers.at(i)->GetBoundingCircle()->center) + remoteToPlayerVec * vectorLength;
					mPlayer->SetPosition(playerPosition);
				}
			}	
		}
	}
}

void PlayState::CheckProjectileCollision()
{
	for ( int i	 = 0; i < mNrOfActiveProjectiles; i++ )
	{
		// Players
		if( mProjectiles[i]->IsActive() ) //&& mRemotePlayers.size() > 0 )
		{
			for ( size_t j = 0; j < mRemotePlayers.size(); j++ )
			{
				if( mProjectiles[i]->GetPlayerID() == mPlayer->GetID() && mProjectiles[i]->GetBoundingCircle()->Intersect( mRemotePlayers[j]->GetBoundingCircle() ) )
				{
					BroadcastProjectileDamage( mRemotePlayers[j]->GetID(), mProjectiles[i]->GetID() );
					return;
				}
			}

			// Enemies
			for ( size_t j = 0; j < MAX_NR_OF_ENEMIES; j++ )
			{
				if( mEnemies[j]->IsAlive() )
				{
					if( mProjectiles[i]->GetPlayerID() == mPlayer->GetID() && mProjectiles[i]->GetBoundingCircle()->Intersect( mEnemies[j]->GetBoundingCircle() ) )
					{
						// hit
						BroadcastEnemyProjectileDamage( mProjectiles[i]->GetPlayerID(), mProjectiles[i]->GetID(), mEnemies[j]->GetID(), mPlayer->GetLoadOut()->rangedWeapon->damage );
						mProjectiles[i]->Reset();
						return;
					}
				}
			}
		}
	}
}

void PlayState::CheckMeeleCollision()
{
	for ( size_t i = 0; i < mRemotePlayers.size(); i++ )
	{
		//Check intersection with melee circle & remotePlayer
		if( mPlayer->GetLoadOut()->meleeWeapon->boundingCircle->Intersect( mRemotePlayers.at(i)->GetBoundingCircle() ) )
		{
			XMVECTOR meeleRadiusVector =  ( XMLoadFloat3( &mPlayer->GetUpperBodyDirection() ) * mPlayer->GetLoadOut()->meleeWeapon->radius );

			//Spread to Radians
			float halfRadian = XMConvertToRadians( mPlayer->GetLoadOut()->meleeWeapon->spread * 18.0f ) * 0.5f;

			XMVECTOR playerToRemote = XMLoadFloat3( &mRemotePlayers.at(i)->GetPosition() ) - XMLoadFloat3( &mPlayer->GetPosition() );

			float angleRemoteToAim = 0.0f;
			XMVECTOR playerToCenter = XMLoadFloat3( &mRemotePlayers.at(i)->GetBoundingCircle()->center ) - XMLoadFloat3( &mPlayer->GetPlayerPosition() );
			XMStoreFloat( &angleRemoteToAim, XMVector4AngleBetweenVectors( playerToCenter, meeleRadiusVector ) );

			if( angleRemoteToAim <= halfRadian )
			{
				XMFLOAT3 direction = XMFLOAT3( 0.0f, 0.0f, 0.0f );
				XMStoreFloat3( &direction, XMVector4Normalize( XMLoadFloat3( &mPlayer->GetUpperBodyDirection() ) ) );
				BroadcastMeleeDamage( mRemotePlayers.at(i)->GetID(), mPlayer->GetLoadOut()->meleeWeapon->damage, mPlayer->GetLoadOut()->meleeWeapon->knockBack, direction );
			}
		}
	}

	for ( size_t i = 0; i < MAX_NR_OF_ENEMIES; i++ )
	{
		//Check intersection with melee circle & enemy
		if( mPlayer->GetLoadOut()->meleeWeapon->boundingCircle->Intersect( mEnemies[i]->GetBoundingCircle() ) )
		{
			XMVECTOR meeleRadiusVector =  ( XMLoadFloat3( &mPlayer->GetUpperBodyDirection() ) * mPlayer->GetLoadOut()->meleeWeapon->radius );

			//Spread to Radians
			float halfRadian = XMConvertToRadians( mPlayer->GetLoadOut()->meleeWeapon->spread * 18.0f ) * 0.5f;

			XMVECTOR playerToRemote = XMLoadFloat3( &mEnemies[i]->GetPosition() ) - XMLoadFloat3( &mPlayer->GetPosition() );

			float angleRemoteToAim = 0.0f;
			XMVECTOR playerToCenter = XMLoadFloat3( &mEnemies[i]->GetBoundingCircle()->center ) - XMLoadFloat3( &mPlayer->GetPlayerPosition() );
			XMStoreFloat( &angleRemoteToAim, XMVector4AngleBetweenVectors( playerToCenter, meeleRadiusVector ) );

			if( angleRemoteToAim <= halfRadian )
			{
				XMFLOAT3 direction = XMFLOAT3( 0.0f, 0.0f, 0.0f );
				XMStoreFloat3( &direction, XMVector4Normalize( XMLoadFloat3( &mPlayer->GetUpperBodyDirection() ) ) );
				BroadcastEnemyMeleeDamage(mEnemies[i]->GetID(), mPlayer->GetLoadOut()->meleeWeapon->damage, mPlayer->GetLoadOut()->meleeWeapon->knockBack, direction );
			}
		}
	}
}

void PlayState::HandleDeveloperCameraInput()
{
	// TOGGLE CAM
	if( Input::GetInstance()->IsKeyPressed( KEYS::KEYS_RCTRL ) )
		Graphics::GetInstance()->ChangeCamera();
	// ZOOM IN
	if( Input::GetInstance()->IsKeyDown( KEYS::KEYS_DOWN ) )
		Graphics::GetInstance()->ZoomOutDeveloperCamera();
	// ZOOM OUT
	if( Input::GetInstance()->IsKeyDown( KEYS::KEYS_UP) )
		Graphics::GetInstance()->ZoomInDeveloperCamera();
	if( Input::GetInstance()->IsKeyDown( KEYS::KEYS_Q ) )
	{
		IEventPtr E1( new Event_Reset_Game() );
		EventManager::GetInstance()->QueueEvent( E1 );
	}
}

void PlayState::HandleRemoteProjectileHit( unsigned int id, unsigned int projectileID )
{
	unsigned int shooter = 0;
	for ( int i = 0; i < mNrOfActiveProjectiles; i++ )
	{
		if( mProjectiles[i]->GetID() == projectileID )
		{
			shooter = mProjectiles[i]->GetPlayerID();
			mProjectiles[i]->Reset();
			Projectile* temp							= mProjectiles[mNrOfActiveProjectiles - 1];
			mProjectiles[mNrOfActiveProjectiles - 1]	= mProjectiles[i];
			mProjectiles[i]								= temp;
			mNrOfActiveProjectiles--;
		}
	}

	if( mPlayer->GetID() == id )
	{
		// Projectile damage
		for ( size_t i = 0; i < mRemotePlayers.size(); i++ )
		{
			if( mRemotePlayers.at(i)->GetID() == shooter )
			{
				if( mRemotePlayers.at(i)->GetTeam() != mPlayer->GetTeam() )
				{
					mPlayer->TakeDamage( mRemotePlayers.at(i)->GetLoadOut()->rangedWeapon->damage, shooter );
				}
			}
		}
	}
}

void PlayState::UpdateProjectiles( float deltaTime )
{
	for ( int i = 0; i < mNrOfActiveProjectiles; i++ )
	{
		if( mProjectiles[i]->IsActive() )
		{
			mProjectiles[i]->Update( deltaTime );
		}
		else
		{
			Projectile* temp							= mProjectiles[mNrOfActiveProjectiles - 1];
			mProjectiles[mNrOfActiveProjectiles - 1]	= mProjectiles[i];
			mProjectiles[i]								= temp;
			mProjectiles[mNrOfActiveProjectiles - 1]->Reset();
			mProjectiles[i]->Update( deltaTime );
			mNrOfActiveProjectiles--;
		}
	}
}

void PlayState::UpdateEnemyPosition( unsigned int id, XMFLOAT3 position, XMFLOAT3 direction, bool isAlive )
{
	mEnemies[id]->SetPosition( position );
	mEnemies[id]->SetDirection( direction );
	mEnemies[id]->SetIsAlive( isAlive );
}

void PlayState::RenderProjectiles()
{
 
	for ( int i = 0; i < mNrOfActiveProjectiles; i++ )
	{
		mProjectiles[i]->Render();
	}

}

void PlayState::SetEnemyState( unsigned int id, EnemyState state )
{
	if( state == Death )
	{
		mEnemies[id]->SetLoopAnimation( false );
		mEnemies[id]->SetIsAlive( false );

	}
	else
	{
		mEnemies[id]->SetLoopAnimation( true );
		mEnemies[id]->SetIsAlive( true );
	}

	mEnemies[id]->SetAnimation( mEnemyAnimationManager->GetAnimation( mEnemies[id]->GetEnemyType(), state ) );
}

///////////////////////////////////////////////////////////////////////////////
//									PUBLIC
///////////////////////////////////////////////////////////////////////////////

HRESULT PlayState::Update( float deltaTime )
{
	CheckProjectileCollision();

	//Fps update
	mFPS = mFPS * 0.1f + 0.9f / deltaTime;

	Client::GetInstance()->FillEventList( mPlayer->GetEvents() );
	mPlayer->ClearEventList();

	if( mFrameCounter >= COLLISION_CHECK_OFFSET )
	{
		CheckPlayerCollision();

		if( mPlayer->GetIsMeleeing() )
		{
			CheckMeeleCollision();
			mPlayer->SetIsMeleeing( false );
		}

		mFrameCounter = 0;
	}
	else
	{
		mFrameCounter++;
	}

	GuiUpdate guiUpdate;
	UINT nrOfRadarObj = 0;
	PlayerName pName[MAX_REMOTE_PLAYERS - 1];
	int nrOfAllies				= 0;
	guiUpdate.mNrOfPlayerNames	= (UINT)mRemotePlayers.size();
	guiUpdate.mPlayerTeamID		= mPlayer->GetTeam();

	for ( size_t i = 0; i < mRemotePlayers.size(); i++)
	{
		if ( mRemotePlayers.at(i) )
		{
			mRemotePlayers.at(i)->Update( deltaTime );
			std::string remotePlayerName = "";
			if( mRemotePlayers[i]->IsAlive() )
			{
				remotePlayerName							= mRemotePlayers[i]->GetName();
				mRadarObjects[nrOfRadarObj].mRadarObjectPos = mRemotePlayers[i]->GetPosition();
				mRadarObjects[nrOfRadarObj++].mType			= RADAR_TYPE::HOSTILE;
			}
			pName[i].mRemotePlayerPos		= mRemotePlayers[i]->GetPosition();
			pName[i].mRemotePlayerName		= remotePlayerName;
			pName[i].mRemotePlayerTeamID	= mRemotePlayers[i]->GetTeam();
			pName[i].mRemotePlayerID		= i;

			if( mRemotePlayers[i]->GetTeam() == mPlayer->GetTeam() )
			{
				mAlliesHP[nrOfAllies] = (float)( mRemotePlayers[i]->GetHP() / mRemotePlayers[i]->GetMaxHP() );
				nrOfAllies++;
			}
		}
	}

	guiUpdate.mPlayerNames	= pName;
	guiUpdate.mNrOfAllies	= nrOfAllies;
	guiUpdate.mAlliesHP		= mAlliesHP;
	guiUpdate.mShipHP		= 1.0f;

	mPlayer->Update( deltaTime, mRemotePlayers );

	HandleDeveloperCameraInput();

	UpdateProjectiles( deltaTime );

	//Test radar due to no ship :(
	mRadarObjects[nrOfRadarObj].mRadarObjectPos = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );//mShip.GetPosition();
	mRadarObjects[nrOfRadarObj++].mType = RADAR_TYPE::HOSTILE;
	//-------

	// Enemies
	if( mEnemyListSynced )
	{
		for ( size_t i = 0; i < MAX_NR_OF_ENEMIES; i++ )
		{
			if( mEnemies[i]->IsSynced() )
			{
				mEnemies[i]->Update( deltaTime );
				if( mEnemies[i]->IsAlive() )
				{
					mRadarObjects[nrOfRadarObj].mType = RADAR_TYPE::HOSTILE;
					mRadarObjects[nrOfRadarObj++].mRadarObjectPos = mEnemies[i]->GetPosition();
				}
			}
		}
	}
	mParticleManager->Update( deltaTime );

	
	guiUpdate.mRadarObjects	= mRadarObjects;
	guiUpdate.mNrOfObjects	= nrOfRadarObj;
	guiUpdate.mPlayerPos	= mPlayer->GetPlayerPosition();	

	guiUpdate.mPlayerHP		= (float)( mPlayer->GetHP() / mPlayer->GetMaxHP() );
	guiUpdate.mPlayerXP		= (float)( mPlayer->GetHP() / mPlayer->GetMaxHP() );
	guiUpdate.mPlayerShield	= (float)( mPlayer->GetHP() / mPlayer->GetMaxHP() );
	
	mGui->Update( guiUpdate );

	// Test Anim
	///////////////////////////////////////////////////////////////////////////
	//RenderManager::GetInstance()->AnimationUpdate( mTestAnimation, deltaTime );
	///////////////////////////////////////////////////////////////////////////

	return S_OK;
}

HRESULT PlayState::Render()
{
	mPlayer->Render( 0.0f, 1 );

	mWorldMap->Render( 0.0f , mPlayer );

	for ( size_t i = 0; i < mRemotePlayers.size(); i++)
	{
		if ( mRemotePlayers.at(i) )
		{
			mRemotePlayers.at(i)->Render( i + 2 );
		}
	}

	RenderProjectiles();

	// Enemies
  	if( mEnemyListSynced )
	{
		for ( size_t i = 0; i < MAX_NR_OF_ENEMIES; i++ )
		{
			if( mEnemies[i]->IsSynced() )
				mEnemies[i]->Render();
		}
	}

	for (size_t i = 0; i < MAX_NR_OF_ENEMY_SPAWNERS; i++)
	{
		RenderManager::GetInstance()->AddObject3dToList( mSpawnModel, mSpawners[i] );
	}

	mParticleManager->Render( 0.0f );

	
	mGui->Render();

	//RENDER DEVTEXT
	std::string textToWrite = "FPS\t" + std::to_string( (int)mFPS ) + "\nRemotePlayers\t" + std::to_string( mRemotePlayers.size() ) + "\nActiveProjectiles\t" + std::to_string( mNrOfActiveProjectiles );
	mFont.WriteText( textToWrite, 40.0f, 200.0f, 2.0f );

	RenderManager::GetInstance()->Render();

	return S_OK;
}

void PlayState::OnEnter()
{
	Reset();
	// Send Game Started event to server
	IEventPtr E1( new Event_Game_Started() );
	EventManager::GetInstance()->QueueEvent( E1 );
}

void PlayState::OnExit()
{
	// Send Game Started event to server
	IEventPtr E1( new Event_Game_Ended() );
	EventManager::GetInstance()->QueueEvent( E1 );
}

void PlayState::Reset()
{
	
	mPlayer->Reset();
	for( size_t i = 0; i < MAX_PROJECTILES; i++ )
		mProjectiles[i]->Reset();

	mNrOfActiveProjectiles = 0;

	mEnemyListSynced		= false;
	mServerInitialized		= false;

	for( auto& rp : mRemotePlayers )
	{
		rp->Release();
		SAFE_DELETE( rp );
	}
	mRemotePlayers.clear();

}

HRESULT PlayState::Initialize()
{
	mStateType = PLAY_STATE;

	//AssetID model	= 0;
	//AssetID loader	= 0;

	/*Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/Enemies/Blowuposaur/Animations/", "blowuposaurSkel.Skel", loader );
	Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/Enemies/Blowuposaur/", "blowuposaur.apfs", loader, model );
	Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/Enemies/Blowuposaur/Animations/", "blowuposaurIdle.PaMan", loader );*/

	//RenderManager::GetInstance()->AnimationInitialize( mTestAnimation, model, loader );

	mPlayer = new Player();
	mPlayer->Initialize();

	mWorldMap = new Map();
	mWorldMap->Initialize( 8 );

	IEventPtr E1( new Event_Load_Level("../Content/Assets/Nodes/ForestMap.xml")); 
	EventManager::GetInstance()->TriggerEvent( E1 );

	//Fill up on Projectiles, test values
	mProjectiles	= new Projectile*[MAX_PROJECTILES];
	for ( size_t i = 0; i < MAX_PROJECTILES; i++ )
	{
		mProjectiles[i] = new Projectile();
		mProjectiles[i]->Initialize();
	}

	mNrOfActiveProjectiles	= 0;

	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Local_Joined::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Remote_Joined::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Remote_Left::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Remote_Damaged::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Remote_Fired_Projectile::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Server_Create_Enemy::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Server_Enemies_Created::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Connect_Server_Success::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Server_Sync_Spawn::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Server_Update_Enemy::GUID );
	EventManager::GetInstance()->AddListener( &PlayState::EventListener, this, Event_Server_Sync_Enemy_State::GUID ); 

	mFont.Initialize( "../Content/Assets/Fonts/final_font/" );

	// Enemies
	mEnemyAnimationManager = new EnemyAnimationManager();
	mEnemyAnimationManager->Initialize();

	mEnemies	= new RemoteEnemy*[MAX_NR_OF_ENEMIES];
	for ( size_t i = 0; i < MAX_NR_OF_ENEMIES; i++ )
	{
		mEnemies[i] = new RemoteEnemy();
		mEnemies[i]->Initialize( i, mEnemyAnimationManager->GetModel( Standard ), mEnemyAnimationManager->GetAnimation( Standard, Idle ) );
	}

	Graphics::GetInstance()->LoadStatic3dAsset( "../Content/Assets/Nests/", "nest_2.pfs", mSpawnModel );
	mSpawners	= new XMFLOAT3[MAX_NR_OF_ENEMY_SPAWNERS];


	//ParticleManager
	mParticleManager = new ParticleManager();
	mParticleManager->Initialize();

	mGui = new Gui();
	mGui->Initialize();

	//TestSound
	m3DSoundAsset	= SoundBufferHandler::GetInstance()->Load3DBuffer( "../Content/Assets/Sound/alert02.wav" );
	mSoundAsset		= SoundBufferHandler::GetInstance()->LoadBuffer( "../Content/Assets/Sound/alert02.wav" );

	return S_OK;
}

void PlayState::Release()
{	
	mWorldMap->Release();
	SAFE_DELETE( mWorldMap );

	mPlayer->Release();
	SAFE_DELETE(mPlayer);

	for( auto& rp : mRemotePlayers )
	{
		rp->Release();
		SAFE_DELETE( rp );
	}

	mRemotePlayers.clear();

	for ( size_t i = 0; i < MAX_PROJECTILES; i++ )
	{
		mProjectiles[i]->Release();
		SAFE_DELETE( mProjectiles[i] );
	}
	delete [] mProjectiles;

	// Enemies
	SAFE_DELETE( mEnemyAnimationManager );

	for ( size_t i = 0; i < MAX_NR_OF_ENEMIES; i++ )
		SAFE_RELEASE_DELETE( mEnemies[i] );

	delete [] mEnemies;

	delete [] mSpawners;

	mFont.Release();

	SAFE_RELEASE_DELETE( mParticleManager );

	mGui->Release();
	SAFE_DELETE( mGui );

}

PlayState::PlayState()
{
	mFPS					= 0.0f;
	mPlayer					= nullptr;
	mRemotePlayers			= std::vector<RemotePlayer*>( 0 );
	mRemotePlayers.reserve(MAX_REMOTE_PLAYERS);
	mEnemyAnimationManager	= nullptr;
	mFrameCounter			= 0;
	mProjectiles			= nullptr;
	mNrOfActiveProjectiles	= 0;
	mEnemies				= nullptr;
	mEnemyListSynced		= false;
	mServerInitialized		= false;
	mParticleManager		= nullptr;
	mGui					= nullptr;
}

PlayState::~PlayState()
{
}