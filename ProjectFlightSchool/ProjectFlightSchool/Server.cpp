#include "Server.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Start of eventlistening functions

void Server::ClientJoined( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Joined::GUID )
	{
		std::shared_ptr<Event_Client_Joined> data = std::static_pointer_cast<Event_Client_Joined>( eventPtr );
		UINT id = data->ID();
		UINT teamID = CurrentTeamDelegate();

		mClientMap[id].NEF.Initialize( id, mSocketManager );
		mClientMap[id].TeamID = teamID;
		mClientMap[id].Circle.radius = 0.5f; // This will be a cause for bugs later, check here if player collision is fucked up

		// Sends necessary information of the newly connected client to the newly connected client
		IEventPtr E1( new Event_Local_Joined( id, teamID ) );
		SendEvent( E1, id );

		// Initializes the ships in their current state to the newly connected client
		for( auto& it : mShips )
		{
			ServerShip* s = &it.second;
			IEventPtr SpawnShip( new Event_Server_Spawn_Ship( s->mID, s->mTeamID, s->mPos, s->mDir, s->mCurrentHP ) );
			SendEvent( SpawnShip, id );
		}

		// Sends the incoming ID to the existing remotes
		IEventPtr E2( new Event_Remote_Joined( id, teamID ) ); 
		BroadcastEvent( E2, id );

		// Sends the list of existing remotes to the newly connected client
		for( auto& remote : mClientMap )
		{
			if( remote.first != id )
			{
				IEventPtr E3( new Event_Remote_Joined( remote.first, remote.second.TeamID ) ); // The key of the map is the ID of the remote
				SendEvent( E3, id );
			}
		}
	}
}

void Server::ClientLeft( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Left::GUID )
	{
		std::shared_ptr<Event_Client_Left> data = std::static_pointer_cast<Event_Client_Left>( eventPtr );
		UINT id = data->ID();
		mClientMap.erase( id );

		IEventPtr E1( new Event_Remote_Left( id ) );
		BroadcastEvent( E1 );
	}
}

void Server::ClientUpdate( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Update::GUID )
	{
		std::shared_ptr<Event_Client_Update> data = std::static_pointer_cast<Event_Client_Update>( eventPtr );
		UINT id = data->ID();
		XMFLOAT3 pos = data->LowerBodyPos();
		XMFLOAT3 vel = data->Velocity();
		XMFLOAT3 dir = data->UpperBodyDirection();
		std::string name = data->Name();

		mClientMap[id].Circle.center = pos;

		IEventPtr E1( new Event_Remote_Update( id, pos, vel, dir, name ) );
		BroadcastEvent( E1, id );
	}
}

void Server::ClientDied( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Died::GUID )
	{
		std::shared_ptr<Event_Client_Died> data = std::static_pointer_cast<Event_Client_Died>( eventPtr );
		UINT id = data->ID();
		UINT killerID = data->KillerID();

		IEventPtr E1( new Event_Remote_Died( id, killerID ) );
		BroadcastEvent( E1, id );
	}
}

void Server::ClientDamaged( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Damaged::GUID )
	{
		std::shared_ptr<Event_Client_Damaged> data = std::static_pointer_cast<Event_Client_Damaged>( eventPtr );
		UINT id = data->ID();
		UINT projectileID = data->ProjectileID();

		IEventPtr E1( new Event_Remote_Damaged( id, projectileID ) );
		BroadcastEvent( E1 );
	}
}

void Server::ClientSpawned( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Spawned::GUID )
	{
		std::shared_ptr<Event_Client_Spawned> data = std::static_pointer_cast<Event_Client_Spawned>( eventPtr );
		UINT id = data->ID();

		IEventPtr E1( new Event_Remote_Spawned( id ) );
		BroadcastEvent( E1, id );
	}
}

void Server::ClientFiredProjectile( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Fired_Projectile::GUID )
	{
		std::shared_ptr<Event_Client_Fired_Projectile> data = std::static_pointer_cast<Event_Client_Fired_Projectile>( eventPtr );
		UINT id = data->ID();
		UINT pid = CurrentPID();
		XMFLOAT3 pos = data->BodyPos();
		XMFLOAT3 dir = data->Direction();

		IEventPtr E1( new Event_Remote_Fired_Projectile( id, pid, pos, dir ) );
		BroadcastEvent( E1 );
	}
}

