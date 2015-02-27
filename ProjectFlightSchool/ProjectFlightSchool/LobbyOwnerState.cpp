#include "LobbyOwnerState.h"

void LobbyOwnerState::HandleInput()
{
	if( mStartButton.LeftMousePressed() )
	{
		IEventPtr E1( new Event_Client_Lobby_Finished() );
		Client::GetInstance()->SendEvent( E1 );
	}
}

HRESULT LobbyOwnerState::Update( float deltaTime )
{
	HRESULT hr = LobbyState::Update( deltaTime );

	mStartButton.Update( deltaTime );
	
	HandleInput();

	return hr;
}

HRESULT LobbyOwnerState::Render()
{
	HRESULT hr = S_OK;
	
	RenderManager::GetInstance()->AddObject2dToList( mBackground, XMFLOAT2( 0.0f, 0.0f ), XMFLOAT2( (float)Input::GetInstance()->mScreenWidth, (float)Input::GetInstance()->mScreenHeight ) );
	
	mStartButton.Render();

	std::string textToWrite = "";

	for( auto p : mPlayers )
	{
		p->button.Render();
	}

	for( auto p : mPlayers )
	{
		textToWrite = p->name;
		
		mFont.WriteText( textToWrite, p->button.GetPosition().x + 20.0f, p->button.GetPosition().y + 15.0f, 3.0f );
	}
	
	RenderManager::GetInstance()->Render();
	
	return hr;
}

void LobbyOwnerState::OnExit()
{
	LobbyState::OnExit();
	mStartButton.SetExitCooldown();
}

HRESULT LobbyOwnerState::Initialize()
{
	mStartButton.Initialize( "../Content/Assets/Textures/Menu/Create_Menu_Text/MultiPlayer.png", 1665, 760, 200, 200 );

	return LobbyState::Initialize();
}

void LobbyOwnerState::Release()
{
	LobbyState::Release();
	mStartButton.Release();
}

LobbyOwnerState::LobbyOwnerState()
{
	LobbyState::LobbyState();
}

LobbyOwnerState::~LobbyOwnerState()
{

}