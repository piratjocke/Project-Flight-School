#include "Player.h"

/////Private

void Player::EventListener( IEventPtr newEvent )
{
	if ( newEvent->GetEventType() == Event_Remote_Died::GUID )
	{
		// Kill remote player
		std::shared_ptr<Event_Remote_Died> data = std::static_pointer_cast<Event_Remote_Died>( newEvent );
		if ( data->KillerID() == mID )
		{
			CountUpKills();
		}
	}
	else if ( newEvent->GetEventType() == Event_Remote_Attempt_Revive::GUID )
	{
		std::shared_ptr<Event_Remote_Attempt_Revive> data = std::static_pointer_cast<Event_Remote_Attempt_Revive>( newEvent) ;
		HandleRevive( data->DeltaTime() );
	}
	else if ( newEvent->GetEventType() == Event_Server_Enemy_Attack_Player::GUID )
	{
		std::shared_ptr<Event_Server_Enemy_Attack_Player> data = std::static_pointer_cast<Event_Server_Enemy_Attack_Player>( newEvent );
		if ( mID == data->PlayerID() )
			TakeDamage( data->Damage(), 0);
	}
	else if ( newEvent->GetEventType() == Event_Remote_Melee_Hit::GUID )
	{
		// Melee Hit
		std::shared_ptr<Event_Remote_Melee_Hit> data = std::static_pointer_cast<Event_Remote_Melee_Hit>( newEvent );
		if ( mID == data->ID() )
		{
			XMFLOAT3 direction = data->Direction();
			direction.x *= data->KnockBack();
			direction.z *= data->KnockBack();
			AddImpuls( direction );
			TakeDamage( data->Damage(), 0);
		}
	}
	else if ( newEvent->GetEventType() == Event_Create_Player_Name::GUID )
	{
		std::shared_ptr<Event_Create_Player_Name> data = std::static_pointer_cast<Event_Create_Player_Name>( newEvent );
		mPlayerName = data->PlayerName();
	}
	else if( newEvent->GetEventType() == Event_Server_Change_Buff_State::GUID )
	{
		std::shared_ptr<Event_Server_Change_Buff_State> data = std::static_pointer_cast<Event_Server_Change_Buff_State>( newEvent );
		if( data->ID() == mID )
		{
			mIsBuffed	= data->IsBuffed();
			mBuffMod	= data->BuffMod();
			if( mIsBuffed )
				printf("%d is buffed!\n", mID);
			else
				printf("%d is no longer buffed!\n", mID);
		}
	}
}