void Server::ClientUpdateHP( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Update_HP::GUID )
	{
		std::shared_ptr<Event_Client_Update_HP> data = std::static_pointer_cast<Event_Client_Update_HP>( eventPtr );
		UINT id = data->ID();
		float hp = data->HP();

		IEventPtr E1( new Event_Remote_Update_HP( id, hp ) );
		BroadcastEvent( E1, id );
	}
}

void Server::ClientMeleeHit( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Melee_Hit::GUID )
	{
		std::shared_ptr<Event_Client_Melee_Hit> data = std::static_pointer_cast<Event_Client_Melee_Hit>( eventPtr );
		UINT id = data->ID();
		float damage = data->Damage();
		float knockBack = data->KnockBack();
		XMFLOAT3 dir = data->Direction();

		IEventPtr E1( new Event_Remote_Melee_Hit( id, damage, knockBack, dir ) );
		BroadcastEvent( E1 );
	}
}

//void Server::StateCheck()
//{
//	if( mEnemies[i]->IsAlive() )
//	{
//		for ( size_t j = 0; j < mNrOfPlayers; j++ )
//		{
//			// The players agg circle
//			mAggCircle->center = mPlayers[j].Position;
//			mAggCircle->radius = 1.0f;
//
//			if( mPlayers[j].IsAlive && mEnemies[i]->GetAttentionCircle()->Intersect( mAggCircle ) && mEnemies[i]->GetEnemyState() != Stunned )
//			{
//				if( mEnemies[i]->GetAttackCircle()->Intersect( mAggCircle ) )
//				{
//					mEnemies[i]->SetState( Attack );
//					float attack = mEnemies[i]->HandleAttack();
//					if( attack != 0.0f )
//					{
//						EvEnemyAttackPlayer enemyAtk;
//						enemyAtk.playerID	= mPlayers[j].ID;
//						enemyAtk.damage		= attack;
//						for ( auto& socket : mClientSockets )
//						{
//							if ( socket.s != INVALID_SOCKET && mEnemyListSynced )
//							{
//								mConn->SendPkg( socket.s, 0, Net_Event::EV_ENEMY_ATTACK_PLAYER, enemyAtk );
//							}
//						}
//					}
//				}
//				else
//				{
//					mEnemies[i]->SetState( HuntPlayer );
//					mEnemies[i]->SetHuntedPlayer( mPlayers[j].Position );
//				}
//			}
//			else
//			{
//				mEnemies[i]->SetState( Idle );
//			}
//		}
//	}
//}

void Server::ClientAttack( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Attack::GUID )
	{
		std::shared_ptr<Event_Client_Attack> data = std::static_pointer_cast<Event_Client_Attack>( eventPtr );
		UINT id = data->ID();
		UINT armID = data->ArmID();
		UINT anim = data->Animation();

		IEventPtr E1( new Event_Remote_Attack( id, armID, anim ) );
		BroadcastEvent( E1, id );
	}
}

void Server::ClientDown( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Down::GUID )
	{
		std::shared_ptr<Event_Client_Down> data = std::static_pointer_cast<Event_Client_Down>( eventPtr );
		UINT id = data->ID();

		IEventPtr E1( new Event_Remote_Down( id ) );
		BroadcastEvent( E1, id );
	}
}

void Server::ClientUp( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Up::GUID )
	{
		std::shared_ptr<Event_Client_Up> data = std::static_pointer_cast<Event_Client_Up>( eventPtr );
		UINT id = data->ID();

		IEventPtr E1( new Event_Remote_Up( id ) );
		BroadcastEvent( E1, id );
	}
}

void Server::ClientAttemptRevive( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Client_Attempt_Revive::GUID )
	{
		std::shared_ptr<Event_Client_Attempt_Revive> data = std::static_pointer_cast<Event_Client_Attempt_Revive>( eventPtr );
		UINT id = data->ID();
		UINT downedID = data->DownedID();
		float deltaTime = data->DeltaTime();

		if( mClientMap[id].TeamID == mClientMap[downedID].TeamID ) // This IF causes revives to be within teams, might be better to place somewhere else.
		{
			IEventPtr E1( new Event_Remote_Attempt_Revive( id, downedID, deltaTime ) );
			BroadcastEvent( E1, id );
		}
	}
}

