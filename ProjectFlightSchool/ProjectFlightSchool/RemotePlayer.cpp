#include "RemotePlayer.h"

/////Protected

void RemotePlayer::EventListener( IEventPtr newEvent )
{
	if( newEvent->GetEventType() == Event_Remote_Update::GUID )
	{
		std::shared_ptr<Event_Remote_Update> data = std::static_pointer_cast<Event_Remote_Update>( newEvent );
		if( mID == data->ID() )
		{
			mLowerBody.position								= data->LowerBodyPos();
			mVelocity										= data->Velocity();
			if( XMVectorGetX( XMVector3Length( XMLoadFloat3( &mVelocity ) ) ) > 0.05f )
				XMStoreFloat3( &mLowerBody.direction, XMVector3Normalize( XMLoadFloat3( &data->Velocity() ) ) );
			mUpperBody.direction							= data->UpperBodyDirection();
			mPlayerName										= data->Name();

			//TEST
			mBoundingBox->position		= mLowerBody.position;
			mBoundingCircle->center		= mLowerBody.position;
			mBoundingCircleAura->center	= mLowerBody.position;
		}
	}
	else if( newEvent->GetEventType() == Event_Remote_Attack::GUID )
	{
		std::shared_ptr<Event_Remote_Attack> data = std::static_pointer_cast<Event_Remote_Attack>( newEvent );
		if( mID == data->ID() )
		{
			if( data->ArmID() == LEFT_ARM_ID )
			{
				RenderManager::GetInstance()->AnimationStartNew( mArms.leftArm, data->Animation() );
				mLeftArmAnimationCompleted = false;
			}
			else if( data->ArmID() == RIGHT_ARM_ID )
			{
				RenderManager::GetInstance()->AnimationStartNew( mArms.rightArm, data->Animation() );
				mRightArmAnimationCompleted = false;
			}
		}
	}
	else if ( newEvent->GetEventType() == Event_Remote_Died::GUID )
	{
		// Kill remote player
		std::shared_ptr<Event_Remote_Died> data = std::static_pointer_cast<Event_Remote_Died>( newEvent );
		if ( data->ID() == mID )
		{
			Die();
		}
		else if ( data->KillerID() == mID )
		{
			CountUpKills();
		}
	}
	else if ( newEvent->GetEventType() == Event_Remote_Spawned::GUID )
	{
		std::shared_ptr<Event_Remote_Spawned> data = std::static_pointer_cast<Event_Remote_Spawned>( newEvent );
		if( mID == data->ID() )
		{
			Spawn();
		}
	}
	else if ( newEvent->GetEventType() == Event_Remote_Update_HP::GUID )
	{
		std::shared_ptr<Event_Remote_Update_HP> data = std::static_pointer_cast<Event_Remote_Update_HP>( newEvent );
		if( mID == data->ID() )
		{
			mCurrentHp = data->HP();
		}
	}
	else if ( newEvent->GetEventType() == Event_Remote_Down::GUID )
	{
		std::shared_ptr<Event_Remote_Down> data = std::static_pointer_cast<Event_Remote_Down>( newEvent );
		if ( mID == data->ID() )
		{
			GoDown();
		}
	}
	else if ( newEvent->GetEventType() == Event_Remote_Up::GUID )
	{
		std::shared_ptr<Event_Remote_Up> data = std::static_pointer_cast<Event_Remote_Up>( newEvent );
		if ( mID == data->ID() )
		{
			GoUp();
		}
	}
	else if( newEvent->GetEventType() == Event_Server_Switch_Team::GUID )
	{
		std::shared_ptr<Event_Server_Switch_Team> data = std::static_pointer_cast<Event_Server_Switch_Team>( newEvent );
		if( mID == data->ID() )
		{
			mTeam = data->TeamID();
			printf( "RemotePlayer:: Spelare: %d, blev lag %d\n", mID, mTeam );
		}
	}
	else if( newEvent->GetEventType() == Event_Server_Change_Buff_State::GUID )
	{
		std::shared_ptr<Event_Server_Change_Buff_State> data = std::static_pointer_cast<Event_Server_Change_Buff_State>( newEvent );
		if( data->ID() == mID )
		{
			mIsBuffed	= data->IsBuffed();
			mBuffMod	= data->BuffMod();
		}
	}
	else if( newEvent->GetEventType() == Event_Server_Change_Weapon::GUID )
	{
		std::shared_ptr<Event_Server_Change_Weapon> data = std::static_pointer_cast<Event_Server_Change_Weapon>( newEvent );
		if( data->ID() == mID )
		{
			if( (WeaponType)data->Weapon() == MINIGUN || (WeaponType)data->Weapon() == SHOTGUN || (WeaponType)data->Weapon() == GRENADELAUNCHER || (WeaponType)data->Weapon() == SNIPER )
			{
				delete mLoadOut->rangedWeapon;
				mLoadOut->rangedWeapon	= new RangedInfo( (WeaponType)data->Weapon() );	
				RenderManager::GetInstance()->AnimationInitialize( mArms.rightArm, mWeaponModels[mLoadOut->rangedWeapon->weaponType], mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][IDLE] );
			}
			else if( (WeaponType)data->Weapon() == CLAYMORE || (WeaponType)data->Weapon() == HAMMER || (WeaponType)data->Weapon() == BLOWTORCH || (WeaponType)data->Weapon() == SAW )
			{
				if( mLoadOut->meleeWeapon->boundingCircle )
					delete mLoadOut->meleeWeapon->boundingCircle;
				delete mLoadOut->meleeWeapon;
				mLoadOut->meleeWeapon	= new MeleeInfo( (WeaponType)data->Weapon() );
				RenderManager::GetInstance()->AnimationInitialize( mArms.leftArm, mWeaponModels[mLoadOut->meleeWeapon->weaponType], mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][IDLE] );
			}
		}
	}
}