void Player::HandleInput( float deltaTime, std::vector<RemotePlayer*> remotePlayers )
{
	if( Input::GetInstance()->IsKeyDown(KEYS::KEYS_SPACE) && mCloseToPlayer )
	{
		mAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for( auto rp : remotePlayers )
		{
			if ( rp->IsDown() )
			{
				if ( mBoundingCircleAura->Intersect( rp->GetBoundingCircleAura() ) )
				{
					ReviveRemotePlayer( rp->GetID(), deltaTime );
				}
			}
		}
	}
	else
	{
		mAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (Input::GetInstance()->IsKeyDown(KEYS::KEYS_W) && !Input::GetInstance()->IsKeyDown(KEYS::KEYS_S))
			mAcceleration.z = mMaxAcceleration;

		if (Input::GetInstance()->IsKeyDown(KEYS::KEYS_A) && !Input::GetInstance()->IsKeyDown(KEYS::KEYS_D))
			mAcceleration.x = -mMaxAcceleration;

		if (Input::GetInstance()->IsKeyDown(KEYS::KEYS_S) && !Input::GetInstance()->IsKeyDown(KEYS::KEYS_W))
			mAcceleration.z = -mMaxAcceleration;

		if (Input::GetInstance()->IsKeyDown(KEYS::KEYS_D) && !Input::GetInstance()->IsKeyDown(KEYS::KEYS_A))
			mAcceleration.x = mMaxAcceleration;


		//Normalize acceleration 
		XMVECTOR normalizer = XMVector3Length(XMLoadFloat3(&mAcceleration));
		if (XMVectorGetX(normalizer) > mMaxAcceleration)
		{
			normalizer = XMVector3Normalize(XMLoadFloat3(&mAcceleration));
			normalizer *= mMaxAcceleration;
			XMStoreFloat3(&mAcceleration, normalizer);
		}

		//== Calculate upper body rotation ==
		XMVECTOR rayOrigin = XMVECTOR(Input::GetInstance()->mCurrentNDCMousePos);
		XMVECTOR rayDir = rayOrigin;

		XMFLOAT3 unPack;
		XMStoreFloat3(&unPack, rayOrigin);
		rayDir = XMVectorSet(unPack.x, unPack.y, 1.0f, 1.0f);

		XMMATRIX viewInverse;
		Graphics::GetInstance()->GetInverseViewMatrix(viewInverse);

		XMMATRIX projectionInverse;
		Graphics::GetInstance()->GetInverseProjectionMatrix(projectionInverse);

		XMMATRIX combinedInverse = XMMatrixMultiply(projectionInverse, viewInverse);

		XMVECTOR rayPosInWorld = XMVector3TransformCoord(rayOrigin, combinedInverse);
		XMVECTOR rayDirInWorld = XMVector3TransformCoord(rayDir, combinedInverse);
		rayDirInWorld = XMVector3Normalize(rayDirInWorld - rayPosInWorld);

		float t = 0.0f;
		XMVECTOR planeNormal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR result = -(XMVector3Dot(rayPosInWorld, planeNormal)) / (XMVector3Dot(rayDirInWorld, planeNormal));
		XMStoreFloat(&t, result);
		XMVECTOR intersection = XMVectorAdd(rayPosInWorld, rayDirInWorld * t);



	//== Weapon handling ==
	if( Input::GetInstance()->IsKeyDown( KEYS::KEYS_MOUSE_LEFT ) && mWeaponCoolDown <= 0.0f )
	{
		Fire();
		mWeaponCoolDown = mLoadOut->rangedWeapon->attackRate;
		
		RenderManager::GetInstance()->AnimationStartNew( mArms.rightArm, mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][ATTACK] );
		mRightArmAnimationCompleted		= false;
		IEventPtr E1( new Event_Client_Attack( mID, RIGHT_ARM_ID, mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][ATTACK] ) );
		QueueEvent( E1 );
	}
	else
		mWeaponCoolDown -= deltaTime;

		XMVECTOR playerToCursor = XMVectorSubtract(intersection, XMLoadFloat3(&XMFLOAT3(mLowerBody.position.x, 1.0f, mLowerBody.position.z)));
		XMStoreFloat3(&unPack, playerToCursor);
		playerToCursor = XMVector3Normalize(XMVectorSet(unPack.x, 0.0f, unPack.z, 0.0f));
		XMStoreFloat3(&mUpperBody.direction, playerToCursor);


		//== Weapon handling ==
		if (Input::GetInstance()->IsKeyDown(KEYS::KEYS_MOUSE_LEFT) && mWeaponCoolDown <= 0.0f)
		{
			Fire();
			mWeaponCoolDown = 0.1f;

			RenderManager::GetInstance()->AnimationStartNew(mArms.rightArm, mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][ATTACK]);
			mRightArmAnimationCompleted = false;
			IEventPtr E1( new Event_Client_Attack(mID, RIGHT_ARM_ID, mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][ATTACK]) );
			QueueEvent( E1 );
		}
		else
			mWeaponCoolDown -= deltaTime;

		if (Input::GetInstance()->IsKeyDown(KEYS::KEYS_MOUSE_RIGHT) && mMeleeCoolDown <= 0.0f)
		{
			RenderManager::GetInstance()->AnimationStartNew(mArms.leftArm, mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][ATTACK]);
			mLeftArmAnimationCompleted = false;
			IEventPtr E1( new Event_Client_Attack(mID, LEFT_ARM_ID, mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][ATTACK]) );
			QueueEvent( E1 );
			mHasMeleeStarted = true;
			mMeleeCoolDown = mLoadOut->meleeWeapon->attackRate;
		}
		else
			mMeleeCoolDown -= deltaTime;
	}
}

void Player::HandleSpawn( float deltaTime )
{
	if( mTimeTillSpawn <= 0.0f )
	{
		UnLock();
		Spawn();
		IEventPtr E1( new Event_Client_Spawned( mID ) );
		QueueEvent( E1 );
	}
	else
	{
		mTimeTillSpawn -= deltaTime;
	}
}

