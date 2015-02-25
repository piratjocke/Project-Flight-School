#include "Enemy.h"

////////////////////////////////////////////////////////////////////
//					Steering Behavior Base
////////////////////////////////////////////////////////////////////

bool SteeringBehavior::Update(float deltaTime, XMFLOAT3& totalForce )
{
	return false;
}

void SteeringBehavior::SteerTowards( XMFLOAT3& target, XMFLOAT3& result )
{
	XMFLOAT3 desired;
	desired.x = target.x - mEnemy->GetPosition().x;
	desired.z = target.z - mEnemy->GetPosition().z;
	desired.y = 0.0f;

	// Check if this get the right values
	// targetDistance = desired.length()
	float targetDistance = XMVectorGetX( XMVector3Length( XMLoadFloat3( &desired ) ) ); 

	if( targetDistance > 0.0f )
	{
		// desired = desired.normalize * enemy->speed
		XMStoreFloat3( &desired, XMVector3Normalize( XMLoadFloat3( &desired ) ) * mEnemy->GetSpeed() );
		// result = desired - enemy->velocity
		result.x = desired.x - mEnemy->GetVelocity().x;
		result.z = desired.z - mEnemy->GetVelocity().z;
		result.y = 0.0f;
	}
	else
		XMStoreFloat3( &result, XMVectorZero() );
}

void SteeringBehavior::SteerAway( XMFLOAT3& target, XMFLOAT3& result )
{
	XMFLOAT3 desired;
	desired.x =  mEnemy->GetPosition().x - target.x;
	desired.z =  mEnemy->GetPosition().z - target.z;
	desired.y = 0.0f;

	// Check if this get the right values
	// targetDistance = desired.length()
	float targetDistance = XMVectorGetX( XMVector3Length( XMLoadFloat3( &desired ) ) ); 

	if( targetDistance > 0.0f )
	{
		// desired = desired.normalize * enemy->speed
		XMStoreFloat3( &desired, XMVector3Normalize( XMLoadFloat3( &desired ) ) * mEnemy->GetSpeed() );
		// result = desired - enemy->velocity
		result.x = desired.x - mEnemy->GetVelocity().x;
		result.z = desired.z - mEnemy->GetVelocity().z;
		result.y = 0.0f;
	}
	else
		XMStoreFloat3( &result, XMVectorZero() );
}

void SteeringBehavior::Reset()
{
}

void SteeringBehavior::Release()
{
}

SteeringBehavior::SteeringBehavior( Enemy* enemy )
{
	mEnemy			= enemy;
	mDisable		= false;
	mWeight			= 0.0f;
	mProbability	= 0.0f;
}

SteeringBehavior::~SteeringBehavior()
{

}

////////////////////////////////////////////////////////////////////
//						Steer Approach
////////////////////////////////////////////////////////////////////

bool SteerApproach::Update( float deltaTime, XMFLOAT3& totalForce )
{
	XMFLOAT3 steeringForce = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	SteerTowards( mEnemy->mPlayers[mEnemy->mTargetIndex]->Pos, steeringForce );
	totalForce.x	+= steeringForce.x;
	totalForce.z	+= steeringForce.z;
	totalForce.y	 = 0.0f;

	return true;
}

SteerApproach::SteerApproach( Enemy* enemy ) : SteeringBehavior( enemy ) { }

SteerApproach::~SteerApproach() { }

////////////////////////////////////////////////////////////////////
//						Steer Pursuit
////////////////////////////////////////////////////////////////////

bool SteerPursuit::Update( float deltatime, XMFLOAT3& totalForce )
{
	bool retVal		= false;
	bool found		= FindTarget();

	if( found )
	{
		XMFLOAT3 steeringForce = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		SteerTowards( mEnemy->mPlayers[mEnemy->mTargetID]->Pos, steeringForce );
		totalForce.x	+= steeringForce.x;
		totalForce.z	+= steeringForce.z;
		totalForce.y	 = 0.0f;

		retVal = true;
	}

	return retVal;
}

