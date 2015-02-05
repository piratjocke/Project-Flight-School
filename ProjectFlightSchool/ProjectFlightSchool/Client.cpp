#include "Client.h"

bool Client::ReceiveLoop()
{
	Package<void*>* p = new Package<void*>[DEFAULT_BUFLEN]; // This will only be called once per Client instance
	while ( mServerSocket != INVALID_SOCKET )
	{
		if ( mConn->ReceivePkg( mServerSocket, *p ) )
		{
			if ( p->head.eventType != Net_Event::ERROR_EVENT )
			{
				HandlePkg( p );
			}
		}
	}
	if ( p )
	{
		delete[] p;
	}
	return true;
}

void Client::EventListener( IEventPtr newEvent )
{
	if ( newEvent->GetEventType() == Event_Player_Update::GUID )
	{
		std::shared_ptr<Event_Player_Update> data = std::static_pointer_cast<Event_Player_Update>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvPlayerUpdate msg;
			msg.id						= mID;
			msg.lowerBodyPosition		= data->LowerBodyPos();
			msg.velocity				= data->Velocity();
			msg.upperBodyDirection		= data->UpperBodyDirection();
			msg.playerName				= data->Name();
			
			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PLAYER_UPDATE, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Player_Died::GUID )
	{
		std::shared_ptr<Event_Player_Died> data = std::static_pointer_cast<Event_Player_Died>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvKilled msg;
			msg.ID			= data->ID();
			msg.killerID	= data->KillerID();

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PLAYER_DIED, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Player_Damaged::GUID )
	{
		std::shared_ptr<Event_Player_Damaged> data = std::static_pointer_cast<Event_Player_Damaged>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvPlayerID msg;
			msg.ID = data->ID();
			msg.projectileID = data->ProjectileID();

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PLAYER_DAMAGED, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Player_Spawned::GUID )
	{
		std::shared_ptr<Event_Player_Spawned> data = std::static_pointer_cast<Event_Player_Spawned>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvPlayerID msg;
			msg.ID = data->ID();

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PLAYER_SPAWNED, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Projectile_Fired::GUID )
	{
		std::shared_ptr<Event_Projectile_Fired> data = std::static_pointer_cast<Event_Projectile_Fired>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvProjectileFired msg;
			msg.ID			= mID;
			msg.position	= data->BodyPos();
			msg.direction	= data->Direction();

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PROJECTILE_FIRED, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Player_Update_HP::GUID )
	{
		std::shared_ptr<Event_Player_Update_HP> data = std::static_pointer_cast<Event_Player_Update_HP>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvPlayerID msg;
			msg.ID	= mID;
			msg.HP	= (unsigned int)data->HP();

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_UPDATE_HP, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Player_Melee_Hit::GUID )
	{
		std::shared_ptr<Event_Player_Melee_Hit> data = std::static_pointer_cast<Event_Player_Melee_Hit>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvMeleeHit msg;
			msg.ID			= data->ID();
			msg.damage		= data->Damage();
			msg.knockBack	= data->KnockBack();
			msg.direction	= data->Direction();
			

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_MELEE_HIT, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Player_Attack::GUID )
	{
		std::shared_ptr<Event_Player_Attack> data = std::static_pointer_cast<Event_Player_Attack>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvPlayerAttack msg;
			msg.ID			= mID;
			msg.armID		= data->ArmID();
			msg.animation	= data->Animation();		

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PLAYER_ATTACK, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Projectile_Damage_Enemy::GUID )
	{
		std::shared_ptr<Event_Projectile_Damage_Enemy> data = std::static_pointer_cast<Event_Projectile_Damage_Enemy>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvEnemyProjectileDamage msg;
			msg.shooterID		= data->Shooter();	
			msg.projectileID	= data->Projectile();
			msg.enemyID			= data->Enemy();
			msg.damage			= data->Damage();

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_ENEMY_PROJECTILE_DAMGAE, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Player_Down::GUID )
	{
		std::shared_ptr<Event_Player_Down> data = std::static_pointer_cast<Event_Player_Down>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvPlayerID msg;
			msg.ID = data->Player();

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PLAYER_DOWN, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Player_Up::GUID )
	{
		std::shared_ptr<Event_Player_Up> data = std::static_pointer_cast<Event_Player_Up>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvPlayerID msg;
			msg.ID = data->Player();

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PLAYER_UP, msg );
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Remote_Player_Revive::GUID )
	{
		std::shared_ptr<Event_Remote_Player_Revive> data = std::static_pointer_cast<Event_Remote_Player_Revive>( newEvent );
		if ( mServerSocket != INVALID_SOCKET )
		{
			EvIDAndTime msg;
			msg.playerID	= data->Player();
			msg.deltaTime	= data->DeltaTime();

			if ( mServerSocket != INVALID_SOCKET )
			{
				mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PLAYER_REVIVE, msg );
			}
		}
	}
}