void Player::HandleDeath( float deltaTime )
{
	if( mTimeTillDeath <= 0.0f )
	{
		Lock();
		Die();
		BroadcastDeath( mLastKiller );
	}
	else
	{
		mTimeTillDeath -= deltaTime;
	}
}

void Player::Move( float deltaTime )
{
	mVelocity.x += mAcceleration.x * deltaTime - mVelocity.x * VELOCITY_FALLOFF * deltaTime;
	mVelocity.y = 0.0f;
	mVelocity.z += mAcceleration.z * deltaTime - mVelocity.z * VELOCITY_FALLOFF * deltaTime;

	XMVECTOR normalizer = XMVector3Length( XMLoadFloat3( &mVelocity ) );
	mCurrentVelocity = XMVectorGetX( normalizer );
	if(  mCurrentVelocity > mMaxVelocity )
	{
		normalizer	 = XMVector3Normalize( XMLoadFloat3( &mVelocity ) );
		normalizer	*= mMaxVelocity;
		XMStoreFloat3( &mVelocity, normalizer );
	}

	mLowerBody.direction.x	= mVelocity.x;
	mLowerBody.direction.y	= 0.0f;
	mLowerBody.direction.z	= mVelocity.z;
	normalizer				= XMVector3Normalize( XMLoadFloat3( &mLowerBody.direction ) );
	XMStoreFloat3( &mLowerBody.direction, normalizer );

	mLowerBody.position.x += mVelocity.x * deltaTime;
	mLowerBody.position.z += mVelocity.z * deltaTime;
}

void Player::GoDown( int shooter )
{
	Lock();
	RemotePlayer::GoDown();
	mTimeTillRevive	= mReviveTime;
	mLastKiller		= shooter;
	IEventPtr E1( new Event_Client_Down( mID ) );
	QueueEvent( E1 );
}

void Player::GoUp()
{
	UnLock();
	RemotePlayer::GoUp();
	IEventPtr E1( new Event_Client_Up( mID ) );
	QueueEvent( E1 );
}

void Player::ReviveRemotePlayer( int remotePlayerID, float deltaTime )
{
	IEventPtr E1( new Event_Client_Attempt_Revive( mID, remotePlayerID, deltaTime ) );
	QueueEvent( E1 );
}

void Player::BroadcastDeath( unsigned int shooter )
{
	IEventPtr E1( new Event_Client_Died( mID, shooter ) );
	QueueEvent( E1 );
}

void Player::Revive()
{
	GoUp();
}

void Player::Die()
{
	RemotePlayer::Die();
	mTimeTillSpawn	= mSpawnTime;
}

void Player::Fire()
{
	//Offset calcs
	XMFLOAT3 weaponOffsets =	mLoadOut->rangedWeapon->weaponType == MINIGUN			? MINIGUN_OFFSETS : 
								mLoadOut->rangedWeapon->weaponType == SHOTGUN			? SHOTGUN_OFFSETS :
								mLoadOut->rangedWeapon->weaponType == GRENADELAUNCHER	? GL_OFFSETS :
								mLoadOut->rangedWeapon->weaponType == SNIPER			? SNIPER_OFFSETS : XMFLOAT3( 0.0f, 0.0f, 1.0f );

	XMVECTOR position	= XMLoadFloat3( &mLowerBody.position );
	XMVECTOR direction	= XMLoadFloat3( &mUpperBody.direction );
	XMVECTOR offset		= XMLoadFloat3( &XMFLOAT3( mLowerBody.position.x, weaponOffsets.z, mLowerBody.position.z ) );
	
	offset += XMVector3Normalize( XMVector3Cross( XMLoadFloat3( &XMFLOAT3( 0.0f, 1.0f, 0.0f ) ), direction ) ) * weaponOffsets.y;
	offset += direction * weaponOffsets.x;

	XMFLOAT3 loadDir;
	XMStoreFloat3( &loadDir, offset );

	if( mLoadOut->rangedWeapon->weaponType == SHOTGUN )
	{
		FireShotgun( &loadDir );
	}
	else
	{
		// Set random spread
		if( mLoadOut->rangedWeapon->spread != 0.0f )
		{
			float directionOffset	=  (float)( rand() % 100 ) * 0.001f - mLoadOut->rangedWeapon->spread;
			mFireDirection			= XMFLOAT3( mUpperBody.direction.x + directionOffset, mUpperBody.direction.y, mUpperBody.direction.z + directionOffset );
			QueueEvent( (IEventPtr)new Event_Client_Fired_Projectile( mID, loadDir, mFireDirection, mLoadOut->rangedWeapon->GetRandomProjectileSpeed(), mLoadOut->rangedWeapon->range ) );
		}
		else
		{
			QueueEvent(  (IEventPtr)new Event_Client_Fired_Projectile( mID, loadDir, mUpperBody.direction, mLoadOut->rangedWeapon->GetRandomProjectileSpeed(), mLoadOut->rangedWeapon->range ) );
		}
	}
}

