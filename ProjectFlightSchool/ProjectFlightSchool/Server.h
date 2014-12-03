#ifndef SERVER_H
#define SERVER_H
#include "Connection.h"
#include "Package.h"
#include <vector>

class Server
{
	private:
		int							mResult;
		addrinfo*					mAddrResult;
		SOCKET						mListenSocket;
		std::vector<SOCKET>			mClientSockets;
		Connection*					mConn;
		std::vector<std::thread>	mListenThreads;

	protected:

	public:

	private:
						Server();
		virtual			~Server();
		bool			AcceptConnection();
		bool			ReceiveLoop( int index );
		void			HandleMsg( SOCKET &socket, char* msg );

	protected:

	public:
		static Server*	GetInstance();
		bool			Connect();
		bool			Run();
		bool			Initialize( const char* port );
		void			Release();
};
#endif