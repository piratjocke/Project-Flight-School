#ifndef CREATEMENUSTATE_H
#define CREATEMENUSTATE_H

#include "BaseMenuState.h"

class CreateMenuState : public BaseMenuState
{
	private:
		static const int
			SINGLEPLAYER	= 0,
			MULTIPLAYER		= 1,
			BACK			= 2,
			BUTTON_AMOUNT	= 3;

	protected:
	public:

	private:
		void	HandleInput();
	protected:
	public:
		HRESULT Update( float deltaTime );
		HRESULT Render();
		void	OnEnter();
		void	OnExit();
		void	Reset();
		HRESULT Initialize();
		void	Release();
				CreateMenuState();
		virtual ~CreateMenuState();
};
#endif