void Player::FireShotgun( XMFLOAT3* spawnPoint )
{
	// Fire shotgun

	// middle projectile
	QueueEvent( (IEventPtr)new Event_Client_Fired_Projectile( mID, *spawnPoint, mUpperBody.direction, mLoadOut->rangedWeapon->projectileSpeed, mLoadOut->rangedWeapon->range ) );

	//// projectile 1
	XMFLOAT3 shotDir = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	XMStoreFloat3( &shotDir, XMVector3TransformNormal( XMLoadFloat3( &mUpperBody.direction ), XMMatrixRotationY( XMConvertToRadians( 30.0f ) ) ) );
	QueueEvent( (IEventPtr)new Event_Client_Fired_Projectile( mID, *spawnPoint, shotDir, mLoadOut->rangedWeapon->projectileSpeed, mLoadOut->rangedWeapon->range ) );

	// projectile 2
	//shotDir = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	XMStoreFloat3( &shotDir, XMVector3TransformNormal( XMLoadFloat3( &mUpperBody.direction ), XMMatrixRotationY( XMConvertToRadians( -30.0f ) ) ) );
	QueueEvent( (IEventPtr)new Event_Client_Fired_Projectile( mID, *spawnPoint, shotDir, mLoadOut->rangedWeapon->projectileSpeed, mLoadOut->rangedWeapon->range ) );

	XMStoreFloat3( &shotDir, XMVector3TransformNormal( XMLoadFloat3( &mUpperBody.direction ), XMMatrixRotationY( XMConvertToRadians( 20.0f ) ) ) );
	QueueEvent( (IEventPtr)new Event_Client_Fired_Projectile( mID, *spawnPoint, shotDir, mLoadOut->rangedWeapon->projectileSpeed, mLoadOut->rangedWeapon->range ) );

	// projectile 2
	//shotDir = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	XMStoreFloat3( &shotDir, XMVector3TransformNormal( XMLoadFloat3( &mUpperBody.direction ), XMMatrixRotationY( XMConvertToRadians( -20.0f ) ) ) );
	QueueEvent( (IEventPtr)new Event_Client_Fired_Projectile( mID, *spawnPoint, shotDir, mLoadOut->rangedWeapon->projectileSpeed, mLoadOut->rangedWeapon->range ) );

	XMStoreFloat3( &shotDir, XMVector3TransformNormal( XMLoadFloat3( &mUpperBody.direction ), XMMatrixRotationY( XMConvertToRadians( 40.0f ) ) ) );
	QueueEvent( (IEventPtr)new Event_Client_Fired_Projectile( mID, *spawnPoint, shotDir, mLoadOut->rangedWeapon->projectileSpeed, mLoadOut->rangedWeapon->range ) );

	// projectile 2
	//shotDir = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	XMStoreFloat3( &shotDir, XMVector3TransformNormal( XMLoadFloat3( &mUpperBody.direction ), XMMatrixRotationY( XMConvertToRadians( -40.0f ) ) ) );
	QueueEvent( (IEventPtr)new Event_Client_Fired_Projectile( mID, *spawnPoint, shotDir, mLoadOut->rangedWeapon->projectileSpeed, mLoadOut->rangedWeapon->range ) );
}

void Player::AddImpuls( XMFLOAT3 impuls )
{
	if (!mLock)
	{
		mVelocity.x += impuls.x;
		mVelocity.z += impuls.z;
	}
}

void Player::Lock()
{
	mVelocity = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mLock = true;
}

void Player::UnLock()
{
	mLock = false;
}

void Player::QueueEvent( IEventPtr ptr )
{
	gEventList.push_front( ptr );
}

