#ifndef CLIENT_H
#define CLIENT_H

#include "Connection.h"
#include "Package.h"
#include "EventManager.h"
#include "Events.h"

class Client // The class used by clients to connect to the server
{
	// Members
	private:
		int				mResult;
		addrinfo*		mAddrResult;
		SOCKET			mServerSocket;
		Connection*		mConn;

	protected:

	public:

		// Template functions
	private:
		template <typename T>
		void HandlePkg( Package<T>* p );

	protected:
	public:
		
		// Functions
	private:
		bool	ReceiveLoop();
		void	PlayerMoved( IEventPtr newEvent );

	protected:

	public:
		bool	Connect();
		bool	Run();
		bool	Initialize( const char* port, const char* ip ); // Sets up and connects to the server
		void	Release();
				Client();
		virtual	~Client();
};

template <typename T>
void Client::HandlePkg( Package<T>* p )
{
	switch ( p->head.eventType )
	{
		case Net_Event::QUIT:
		{
			printf( "Disconnected from server.\n" );
			mConn->DisconnectSocket( mServerSocket );
		}
			break;
		case Net_Event::EV_PLAYER_MOVED:
		{
			//printf("Eventet fr�n servern var Event_Player_Moved och den inneh�ll positionerna:\n" ); // %f, %f, %f och %f, %f, %f
			EvPlayerMoved msg = (EvPlayerMoved&)p->body.content;
			IEventPtr E1( new Event_Remote_Player_Update( msg.lowerBody, msg.upperBody ) );
			EventManager::GetInstance()->QueueEvent( E1 );
		}
			break;
		case Net_Event::EV_PLAYER_JOINED:
		{
			EvPlayerJoined msg = (EvPlayerJoined&)p->body.content;
			IEventPtr E1( new Event_Remote_Player_Joined( msg.ID ) );
			EventManager::GetInstance()->QueueEvent( E1 );
		}
			break;
		default:
		{
		}
			break;
	}
}
#endif