HRESULT RemotePlayer::InitializeGraphics()
{
	// Animations Body
	//team A
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamA/Animations/", "idle.PaMan", mAnimations[PLAYER_ANIMATION::LEGS_IDLE][0] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamA/Animations/", "walk.PaMan", mAnimations[PLAYER_ANIMATION::LEGS_WALK][0] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamA/Animations/", "death.PaMan", mAnimations[PLAYER_ANIMATION::LEGS_DEATH][0] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamA/Animations/", "down.PaMan", mAnimations[PLAYER_ANIMATION::LEGS_DOWN][0] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	//team B
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamB/Animations/", "idle.PaMan", mAnimations[PLAYER_ANIMATION::LEGS_IDLE][1] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamB/Animations/", "walk.PaMan", mAnimations[PLAYER_ANIMATION::LEGS_WALK][1] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamB/Animations/", "death.PaMan", mAnimations[PLAYER_ANIMATION::LEGS_DEATH][1] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamB/Animations/", "downs.PaMan", mAnimations[PLAYER_ANIMATION::LEGS_DOWN][1] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//BLOWTORCH
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Blowtorch/Animations/", "blowtorchIdle.PaMan",			mWeaponAnimations[BLOWTORCH][IDLE] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Blowtorch/Animations/", "blowtorchAttackStart.PaMan",		mWeaponAnimations[BLOWTORCH][ATTACK_START] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																							
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Blowtorch/Animations/", "blowtorchAttackIdle.PaMan",		mWeaponAnimations[BLOWTORCH][ATTACK_IDLE] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																										
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Blowtorch/Animations/", "blowtorchAttackEnd.PaMan",		mWeaponAnimations[BLOWTORCH][ATTACK_END] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																										
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Blowtorch/Animations/", "blowtorchOverheatStart.PaMan",	mWeaponAnimations[BLOWTORCH][OVERHEAT_START] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																										
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Blowtorch/Animations/", "blowtorchOverheatEnd.PaMan",		mWeaponAnimations[BLOWTORCH][OVERHEAT_END] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//CLAYMORE
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Claymore/Animations/", "claymoreIdle.PaMan"	, mWeaponAnimations[CLAYMORE][IDLE] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																						
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Claymore/Animations/", "claymoreWalk.PaMan"	, mWeaponAnimations[CLAYMORE][WALK] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																								
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Claymore/Animations/", "claymoreAttack.PaMan"	, mWeaponAnimations[CLAYMORE][ATTACK] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	
	//

	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Hammer/Animations/", "hammerIdle.PaMan",	mWeaponAnimations[HAMMER][IDLE] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																							
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Hammer/Animations/", "hammerWalk.PaMan",	mWeaponAnimations[HAMMER][WALK] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																							
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Hammer/Animations/", "hammerAttack1.PaMan", mWeaponAnimations[HAMMER][ATTACK] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																							
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Hammer/Animations/", "hammerAttack2.PaMan",	mWeaponAnimations[HAMMER][ATTACK2] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//MINIGUN
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Minigun/Animations/", "minigunWalknIdle.PaMan",	mWeaponAnimations[MINIGUN][IDLE] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	else
		mWeaponAnimations[MINIGUN][WALK] = mWeaponAnimations[MINIGUN][IDLE];

	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Minigun/Animations/", "minigunAttack.PaMan",	mWeaponAnimations[MINIGUN][ATTACK] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																							

	//GRENADELAUNCHER
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Grenadelauncher/Animations/", "grenadeLauncherIdleWalk.PaMan", mWeaponAnimations[GRENADELAUNCHER][IDLE] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																							
	else
		mWeaponAnimations[GRENADELAUNCHER][WALK] = mWeaponAnimations[GRENADELAUNCHER][IDLE];

	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Grenadelauncher/Animations/", "grenadeLauncherAttack.PaMan", mWeaponAnimations[GRENADELAUNCHER][ATTACK] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );																							

	//SHOTGUN
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Shotgun/Animations/", "shotgunIdle.PaMan",	mWeaponAnimations[SHOTGUN][IDLE] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Shotgun/Animations/", "shotgunWalk.PaMan",	mWeaponAnimations[SHOTGUN][WALK] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Shotgun/Animations/", "shotgunAttack.PaMan",	mWeaponAnimations[SHOTGUN][ATTACK] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//SNIPER
	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Sniper/Animations/", "sniperWalknIdle.PaMan",	mWeaponAnimations[SNIPER][IDLE] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	else
		mWeaponAnimations[SNIPER][WALK] = mWeaponAnimations[SNIPER][IDLE];

	if( FAILED( Graphics::GetInstance()->LoadAnimationAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Sniper/Animations/", "sniperAnim.PaMan",	mWeaponAnimations[SNIPER][ATTACK] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//////////////////////////////////////
	//			ROBOT PARTS
	/////////////////////////////////////
	AssetID model		= 0;
	AssetID skeleton	= 0;
	if( FAILED( Graphics::GetInstance()->LoadStatic3dAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamA/", "robotUpperBodyTeamA.pfs", mUpperBody.playerModel[0] ) ) ) //robotUpperBodyTeamA.pfs
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadStatic3dAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamB/", "robotBodyTeamB.pfs", mUpperBody.playerModel[1] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamA/Animations/", "legsSkel.Skel", skeleton ) ) //Skeleton for legs
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamA/", "robotLegs.apfs", skeleton, model ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	RenderManager::GetInstance()->AnimationInitialize( mLowerBody.playerModel[0], model, mAnimations[PLAYER_ANIMATION::LEGS_IDLE][0] );

	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamB/Animations/", "robotLegs.Skel", skeleton ) ) //Skeleton for legs
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/robotTeamB/", "robotLegsTeamB.apfs", skeleton, model ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	RenderManager::GetInstance()->AnimationInitialize( mLowerBody.playerModel[1], model, mAnimations[PLAYER_ANIMATION::LEGS_IDLE][1] );

	//WEAPON MODELS
	//Blowtorch
	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Blowtorch/Animations/", "blowtorchSkel.Skel", skeleton ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Blowtorch/", "blowtorch.apfs", skeleton, mWeaponModels[BLOWTORCH] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//Claymore
	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Claymore/Animations/", "claymoreSkel.Skel", skeleton ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Claymore/", "claymore.apfs", skeleton, mWeaponModels[CLAYMORE] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//Grenadelauncher
	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Grenadelauncher/Animations/", "grenadeLauncherSkel.Skel", skeleton ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Grenadelauncher/", "grenadeLauncher.apfs", skeleton, mWeaponModels[GRENADELAUNCHER] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//Hammer
	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Hammer/Animations/", "hammerSkel.Skel", skeleton ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Hammer/", "hammer.apfs", skeleton, mWeaponModels[HAMMER] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//Minigun
	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Minigun/Animations/", "minigunSkel.Skel", skeleton ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Minigun/", "minigun.apfs", skeleton, mWeaponModels[MINIGUN] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//Saw
	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Saw/Animations/", "sawSkel.Skel", skeleton ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Saw/", "saw.apfs", skeleton, mWeaponModels[SAW] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//Shotgun
	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Shotgun/Animations/", "shotgunSkel.Skel", skeleton ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Shotgun/", "shotgun.apfs", skeleton, mWeaponModels[SHOTGUN] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );

	//Sniper
	if( Graphics::GetInstance()->LoadSkeletonAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Sniper/Animations/", "sniperSkel.Skel", skeleton ) )
		OutputDebugString( L"\nERROR loading player model\n" );
	if( FAILED( Graphics::GetInstance()->LoadAnimated3dAsset( "../Content/Assets/PermanentAssets/Robot/Weapons/Sniper/", "sniper.apfs", skeleton, mWeaponModels[SNIPER] ) ) )
		OutputDebugString( L"\nERROR loading player model\n" );


	//////////////////////////////////////
	//			HUD ELEMENTS
	/////////////////////////////////////

	mFont.Initialize( "../Content/Assets/GUI/Fonts/mv_boli_26_red/" );
	
	mLeftArmAnimationCompleted				= false;
	mRightArmAnimationCompleted				= false;

	RenderManager::GetInstance()->AnimationInitialize( mArms.leftArm, mWeaponModels[mLoadOut->meleeWeapon->weaponType], mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][IDLE] );
	RenderManager::GetInstance()->AnimationInitialize( mArms.rightArm, mWeaponModels[mLoadOut->rangedWeapon->weaponType], mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][IDLE] );

	return S_OK;
}