bool Client::Connect()
{
	for ( addrinfo* ptr = mAddrResult; ptr != nullptr; ptr = ptr->ai_next )
	{
		mServerSocket = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
		if ( mServerSocket == INVALID_SOCKET )
		{
			printf( "socket failed with error: %d\n", WSAGetLastError() );
			WSACleanup();
			return false;
		}

		mResult = connect( mServerSocket, ptr->ai_addr, (int)ptr->ai_addrlen );
		if ( mResult == SOCKET_ERROR )
		{
			closesocket( mServerSocket );
			mServerSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo( mAddrResult );

	if ( mServerSocket == INVALID_SOCKET )
	{
		printf( "Unable to connect to server.\n" );
		closesocket( mServerSocket );
		WSACleanup();
		return false;
	}

	int flag = 1;
    int mResult = setsockopt( mServerSocket,            /* socket affected */
                                 IPPROTO_TCP,     /* set option at TCP level */
                                 TCP_NODELAY,     /* name of option */
                                 (char *) &flag,  /* the cast is historical
                                                         cruft */
                                 sizeof(int) );    /* length of option value */

	if( mResult != 0 )
	{
		printf( "setsockopt failed with error %d.\n", WSAGetLastError() );
		mConn->DisconnectSocket( mServerSocket );
		WSACleanup();
		return false;
	}

	printf( "Connected to: %d\n", mServerSocket );

	return true;
}

bool Client::Run()
{
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Player_Update::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Player_Died::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Player_Damaged::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Player_Spawned::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Projectile_Fired::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Player_Melee_Hit::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Player_Attack::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Player_Update_HP::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Projectile_Damage_Enemy::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Player_Down::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Player_Up::GUID );
	EventManager::GetInstance()->AddListener( &Client::EventListener, this, Event_Remote_Player_Revive::GUID );

	std::thread listen( &Client::ReceiveLoop, this );

	mConn->SendPkg( mServerSocket, 0, Net_Event::EV_PLAYER_JOINED, 0 ); // The client "announces" itself to the server, and by extension, the other clients

	listen.join();

	return true;
}

bool Client::Initialize( std::string ip, std::string port )
{
	WSADATA WSAData = WSADATA();
	mResult			= WSAStartup( MAKEWORD( 2, 2 ), &WSAData );
	if ( mResult != 0 )
	{
		printf( "WSAStartup failed with error: %d\n", mResult );
		return false;
	}

	addrinfo hints = { 0 };
	ZeroMemory( &hints, sizeof( hints ) );
	hints.ai_family		= AF_INET;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= IPPROTO_TCP;

	mResult = getaddrinfo( ip.c_str(), port.c_str(), &hints, &mAddrResult );
	if ( mResult != 0 )
	{
		printf( "getaddrinfo failed with error: %d\n", mResult );
		WSACleanup();
		return false;
	}

	mConn = new Connection();
	mConn->Initialize();

	return true;
}

void Client::Release()
{
	mConn->DisconnectSocket( mServerSocket );

	WSACleanup();

	mConn->Release();
	if ( mConn )
		delete mConn;
}

Client::Client()
{
	mResult			= 0;
	mAddrResult		= nullptr;
	mServerSocket	= INVALID_SOCKET;
	mConn			= nullptr;
}

Client::~Client()
{
}