/////Public

void Player::TakeDamage( float damage, unsigned int shooter )
{
	if( mIsBuffed )
	{
		float moddedDmg = damage * mBuffMod;
		damage -= moddedDmg;
	}
	mCurrentHp -= damage;
	IEventPtr E1( new Event_Client_Update_HP( mID, mCurrentHp ) );
	QueueEvent( E1 );
	if ( !mIsDown && mIsAlive && mCurrentHp <= 0.0f )
	{
		GoDown( shooter );
	}
}

void Player::HandleRevive(float deltaTime)
{
	if (mTimeTillRevive <= 0.0f)
	{
		GoUp();
	}
	else
	{
		mTimeTillRevive -= deltaTime;
	}
}

void Player::Reset()
{
	mEventCapTimer					= 0.0f;
	mPointLight->position		= DirectX::XMFLOAT4( mLowerBody.position.x, mLowerBody.position.y, mLowerBody.position.z, 0.0f );
	mPointLight->colorAndRadius	= DirectX::XMFLOAT4( 0.8f, 0.8f, 0.8f, 17.0f );

	mWeaponCoolDown				= 0;
	mMeleeCoolDown				= 0;
	mTimeTillattack				= mLoadOut->meleeWeapon->timeTillAttack;
	mIsMeleeing					= false;
	mHasMeleeStarted			= false;
	mLock						= false;
	mCloseToPlayer				= false;

	mMaxVelocity				= 7.7f;
	mVelocity					= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mCurrentVelocity			= 0.0f;
	mMaxAcceleration			= 20.0f;;
	mAcceleration				= XMFLOAT3( 0.0f, 0.0f, 0.0f );

	mIsBuffed					= false;
	mBuffMod					= 1; // Modifies the damage a player takes by a percentage, should only range between 0 and 1

	mTimeTillSpawn				= mSpawnTime;
	mTimeTillDeath				= mDeathTime;
	mTimeTillRevive				= mReviveTime;
	mLastKiller					= 0;

	mLowerBody.position		= XMFLOAT3( 3.0f, 0.0f, 0.0f );
	
	mIsAlive				= true;
	mIsDown					= false;
	mMaxHp					= 100.0f;
	mCurrentHp				= mMaxHp;
	mNrOfDeaths				= 0;
	mNrOfKills				= 0;
	mID						= -1;
	mTeam					= -1;

	mLeftArmAnimationCompleted	= false;
	mRightArmAnimationCompleted	= false;

	mUpperBody.direction	= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mLowerBody.direction	= XMFLOAT3( 0.0f, 0.0f, 0.0f );

	RenderManager::GetInstance()->AnimationReset( mLowerBody.playerModel, mAnimations[PLAYER_ANIMATION::LEGS_IDLE] );
	RenderManager::GetInstance()->AnimationReset( mArms.leftArm, mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][WEAPON_ANIMATION::IDLE] );
	RenderManager::GetInstance()->AnimationReset( mArms.rightArm, mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][WEAPON_ANIMATION::IDLE] );
}