/////Public

// Kill remote player
// Set current hp to 0 to avoid negative values and send event that player has died
void RemotePlayer::Die()
{
	IEventPtr reg( new Event_Remove_Point_Light( mPointLightIfDown ) );
	EventManager::GetInstance()->QueueEvent( reg );
	SAFE_DELETE( mPointLightIfDown );
	mNrOfDeaths++;
	mIsAlive		= false;
	mIsDown			= false;
	mCurrentHp		= 0.0f;
	mTimeTillSpawn	= mSpawnTime;
}

void RemotePlayer::Spawn()
{
	mIsAlive			= true;
	mCurrentHp			= mMaxHp;
	mLowerBody.position = mSpawnPosition;
}

void RemotePlayer::CountUpKills()
{
	mNrOfKills++;
}

void RemotePlayer::GoDown()
{
	mTimeTillDeath				= mDeathTime;
	mIsDown						= true;
	mPointLightIfDown			= new PointLight;
	mPointLightIfDown->position	= DirectX::XMFLOAT4( mLowerBody.position.x, mLowerBody.position.y, mLowerBody.position.z, 0.0f );
	IEventPtr reg( new Event_Add_Point_Light( mPointLightIfDown ) );
	EventManager::GetInstance()->QueueEvent( reg );
	mPointLightIfDown->colorAndRadius = DirectX::XMFLOAT4( 0.6f, 0.2f, 0.6f, mTimeTillDeath );
}

