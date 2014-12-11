#include "RemotePlayer.h"

void RemotePlayer::RemoteUpdate( IEventPtr newEvent )
{
	if ( newEvent->GetEventType() == Event_Remote_Player_Update::GUID )
	{
		std::shared_ptr<Event_Remote_Player_Update>data	= std::static_pointer_cast<Event_Remote_Player_Update>( newEvent );
		if (mID != data->ID())
		{
			mLowerBody.position									= data->LowerBodyPos();
			mUpperBody.position									= data->UpperBodyPos();
		}
	}
}

void RemotePlayer::LookAt( float rotation )
{

}

HRESULT RemotePlayer::Render( float deltaTime )
{
	Graphics::GetInstance()->RenderStatic3dAsset( mUpperBody.playerModel, mUpperBody.position.x, mUpperBody.position.y, mUpperBody.position.z );
	Graphics::GetInstance()->RenderStatic3dAsset( mLowerBody.playerModel, mLowerBody.position.x, mLowerBody.position.y, mLowerBody.position.z );

	return S_OK;
}

HRESULT RemotePlayer::Initialize( unsigned int id )
{
	if ( FAILED( Graphics::GetInstance()->LoadStatic3dAsset( "CUBE", mUpperBody.playerModel) ) )
		OutputDebugString( L"\nERROR\n" );

	if ( FAILED( Graphics::GetInstance()->LoadStatic3dAsset( "CUBE", mLowerBody.playerModel ) ) )
		OutputDebugString( L"\nERROR\n" );

	mID					= id;
	mUpperBody.position = XMFLOAT3( 10.0f, 2.0f, 10.0f );
	mLowerBody.position = XMFLOAT3( 10.0f, 1.0f, 10.0f );
	mLowerBody.speed	= 0.005f;

	EventManager::GetInstance()->AddListener( &RemotePlayer::RemoteUpdate, this, Event_Remote_Player_Update::GUID );

	return S_OK;
}

void RemotePlayer::Release()
{}

RemotePlayer::RemotePlayer()
{
	mUpperBody.playerModel	= 0;
	mUpperBody.position		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	mUpperBody.direction	= XMFLOAT3(0.0f, 0.0f, 0.0f);

	mLowerBody.playerModel	= 0;
	mLowerBody.position		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	mLowerBody.direction	= XMFLOAT3(0.0f, 0.0f, 0.0f);
	mLowerBody.speed		= 0.0f;
}

RemotePlayer::~RemotePlayer()
{}