HRESULT Player::Update( float deltaTime, std::vector<RemotePlayer*> remotePlayers )
{
	mCloseToPlayer = false;
	for( auto rp : remotePlayers )
	{
		if( mBoundingCircle->Intersect( rp->GetBoundingCircleAura() ) )
		{
			mCloseToPlayer = true;
		}
	}

	//Temp visual aid to know that you're close to another player. Used to know if you can revive.
	if( mCloseToPlayer )
	{
		mPointLight->colorAndRadius = DirectX::XMFLOAT4( 0.8f, 0.6f, 0.8f, 17.0f );
	}
	else
	{
		mPointLight->colorAndRadius = DirectX::XMFLOAT4( 0.8f, 0.8f, 0.8f, 17.0f );
	}

	if ( !mLock )
	{
		HandleInput( deltaTime, remotePlayers );
		Move( deltaTime );
	}

	// Mele attack
	if( mHasMeleeStarted )
		mTimeTillattack -= deltaTime;

	if( mTimeTillattack <= 0.0f )
	{
		mIsMeleeing						= true;
		mMeleeCoolDown					= mLoadOut->meleeWeapon->attackRate;
		//RenderManager::GetInstance()->AnimationStartNew( mArms.leftArm, mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][ATTACK] );
		//mLeftArmAnimationCompleted		= false;

		//QueueEvent( new Event_Client_Attack( mID, LEFT_ARM_ID, mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][ATTACK]) );

		mTimeTillattack = mLoadOut->meleeWeapon->timeTillAttack;
		mHasMeleeStarted		= false;
	}

	// If player is alive, update position. If hp <= 0 kill player

	if( mIsAlive )
	{
		float currentVelocity = XMVectorGetX( XMVector3Length( XMLoadFloat3( &mVelocity ) ) );

		if( currentVelocity < 0.2f )
		{
			if( mLowerBody.playerModel.mNextAnimation != mAnimations[PLAYER_ANIMATION::LEGS_IDLE] )
			{
				RenderManager::GetInstance()->AnimationStartNew( mLowerBody.playerModel, mAnimations[PLAYER_ANIMATION::LEGS_IDLE] );
			}
		}
		else
		{
			if(	mLowerBody.playerModel.mNextAnimation != mAnimations[PLAYER_ANIMATION::LEGS_WALK] )
			{
				RenderManager::GetInstance()->AnimationStartNew( mLowerBody.playerModel, mAnimations[PLAYER_ANIMATION::LEGS_WALK] );
			}
		}

		RenderManager::GetInstance()->AnimationUpdate( mLowerBody.playerModel, 
			mLowerBody.playerModel.mNextAnimation == mAnimations[PLAYER_ANIMATION::LEGS_WALK] ? deltaTime * currentVelocity / 1.1f : deltaTime );

		if( mLeftArmAnimationCompleted && mArms.leftArm.mNextAnimation != mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][IDLE] )
			RenderManager::GetInstance()->AnimationStartNew( mArms.leftArm, mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][IDLE] );

		if( mRightArmAnimationCompleted && mArms.rightArm.mNextAnimation != mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][IDLE] )
			RenderManager::GetInstance()->AnimationStartNew( mArms.rightArm, mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][IDLE] );

		RenderManager::GetInstance()->AnimationUpdate( mArms.leftArm, deltaTime );
		RenderManager::GetInstance()->AnimationUpdate( mArms.rightArm, deltaTime );
	}
	else
	{
		HandleSpawn( deltaTime );
	}
	
	if( mIsDown )
	{
		HandleDeath( deltaTime );
	}
	

	///Lock camera position to player
	XMFLOAT3 cameraPosition;
	cameraPosition.x = mLowerBody.position.x;
	cameraPosition.y = mLowerBody.position.y + 20.0f;
	cameraPosition.z = mLowerBody.position.z - 12.0f;

	Graphics::GetInstance()->SetEyePosition( CAMERAS_MAIN, cameraPosition );
	Graphics::GetInstance()->SetFocus( CAMERAS_MAIN, mLowerBody.position );

	//Shadow map camera
	cameraPosition.y = mLowerBody.position.y + 30.0f;
	cameraPosition.z = mLowerBody.position.z;

	Graphics::GetInstance()->SetEyePosition( CAMERAS_SHADOWMAP, cameraPosition );
	Graphics::GetInstance()->SetFocus( CAMERAS_SHADOWMAP, mLowerBody.position );

	//Update Bounding Primitives
	mBoundingBox->position								= mLowerBody.position;
	mBoundingCircle->center								= mLowerBody.position;
	mBoundingCircleAura->center							= mLowerBody.position;
	mLoadOut->meleeWeapon->boundingCircle->center		= mLowerBody.position;

	//Update Light
	mPointLight->position = DirectX::XMFLOAT4( mLowerBody.position.x, mLowerBody.position.y + 5.0f, mLowerBody.position.z, 0.0f );

	//== Event to sync player with server ==

	mEventCapTimer += deltaTime;
	if( mEventCapTimer > 0.02f )
	{
		IEventPtr E1( new Event_Client_Update( mID, mLowerBody.position, mVelocity, mUpperBody.direction, mPlayerName, mIsBuffed, mIsAlive ) );
		QueueEvent( E1 );
		mEventCapTimer = 0.0f;
	}
	return S_OK;
}

HRESULT Player::Render( float deltaTime, int position )
{
	if( !mIsAlive )
	{
        std::string textToWrite = std::to_string( (int)mTimeTillSpawn );
		mFont.WriteText( textToWrite, (float)Input::GetInstance()->mScreenWidth/2, (float)Input::GetInstance()->mScreenHeight/2, 7.8f );
	}

	RemotePlayer::Render( position );

	return S_OK;
}