void RemotePlayer::GoUp()
{
	mIsDown		= false;
	mCurrentHp	= mMaxHp / 5.0f;
	IEventPtr reg( new Event_Remove_Point_Light( mPointLightIfDown ) );
	EventManager::GetInstance()->QueueEvent( reg );
	SAFE_DELETE( mPointLightIfDown );
}

HRESULT RemotePlayer::Update( float deltaTime )
{
	//Keep the rootMatrix for legs updated.
	XMFLOAT4X4 upperBody = Graphics::GetInstance()->GetRootMatrix( mLowerBody.playerModel[TEAM_ARRAY_ID] );
	XMMATRIX loadedMat = XMLoadFloat4x4( &upperBody );
	XMMATRIX translate = XMMatrixTranslation( mLowerBody.position.x, mLowerBody.position.y, mLowerBody.position.z );

	float yaw = -atan2f( mUpperBody.direction.z, mUpperBody.direction.x );
	XMMATRIX rotate	= XMMatrixRotationRollPitchYaw( 0.0f, yaw, 0.0 );

	XMMATRIX transformation = loadedMat * rotate * translate;

	XMStoreFloat4x4( &mLowerBody.rootMatrix, transformation );

	// Update Animation
	if( !mIsDown )
	{
		if( mIsAlive )
		{
			float currentVelocity = XMVectorGetX( XMVector3Length( XMLoadFloat3( &mVelocity ) ) );

			if( currentVelocity < 0.2f )
			{
				if( mLowerBody.playerModel[TEAM_ARRAY_ID].mNextAnimation != mAnimations[PLAYER_ANIMATION::LEGS_IDLE][TEAM_ARRAY_ID] )
				{
					RenderManager::GetInstance()->AnimationStartNew( mLowerBody.playerModel[TEAM_ARRAY_ID], mAnimations[PLAYER_ANIMATION::LEGS_IDLE][TEAM_ARRAY_ID] );
				}
			}
			else
			{
				if(	mLowerBody.playerModel[TEAM_ARRAY_ID].mNextAnimation != mAnimations[PLAYER_ANIMATION::LEGS_WALK][TEAM_ARRAY_ID] )
				{
					RenderManager::GetInstance()->AnimationStartNew( mLowerBody.playerModel[TEAM_ARRAY_ID], mAnimations[PLAYER_ANIMATION::LEGS_WALK][TEAM_ARRAY_ID] );
				}
			}

			RenderManager::GetInstance()->AnimationUpdate( mLowerBody.playerModel[TEAM_ARRAY_ID], 
				mLowerBody.playerModel[TEAM_ARRAY_ID].mNextAnimation == mAnimations[PLAYER_ANIMATION::LEGS_WALK][TEAM_ARRAY_ID] ? deltaTime * currentVelocity / 1.1f : deltaTime );

			if( mLeftArmAnimationCompleted && mArms.leftArm.mNextAnimation != mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][IDLE] )
				RenderManager::GetInstance()->AnimationStartNew( mArms.leftArm, mWeaponAnimations[mLoadOut->meleeWeapon->weaponType][IDLE] );

			if( mRightArmAnimationCompleted && mArms.rightArm.mNextAnimation != mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][IDLE] )
				RenderManager::GetInstance()->AnimationStartNew( mArms.rightArm, mWeaponAnimations[mLoadOut->rangedWeapon->weaponType][IDLE] );

			RenderManager::GetInstance()->AnimationUpdate( mArms.leftArm, deltaTime );
			RenderManager::GetInstance()->AnimationUpdate( mArms.rightArm, deltaTime );
		}
		else
		{		
			if( mLowerBody.playerModel[TEAM_ARRAY_ID].mNextAnimation != mAnimations[PLAYER_ANIMATION::LEGS_DEATH][TEAM_ARRAY_ID] )
				RenderManager::GetInstance()->AnimationStartNew( mLowerBody.playerModel[TEAM_ARRAY_ID], mAnimations[PLAYER_ANIMATION::LEGS_DEATH][TEAM_ARRAY_ID] );
			RenderManager::GetInstance()->AnimationUpdate( mLowerBody.playerModel[TEAM_ARRAY_ID], deltaTime );

			/////////////////////////////////////////////////
			// interpolate upper to face lower direction
			XMVECTOR upLoad		= XMLoadFloat3( &mUpperBody.direction );
			XMVECTOR lowLoad	= XMLoadFloat3( &mLowerBody.direction );
			float change		= min( 1.0f, 6.0f * deltaTime );
			XMStoreFloat3( &mUpperBody.direction, upLoad * ( 1.0f - change ) + lowLoad * change );
			/////////////////////////////////////////////////

			//Particle effects when player is dead.
			mPlayerDownSparksTimer	-= deltaTime;
			mTimeTillSpawn			-= deltaTime;

			// Spawn smoke and electricity after ground impact
			if ( mTimeTillSpawn < 8.6f )
			{
				XMFLOAT3 newPos;
				XMStoreFloat3( &newPos, XMVector3TransformCoord( XMVectorZero(), XMLoadFloat4x4( &mLowerBody.rootMatrix ) ) );

				RenderManager::GetInstance()->RequestParticleSystem( mID, FireSmoke, XMFLOAT3( newPos.x, newPos.y - 0.3f, newPos.z ), XMFLOAT3( 0.0f, -0.1f , 0.0f ) );

				if( mPlayerDownSparksTimer < 0.0f )
				{
					RenderManager::GetInstance()->RequestParticleSystem( mID, Spark_Electric, newPos, mUpperBody.direction );
					mPlayerDownSparksTimer = (float)( rand() % 6 + 4 ) * 0.1f;
				}
			}

			// Spawn fire at death
			else if (  9.75f <= mTimeTillSpawn && mTimeTillSpawn <= 10.0f )
			{
				XMFLOAT3 newPos;
				XMFLOAT3 inverseDir;

				XMStoreFloat3( &newPos, XMVector3TransformCoord( XMVectorZero(), XMLoadFloat4x4( &mLowerBody.rootMatrix ) ) );
				XMStoreFloat3( &inverseDir, -XMLoadFloat3( &mUpperBody.direction ) );

				RenderManager::GetInstance()->RequestParticleSystem( mID, FIRE, newPos, inverseDir );
				RenderManager::GetInstance()->RequestParticleSystem( mID, FireSmoke, XMFLOAT3( newPos.x, newPos.y + 1, newPos.z ), inverseDir );
			}
		}
	}
	else
	{
		if( mLowerBody.playerModel[TEAM_ARRAY_ID].mNextAnimation != mAnimations[PLAYER_ANIMATION::LEGS_DOWN][TEAM_ARRAY_ID] )
				RenderManager::GetInstance()->AnimationStartNew( mLowerBody.playerModel[TEAM_ARRAY_ID], mAnimations[PLAYER_ANIMATION::LEGS_DOWN][TEAM_ARRAY_ID] );
			RenderManager::GetInstance()->AnimationUpdate( mLowerBody.playerModel[TEAM_ARRAY_ID], deltaTime );

			/////////////////////////////////////////////////
			// interpolate upper to face lower direction
			XMVECTOR upLoad		= XMLoadFloat3( &mUpperBody.direction );
			XMVECTOR lowLoad	= XMLoadFloat3( &mLowerBody.direction );
			float change		= min( 1.0f, 6.0f * deltaTime );
			XMStoreFloat3( &mUpperBody.direction, upLoad * ( 1.0f - change ) + lowLoad * change );
			/////////////////////////////////////////////////

		mTimeTillDeath -= deltaTime;
		mPointLightIfDown->colorAndRadius = DirectX::XMFLOAT4( 0.6f, 0.2f, 0.6f, mTimeTillDeath );

		//Particle effects when player is downed
		mPlayerDownSparksTimer -= deltaTime;

		if ( mPlayerDownSparksTimer < 0.0f )
		{
			XMFLOAT3 newPos;
			XMFLOAT3 inverseDir;

			XMStoreFloat3( &newPos, XMVector3TransformCoord( XMVectorZero(), XMLoadFloat4x4( &mLowerBody.rootMatrix ) ) );
			XMStoreFloat3( &inverseDir, -XMLoadFloat3( &mUpperBody.direction ) );

			RenderManager::GetInstance()->RequestParticleSystem( mID, Spark_Electric, newPos, mUpperBody.direction );
			RenderManager::GetInstance()->RequestParticleSystem( mID, Spark, newPos, inverseDir );

			mPlayerDownSparksTimer = (float)( rand() % 3 + 1 ) * 0.1f;
		}
	}

	return S_OK;
}