// End of eventlistening functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is technically also an eventlistening function, but it's special so it can't be with the other ones
void Server::StartUp( IEventPtr eventPtr )
{
	if( eventPtr->GetEventType() == Event_Start_Server::GUID )
	{
		std::shared_ptr<Event_Start_Server> data = std::static_pointer_cast<Event_Start_Server>( eventPtr );
		std::string port = data->Port();

		// Makes sure everything is clean before starting
		Release();
		CreateShips();
		
		std::stringstream sstr;
		sstr << port << " ";
		UINT iPort;
		sstr >> iPort;

		if( Connect( iPort ) )
		{
			mActive = true;
			IEventPtr E1( new Event_Connect_Server_Success () );
			EventManager::GetInstance()->QueueEvent( E1 );
		}
		else
		{
			IEventPtr E1( new Event_Connect_Server_Fail ( "Server failed at connecting!" ) );
			EventManager::GetInstance()->QueueEvent( E1 );
			Release();
		}
	}
}

void Server::BroadcastEvent( IEventPtr eventPtr, UINT exception )
{
	for( auto& to : mClientMap )
	{
		if( to.first != exception )
		{
			to.second.NEF.ForwardEvent( eventPtr );
		}
	}
}

void Server::SendEvent( IEventPtr eventPtr, UINT to )
{
	mClientMap[to].NEF.ForwardEvent( eventPtr );
}

UINT Server::CurrentTeamDelegate()
{
	UINT currentTeam = mTeamDelegate;
	mTeamDelegate++;
	if( mTeamDelegate > MAX_TEAMS )
		mTeamDelegate = 1;
	return currentTeam;
}

UINT Server::CurrentPID()
{
	UINT currentPID = mCurrentPID;
	mCurrentPID++;
	if( mCurrentPID >= MAX_PROJECTILE_ID )
		mCurrentPID = 0;
	return currentPID;
}

void Server::CreateShips()
{
	UINT shipID = 60;
	float xOffset = -10.0f;
	for( UINT i = 0; i < MAX_TEAMS; i++ )
	{
		mShips[shipID].Initialize( shipID, CurrentTeamDelegate(), XMFLOAT3( xOffset, 0.0f, 0.0f ), XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
		shipID++;
		xOffset += 20.0f;
	}
}

bool Server::Connect( UINT port )
{
	mSocketManager = new SocketManager();
	if( !mSocketManager->Initialize() )
	{
		return false;
	}
	mSocketManager->AddSocket( new ServerListenSocket( mSocketManager, port ) );
	std::cout << "Server started on port: " << port << std::endl;

	return true;
}

void Server::Update( float deltaTime )
{
	if( mActive )
	{

	}
}

void Server::DoSelect( int pauseMicroSecs, bool handleInput )
{
	if( mActive )
	{
		mSocketManager->DoSelect( pauseMicroSecs, handleInput );
	}
}

bool Server::Initialize()
{
	EventManager::GetInstance()->AddListener( &Server::ClientJoined, this, Event_Client_Joined::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientLeft, this, Event_Client_Left::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientUpdate, this, Event_Client_Update::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientDied, this, Event_Client_Died::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientDamaged, this, Event_Client_Damaged::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientSpawned, this, Event_Client_Spawned::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientFiredProjectile, this, Event_Client_Fired_Projectile::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientUpdateHP, this, Event_Client_Update_HP::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientMeleeHit, this, Event_Client_Melee_Hit::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientAttack, this, Event_Client_Attack::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientDown, this, Event_Client_Down::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientUp, this, Event_Client_Up::GUID );
	EventManager::GetInstance()->AddListener( &Server::ClientAttemptRevive, this, Event_Client_Attempt_Revive::GUID );

	EventManager::GetInstance()->AddListener( &Server::StartUp, this, Event_Start_Server::GUID );

	mTeamDelegate	= 1;
	mCurrentPID		= 0;

	return true;
}

void Server::Release()
{
	if( mSocketManager )
		mSocketManager->Release();
	SAFE_DELETE( mSocketManager );
	mClientMap.clear();
	mTeamDelegate	= 1;
	mCurrentPID		= 0;
	mActive			= false;

	for( auto& s : mShips )
	{
		if( s.first )
			s.second.Release();
	}
}

Server::Server() : Network()
{
	mSocketManager	= nullptr;
	mClientMap		= std::map<UINT, ClientNEF>();
	mTeamDelegate	= (UINT)-1;
	mCurrentPID		= (UINT)-1;
	mActive			= false;
	mShips			= std::map<UINT, ServerShip>();
}

Server::~Server()
{
}