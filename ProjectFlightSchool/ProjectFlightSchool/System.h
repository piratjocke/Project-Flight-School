#ifndef SYSTEM_H
#define SYSTEM_H

#include "Server.h" // This include should always be first since it includes windows.h...... for now
#include "Graphics.h"
#include "Client.h"
#include <iostream>
#include "Game.h"


class System
{
	//Class members
	private:
		HINSTANCE	mHInstance;
		HWND		mHWnd;
		UINT		mScreenWidth;
		UINT		mScreenHeight;

		std::thread	mNetworkThread;
		Client		mClient;

		Game*		mGame;

	protected:
	public:

	//Class functions
	private:
		static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
		HRESULT					Update( float deltaTime );
		HRESULT					Render();
		void					NetworkInit();

	protected:
	public:
		int		Run();
		HRESULT Initialize( HINSTANCE hInstance, int nCmdShow );
		void	Release();
				System();
		virtual ~System();
};
#endif
