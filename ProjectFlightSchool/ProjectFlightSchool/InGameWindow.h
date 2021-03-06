#ifndef INGAMEWINDOW_H
#define INGAMEWINDOW_H

#include "Button.h"
#include "RenderManager.h"
#include "Font.h"
#include "SoundBufferHandler.h"

#define	NR_OF_BUTTONS 4
#define NR_OF_OPTION_BUTTONS 4

struct MenuButtonStruct
{
	Button		button;
	std::string	text	= "";
	XMFLOAT2	textPos	= XMFLOAT2( 0.0f, 0.0f );
	float		scale	= 0.0f;

	bool Pressed()
	{
		if( button.LeftMousePressed() )
		{
			return true;
		}
		return false;
	}

	void Update( float deltaTime )
	{
		button.Update( deltaTime );
	}

	void Render( Font font )
	{
		button.Render();
		font.WriteText( text, textPos.x, textPos.y, scale, COLOR_CYAN );
	}
};

class InGameWindow
{
private:
	bool				mIsActive;
	bool				mInOptions;
	Font				mFont;
	AssetID				mBackground;
	MenuButtonStruct	mButtons[NR_OF_BUTTONS];
	MenuButtonStruct	mOptionButtons[NR_OF_OPTION_BUTTONS];
	bool				mInControls;
	Button				mInControlsBackground;

protected:
public:

private:
	void	HandleInput();
protected:
public:
	void	Activate();
	void	DeActivate();
	void	Update( float deltaTime );
	void	Render();
	void	Release();
	HRESULT	Initialize();
	InGameWindow();
	~InGameWindow();

	bool	IsActive() const;
};

#endif // !INGAMEWINDOW_H