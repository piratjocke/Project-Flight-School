#ifndef GUI_H
#define GUI_H

#include "Radar.h"
#include "Text.h"
#include "Font.h"
#include "HealthBars.h"
#include "UpgradeShipWindow.h"

#define MAX_REMOTE_PLAYERS 14

struct PlayerName
{
	DirectX::XMFLOAT3 mRemotePlayerPos;
	std::string mRemotePlayerName;
	int mRemotePlayerTeamID;
	int mRemotePlayerID;
	
};

struct GuiUpdate
{
	float deltaTime;

	//Radar update info
	RADAR_UPDATE_INFO* mRadarObjects;
	UINT mNrOfObjects;
	DirectX::XMFLOAT3 mPlayerPos;
	//------------------------------

	//Player names update info
	PlayerName* mPlayerNames;
	UINT mNrOfPlayerNames;
	int mPlayerTeamID;
	//------------------------------

	//Health bar update info
	float* mAlliesHP;
	int mNrOfAllies;
	float mShipHP;
	//------------------------------

	//Player update info
	float mPlayerHP;
	float mPlayerShield;
	float mPlayerXP;
	//------------------------------
};

class Gui
{
	private:
		UINT		mNrOfRemotePlayer;

		Radar*				mRadar;
		UpgradeShipWindow	mWindow;

		HealthBar*	mHealtBar;

		Text		mPlayerNames[MAX_REMOTE_PLAYERS - 1]; //Don't need to store the local player's name

		Font		mFont;

		int			mPlayerHP;
		int			mPlayerXP;
		int			mPlayerShield;

		float		mExperience;

		AssetID		mPlayerBar;
		XMFLOAT2	mPlayerHealthXPTopLeftCorner;
		XMFLOAT2	mSizePlayerHealthXP;

		AssetID		mLevelUp;
		XMFLOAT2	mSizeLevelUp;
		XMFLOAT2	mTopLeftCompWithPlayerHealthXP;

		

	protected:
	public:

	private:
	protected:
	public:

		HRESULT	Update( GuiUpdate guiUpdate );
		HRESULT	Render();
		HRESULT	Initialize();
		void	Release();
				Gui();
				~Gui();
};


#endif