HRESULT RemotePlayer::Render()
{
	//Render upper body
	RenderManager::GetInstance()->AddObject3dToList( mUpperBody.playerModel[TEAM_ARRAY_ID], mLowerBody.rootMatrix );

	//Render Arms
	if ( RenderManager::GetInstance()->AddAnim3dToList( mArms.leftArm,
		ANIMATION_PLAY_ONCE,
		&mLowerBody.rootMatrix ) )
	{
		mLeftArmAnimationCompleted = true;
		//OutputDebugStringA(" animComplete\n" );
	}

	if ( RenderManager::GetInstance()->AddAnim3dToList( mArms.rightArm,
		ANIMATION_PLAY_ONCE,
		&mLowerBody.rootMatrix ) )
		mRightArmAnimationCompleted = true;

	//Render lower body

	float yaw = -atan2f( mLowerBody.direction.z, mLowerBody.direction.x );
	RenderManager::GetInstance()->AddAnim3dToList(	mLowerBody.playerModel[TEAM_ARRAY_ID],
													mIsAlive ? ANIMATION_PLAY_LOOPED : ANIMATION_PLAY_ONCE,
													mLowerBody.position,
													XMFLOAT3( 0.0f, yaw, 0.0f ) );

	return S_OK;
}

HRESULT RemotePlayer::Initialize()
{
	mLowerBody.position		= XMFLOAT3( 0.0f, 0.0f, 0.0f );

	mBoundingBox			= new BoundingRectangle( 1.5f, 1.5f );
	mBoundingCircle			= new BoundingCircle( 0.5f );
	mBoundingCircleAura		= new BoundingCircle( 1.0f );
	
	mIsAlive				= true;
	mIsDown					= false;
	mMaxHp					= 100.0f;
	mCurrentHp				= mMaxHp;
	mNrOfDeaths				= 0;
	mNrOfKills				= 0;

	mDeathTime				= 8.0f;
	mTimeTillDeath			= mDeathTime;

	mSpawnTime				= 10.0f;
	mTimeTillSpawn			= mSpawnTime;

	//Weapon Initialization
	mLoadOut				= new LoadOut();
	mLoadOut->rangedWeapon	= new RangedInfo( GRENADELAUNCHER );
	mLoadOut->meleeWeapon	= new MeleeInfo( HAMMER );

	InitializeGraphics();
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Server_Change_Buff_State::GUID );

	mBuffMod				= 0.5f;

	return S_OK;
}