HRESULT Player::Initialize()
{
	RemotePlayer::Initialize();

	srand( (unsigned int)time( NULL ) );

	mLowerBody.position	= XMFLOAT3( 3.0f, 0.0f, 0.0f );
	mPlayerName = "";
	
	////////////
	// Light
	////////////
	mPointLight				= new PointLight;
	mPointLight->position	= DirectX::XMFLOAT4( mLowerBody.position.x, mLowerBody.position.y, mLowerBody.position.z, 0.0f );
	IEventPtr reg( new Event_Add_Point_Light( mPointLight ) );
	EventManager::GetInstance()->QueueEvent( reg );

	mPointLight->colorAndRadius		= DirectX::XMFLOAT4( 0.8f, 0.8f, 0.8f, 17.0f );

	mMaxVelocity		= 7.7f;
	mCurrentVelocity	= 0.0f;
	mMaxAcceleration	= 20.0f;
	mAcceleration		= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mVelocity			= XMFLOAT3( 0.0f, 0.0f, 0.0f );

	mBuffMod			= 0.5f;
	
	mSpawnTime				= 10.0f;
	mReviveTime				= 2.0f;
	mTimeTillSpawn			= mSpawnTime;
	mTimeTillRevive			= mReviveTime;

	EventManager::GetInstance()->AddListener( &Player::EventListener, this, Event_Remote_Died::GUID );
	EventManager::GetInstance()->AddListener( &Player::EventListener, this, Event_Remote_Attempt_Revive::GUID );
	EventManager::GetInstance()->AddListener( &Player::EventListener, this, Event_Server_Enemy_Attack_Player::GUID );
	EventManager::GetInstance()->AddListener( &Player::EventListener, this, Event_Remote_Melee_Hit::GUID );
	EventManager::GetInstance()->AddListener( &Player::EventListener, this, Event_Create_Player_Name::GUID );
	EventManager::GetInstance()->AddListener( &Player::EventListener, this, Event_Server_Change_Buff_State::GUID );
	mTimeTillattack	= mLoadOut->meleeWeapon->timeTillAttack;
	
	return S_OK;
}

void Player::Release()
{
	RemotePlayer::Release();

	IEventPtr reg( new Event_Remove_Point_Light( mPointLight ) );
	EventManager::GetInstance()->QueueEvent( reg );
	SAFE_DELETE( mPointLight );
}

Player::Player()
	:RemotePlayer()
{
	mEventCapTimer		= 0.0f;

	mPointLight			= nullptr;

	mWeaponCoolDown		= 0.0f;
	mMeleeCoolDown		= 0.0f;
	mTimeTillattack		= 0.0f;
	mIsMeleeing			= false;
	mLock				= false;
	mCloseToPlayer		= false;

	mMaxVelocity		= 0.0f;
	mCurrentVelocity	= 0.0f;
	mMaxAcceleration	= 0.0f;
	mAcceleration		= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mFireDirection		= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mIsBuffed			= false;
	mBuffMod			= 0.0f;
	mHasMeleeStarted	= false;
	
	mSpawnTime				= 0.0f;
	mTimeTillSpawn			= 0.0f;
	mDeathTime				= 0.0f;
	mTimeTillDeath			= 0.0f;
	mReviveTime				= 0.0f;
	mTimeTillRevive			= 0.0f;
	mLastKiller				= 0;

	gEventList				= std::list<IEventPtr>();
}

Player::~Player()
{}

/////GetSet

bool Player::GetIsMeleeing() const
{
	return mIsMeleeing;
}

XMFLOAT3 Player::GetPlayerPosition() const
{
	return mLowerBody.position;
}

XMFLOAT3 Player::GetUpperBodyDirection() const
{
	return mUpperBody.direction;
}

void Player::SetIsMeleeing( bool isMeleeing )
{
	mIsMeleeing = isMeleeing;
}

void Player::SetID( unsigned int id )
{
	mID = id;
}

void Player::SetTeam( int team )
{
	mTeam		= team;
}

void Player::SetPosition( XMVECTOR position )
{
	XMStoreFloat3( &mLowerBody.position, position );
}
