#include "Projectile.h"

HRESULT Projectile::Update( float deltaTime )
{
	mPosition.x += mDirection.x * mSpeed * deltaTime;
	mPosition.z += mDirection.z * mSpeed * deltaTime;

	if( mLifeTime <= 0.0f )
		mIsActive = false;
	else
		mLifeTime -=deltaTime;

	return S_OK;
}

HRESULT Projectile::Render( float deltaTime )
{

	mRotation += 0.10f;
	//Graphics::GetInstance()->RenderStatic3dAsset( 1, mPosition.x, mPosition.y, mPosition.z );
	Graphics::GetInstance()->RenderStatic3dAsset( 1, mPosition, XMFLOAT3( 0.0f, mRotation, 0.0f ) );

	return S_OK;
}

void Projectile::SetDirection( XMFLOAT3 startPosition, XMFLOAT3 direction )
{
	mPosition		= startPosition;
	mDirection.x	= cosf( direction.y );
	mDirection.y	= 0.0f;
	mDirection.z	= -sinf( direction.y );
}

void Projectile::SetIsActive( bool isActive )
{
	mIsActive = isActive;
}

bool Projectile::IsActive() const
{
	return mIsActive;
}

HRESULT Projectile::Initialize()
{
	mSpeed		= 5.0f;
	mLifeTime	= 5.0f;
	return S_OK;
}

void Projectile::Release()
{

}

Projectile::Projectile()
{
	mPosition	= XMFLOAT3();
	mDirection	= XMFLOAT3();
	mSpeed		= 0.0f;
	mIsActive	= false;
	mRotation	= 0.0f;
	mLifeTime	= 0.0f;
}

Projectile::~Projectile()
{

}