void RemotePlayer::RemoteInit( unsigned int id, int team )
{
	mID				= id;
	mTeam			= team;
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Remote_Update::GUID );
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Remote_Attack::GUID );
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Remote_Died::GUID );
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Remote_Spawned::GUID );
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Remote_Update_HP::GUID );
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Remote_Down::GUID );
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Remote_Up::GUID );
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Server_Switch_Team::GUID );
	EventManager::GetInstance()->AddListener( &RemotePlayer::EventListener, this, Event_Server_Change_Weapon::GUID );
}

void RemotePlayer::Release()
{
	mLoadOut->Release();
	SAFE_DELETE( mLoadOut );

	SAFE_DELETE( mBoundingBox );
	SAFE_DELETE( mBoundingCircle );
	SAFE_DELETE( mBoundingCircleAura );

	if( mIsDown )
	{
		IEventPtr reg( new Event_Remove_Point_Light( mPointLightIfDown ) );
		EventManager::GetInstance()->QueueEvent( reg );
		SAFE_DELETE( mPointLightIfDown );
	}

	mFont.Release();
}

RemotePlayer::RemotePlayer()
{
	mUpperBody.playerModel[TEAM_ARRAY_ID]	= 0;
	mUpperBody.direction			= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mLowerBody.position				= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mLowerBody.direction			= XMFLOAT3( 0.0f, 0.0f, 0.0f );

	mID								= 0;
	mIsAlive						= true;
	mMaxHp							= 0.0f;
	mCurrentHp						= 0.0f;

	mBoundingBox					= nullptr;
	mBoundingCircle					= nullptr;
	mBoundingCircleAura				= nullptr;
	mLoadOut						= nullptr;
	mPointLightIfDown				= nullptr;
	mSpawnPosition					= XMFLOAT3( 0.0f, 0.0f, 0.0f );
	mIsBuffed						= false;
	mBuffMod						= 0.0f;
}

