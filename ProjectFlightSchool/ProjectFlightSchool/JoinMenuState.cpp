#include "JoinMenuState.h"

void JoinMenuState::HandleInput()
{
	if( mButtons.at(BACK)->LeftMousePressed() )
	{
		IEventPtr E1( new Event_Change_State( START_MENU_STATE ) );
		EventManager::GetInstance()->QueueEvent( E1 );
	}
	else if( mButtons.at(JOIN)->LeftMousePressed() )
	{
		std::string ip			= mIPBox.GetText();
		std::string port		= mPortBox.GetText();
		 
		IEventPtr E1( new Event_Start_Client( ip, port ) );
		EventManager::GetInstance()->QueueEvent( E1 );
		IEventPtr E2( new Event_Create_Player_Name( mNameBox.GetText() ) );
		EventManager::GetInstance()->QueueEvent( E2 );
	}
	else if( mButtons.at(MIKAEL)->LeftMousePressed() )
	{
		std::string ip		= "192.168.1.88";
		std::string port	= mPortBox.GetText();

		IEventPtr E1( new Event_Start_Client( ip, port ) );
		EventManager::GetInstance()->QueueEvent( E1 );
	}
	else if( mIPBox.LeftMousePressed() )
	{
		mIPBox.SwitchActive( true );
		mPortBox.SwitchActive( false );
		mNameBox.SwitchActive( false );
	}
	else if( mPortBox.LeftMousePressed() )
	{
		mIPBox.SwitchActive( false );
		mPortBox.SwitchActive( true );
		mNameBox.SwitchActive( false );
	}
	else if( mNameBox.LeftMousePressed() )
	{
		mIPBox.SwitchActive( false );
		mPortBox.SwitchActive( false );
		mNameBox.SwitchActive( true );
	}
}

HRESULT JoinMenuState::Update( float deltaTime )
{
	BaseMenuState::Update( deltaTime );
	for( int i = 0; i < BUTTON_AMOUNT; i++ )
	{
		mButtons.at(i)->Update( deltaTime );
	}
	mIPBox.Update( deltaTime );
	mPortBox.Update( deltaTime );
	mNameBox.Update( deltaTime );

	return S_OK;
}

HRESULT JoinMenuState::Render()
{
	BaseMenuState::Render();
	for( int i = 0; i < BUTTON_AMOUNT; i++ )
	{
		mButtons.at(i)->Render();
	}
	mIPBox.Render();
	mPortBox.Render();
	mNameBox.Render();

	RenderManager::GetInstance()->Render();
	return S_OK;
}

void JoinMenuState::OnEnter()
{
}

void JoinMenuState::OnExit()
{
	BaseMenuState::OnExit();
}

void JoinMenuState::Reset()
{
	mIPBox.Release();
	mPortBox.Release();

	float x = (float)Input::GetInstance()->mScreenWidth  * 0.10f;
	float y = (float)Input::GetInstance()->mScreenHeight * 0.80f;
	float w = 640.0f/2;
	float h = 177.0f/2;

	mIPBox.Initialize( "localhost", "IP", x, y, w, h );

	x += w + 20;

	mPortBox.Initialize( "27015", "Port", x, y, w, h );

	mNameBox.Initialize( "mudkipfucker", "IP", Input::GetInstance()->mScreenWidth * 0.5f - (640.0f * 0.5f) * 0.5f, Input::GetInstance()->mScreenHeight * 0.5f + (177.0f * 0.5f) *0.5f, 640.0f * 0.5f, 177.0f * 0.5f );
}

HRESULT JoinMenuState::Initialize()
{
	BaseMenuState::Initialize();
	mStateType = JOIN_MENU_STATE;
	mButtons.reserve( BUTTON_AMOUNT );

	float x	= (float)Input::GetInstance()->mScreenWidth  * 0.65f;
	float y	= (float)Input::GetInstance()->mScreenHeight * 0.75f;
	float w	= 200.0f;
	float h	= 200.0f;

	mButtons.push_back( new Button() );
	mButtons.at(JOIN)->Initialize( "../Content/Assets/Textures/Menu/Join_Menu_Text/Join.png", x, y, w, h );

	x += w + 20;

	mButtons.push_back( new Button() );
	mButtons.at(BACK)->Initialize( "../Content/Assets/Textures/Menu/Back.png", x, y, w, h );

	////// MIKAEL JOINBUTTON
	x -= ( w + 20 );
	y -= ( h + 20 );

	mButtons.push_back( new Button() );
	mButtons.at(MIKAEL)->Initialize( "../Content/Assets/Textures/Menu/Join_Menu_Text/Join.png", x, y, w, h ); 


	x = (float)Input::GetInstance()->mScreenWidth  * 0.10f;
	y = (float)Input::GetInstance()->mScreenHeight * 0.80f;
	w = 640.0f/2;
	h = 177.0f/2;

	mIPBox.Initialize( "localhost", "IP", x, y, w, h );

	x += w + 20;

	mPortBox.Initialize( "27015", "Port", x, y, w, h );

	mNameBox.Initialize( "mudkipfucker", "IP", Input::GetInstance()->mScreenWidth * 0.5f - (640.0f * 0.5f) * 0.5f, Input::GetInstance()->mScreenHeight * 0.5f + (177.0f * 0.5f) *0.5f, 640.0f * 0.5f, 177.0f * 0.5f );

	return S_OK;
}

void JoinMenuState::Release()
{
	BaseMenuState::Release();
	mIPBox.Release();
	mPortBox.Release();
	mNameBox.Release();
}

JoinMenuState::JoinMenuState() : BaseMenuState()
{
}

JoinMenuState::~JoinMenuState()
{
}