bool SteerPursuit::FindTarget()
{
	bool retVal		= false;

	// If the player is in the enemies path, just approach, 
	// otherwise try to predict player movement and head him off
	return retVal;
}

////////////////////////////////////////////////////////////////////
//						Steer Evade
////////////////////////////////////////////////////////////////////

bool SteerEvade::Update( float deltaTime, XMFLOAT3& totalForce )
{
	bool adjustment		= false;

	// Move away from the nearest object ( for now just other enemies )
	for ( size_t i = 0; i < MAX_NR_OF_ENEMIES; i++ )
	{
		if( mEnemy->GetAttackCircle()->Intersect( mEnemy->mOtherEnemies[i]->GetAttackCircle() ) )
		{
			XMFLOAT3 steeringForce = XMFLOAT3( 0.0f, 0.0f, 0.0f );
			SteerAway( mEnemy->mPlayers[mEnemy->mTargetIndex]->Pos, steeringForce );
			totalForce.x	+= steeringForce.x;
			totalForce.z	+= steeringForce.z;
			totalForce.y	 = 0.0f;
		}
	}

	return adjustment;
}

SteerEvade::SteerEvade( Enemy* enemy ) : SteeringBehavior( enemy ) {}

SteerEvade::~SteerEvade() { }

////////////////////////////////////////////////////////////////////
//						Steer Wander
////////////////////////////////////////////////////////////////////

bool SteerWander::Update( float deltaTime, XMFLOAT3& totalForce )
{
	bool adjustment			= false;
	XMFLOAT3 steeringForce	= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	float delta				= 0.15f;

	//theta represents "where" we are on the circle, perturbing
	//it is what causes the guy to wander
	//range on random is (-delta to delta)
	mThetaValue	= ( randflt() * 2 * delta ) - delta;

	// Calculate the point on the circle and head there
	mCirclePosition			= mEnemy->GetVelocity();
	XMStoreFloat3( &mCirclePosition, XMVector3Normalize( XMLoadFloat3( &mCirclePosition ) ) );		// mCirclePosition.Normalize()
	mCirclePosition.x      *= mWanderCircleDistance;													// mCirclePosition *= mWanderCircleDistance
	mCirclePosition.z      *= mWanderCircleDistance;
	mCirclePosition.y		= 0.0f;	
	mCirclePosition.x      += mEnemy->GetPosition().x;													//mCirclePosition  += mEnemy->GetPosition()
	mCirclePosition.z	   += mEnemy->GetPosition().z;
	mCirclePosition.y		= 0.0f;

	XMFLOAT3 circleTarget	= XMFLOAT3( mWanderCircleRadius * cos( mThetaValue ), 0.0f, mWanderCircleRadius * sin( mThetaValue ) );
	XMFLOAT3 target			= mCirclePosition;
	target.x				= circleTarget.x;
	target.z				= circleTarget.z;
	target.y				= 0.0f;

	SteerTowards( target, steeringForce );

	float distanceToTarget = XMVectorGetX( XMVector3Length( XMLoadFloat3( &steeringForce ) ) ); 
	if( distanceToTarget )
	{
		totalForce.x       += steeringForce.x;
		totalForce.z       += steeringForce.z;
		totalForce.y		= 0.0f;
		adjustment			= true;
		mTargetPosition.x	= target.x;
		mTargetPosition.z	= target.z;
		mTargetPosition.y	= 0.0f;
	}

	return adjustment;
}

SteerWander::SteerWander( Enemy* enemy ) : SteeringBehavior( enemy )
{
	mCirclePosition			= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mTargetPosition			= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mThetaValue				= 0.0f;
	mWanderCircleRadius		= 30.0f;
	mWanderCircleDistance	= 30.0f;
}

SteerWander::~SteerWander() { }