RemotePlayer::~RemotePlayer()
{}

/////GetSet

bool RemotePlayer::IsAlive() const
{
	return mIsAlive;
}

bool RemotePlayer::IsDown() const
{
	return mIsDown;
}

bool RemotePlayer::IsBuffed() const
{
	return mIsBuffed;
}

LoadOut* RemotePlayer::GetLoadOut() const
{
	return mLoadOut;
}

float RemotePlayer::GetHP() const
{
	return mCurrentHp;
}

float RemotePlayer::GetMaxHP() const
{
	return mMaxHp;
}

int RemotePlayer::GetID() const
{
	return mID;
}

int RemotePlayer::GetTeam() const
{
	return mTeam;
}

BoundingRectangle* RemotePlayer::GetBoundingBox() const
{
	return mBoundingBox;
}

BoundingCircle*	RemotePlayer::GetBoundingCircle() const
{
	return mBoundingCircle;
}

BoundingCircle*	RemotePlayer::GetBoundingCircleAura() const
{
	return mBoundingCircleAura;
}

XMFLOAT3 RemotePlayer::GetPosition() const
{
	return mLowerBody.position;
}

XMFLOAT3 RemotePlayer::GetDirection() const
{
	return mLowerBody.direction;
}

std::string RemotePlayer::GetName() const
{
	return mPlayerName;
}

void RemotePlayer::SetDirection( XMFLOAT3 direction )
{
	XMStoreFloat3( &mLowerBody.direction, ( XMLoadFloat3( &mLowerBody.direction ) += XMLoadFloat3( &direction ) ) );
	XMStoreFloat3( &mLowerBody.position, XMLoadFloat3( &mLowerBody.direction ) );
}

void RemotePlayer::SetHP( float hp )
{
	mCurrentHp = hp;
}

void RemotePlayer::SetName( std::string name )
{
	mPlayerName = name;
}
