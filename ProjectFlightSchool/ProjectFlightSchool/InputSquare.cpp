#include "InputSquare.h"

void InputSquare::Add( std::string letter )
{
	if( mCurrentText.size() < mMaxChars )
		mCurrentText += letter;
}

void InputSquare::Pop()
{
	if( mCurrentText.size() > 0 )
	{
		mCurrentText.pop_back();
	}
}

bool InputSquare::IsActive()
{
	return mActive;
}

void InputSquare::SwitchActive( bool b )
{
	mActive = b;
}

std::string InputSquare::GetText()
{
	return mCurrentText;
}

void InputSquare::HandleInput()
{
	for( int i = 0; i < KEYS::KEYS_AMOUNT; i++ )
	{
		
		if( ( Input::GetInstance()->IsKeyPressed( (KEYS)i ) && mKeyTimer <= 0.0f ) || ( Input::GetInstance()->IsKeyPressed( (KEYS)i ) && i != mLastPressed ) )
		{
			mKeyTimer = KEYCOOLDOWN;
			mLastPressed = i;
			switch(i)
			{
			case KEYS::KEYS_A:
				Add( "a" );
				break;
			case KEYS::KEYS_B:
				Add( "b" );
				break;
			case KEYS::KEYS_C:
				Add( "c" );
				break;
			case KEYS::KEYS_D:
				Add( "d" );
				break;
			case KEYS::KEYS_E:
				Add( "e" );
				break;
			case KEYS::KEYS_F:
				Add( "f" );
				break;
			case KEYS::KEYS_G:
				Add( "g" );
				break;
			case KEYS::KEYS_H:
				Add( "h" );
				break;
			case KEYS::KEYS_I:
				Add( "i" );
				break;
			case KEYS::KEYS_J:
				Add( "j" );
				break;
			case KEYS::KEYS_K:
				Add( "k" );
				break;
			case KEYS::KEYS_L:
				Add( "l" );
				break;
			case KEYS::KEYS_M:
				Add( "m" );
				break;
			case KEYS::KEYS_N:
				Add( "n" );
				break;
			case KEYS::KEYS_O:
				Add( "o" );
				break;
			case KEYS::KEYS_P:
				Add( "p" );
				break;
			case KEYS::KEYS_Q:
				Add( "q" );
				break;
			case KEYS::KEYS_R:
				Add( "r" );
				break;
			case KEYS::KEYS_S:
				Add( "s" );
				break;
			case KEYS::KEYS_T:
				Add( "t" );
				break;
			case KEYS::KEYS_U:
				Add( "u" );
				break;
			case KEYS::KEYS_V:
				Add( "v" );
				break;
			case KEYS::KEYS_W:
				Add( "w" );
				break;
			case KEYS::KEYS_X:
				Add( "x" );
				break;
			case KEYS::KEYS_Y:
				Add( "y" );
				break;
			case KEYS::KEYS_Z:
				Add( "z" );
				break;

			case KEYS::KEYS_0:
				Add( "0" );
				break;
			case KEYS::KEYS_1:
				Add( "1" );
				break;
			case KEYS::KEYS_2:
				Add( "2" );
				break;
			case KEYS::KEYS_3:
				Add( "3" );
				break;
			case KEYS::KEYS_4:
				Add( "4" );
				break;
			case KEYS::KEYS_5:
				Add( "5" );
				break;
			case KEYS::KEYS_6:
				Add( "6" );
				break;
			case KEYS::KEYS_7:
				Add( "7" );
				break;
			case KEYS::KEYS_8:
				Add( "8" );
				break;
			case KEYS::KEYS_9:
				Add( "9" );
				break;

			case KEYS::KEYS_SPACE:
				Add( " " );
				break;
			case KEYS::KEYS_PERIOD:
				Add( "." );
				break;
			case KEYS::KEYS_BACKSPACE:
				Pop();
				break;
			}
			break;
		}
		else if( Input::GetInstance()->IsKeyDown( (KEYS)i ) && mKeyTimer <= 0.0f )
		{	
			mKeyTimer = HELDCOOLDOWN;
			switch(i)
			{
			case KEYS::KEYS_A:
				Add( "a" );
				break;
			case KEYS::KEYS_B:
				Add( "b" );
				break;
			case KEYS::KEYS_C:
				Add( "c" );
				break;
			case KEYS::KEYS_D:
				Add( "d" );
				break;
			case KEYS::KEYS_E:
				Add( "e" );
				break;
			case KEYS::KEYS_F:
				Add( "f" );
				break;
			case KEYS::KEYS_G:
				Add( "g" );
				break;
			case KEYS::KEYS_H:
				Add( "h" );
				break;
			case KEYS::KEYS_I:
				Add( "i" );
				break;
			case KEYS::KEYS_J:
				Add( "j" );
				break;
			case KEYS::KEYS_K:
				Add( "k" );
				break;
			case KEYS::KEYS_L:
				Add( "l" );
				break;
			case KEYS::KEYS_M:
				Add( "m" );
				break;
			case KEYS::KEYS_N:
				Add( "n" );
				break;
			case KEYS::KEYS_O:
				Add( "o" );
				break;
			case KEYS::KEYS_P:
				Add( "p" );
				break;
			case KEYS::KEYS_Q:
				Add( "q" );
				break;
			case KEYS::KEYS_R:
				Add( "r" );
				break;
			case KEYS::KEYS_S:
				Add( "s" );
				break;
			case KEYS::KEYS_T:
				Add( "t" );
				break;
			case KEYS::KEYS_U:
				Add( "u" );
				break;
			case KEYS::KEYS_V:
				Add( "v" );
				break;
			case KEYS::KEYS_W:
				Add( "w" );
				break;
			case KEYS::KEYS_X:
				Add( "x" );
				break;
			case KEYS::KEYS_Y:
				Add( "y" );
				break;
			case KEYS::KEYS_Z:
				Add( "z" );
				break;

			case KEYS::KEYS_0:
				Add( "0" );
				break;
			case KEYS::KEYS_1:
				Add( "1" );
				break;
			case KEYS::KEYS_2:
				Add( "2" );
				break;
			case KEYS::KEYS_3:
				Add( "3" );
				break;
			case KEYS::KEYS_4:
				Add( "4" );
				break;
			case KEYS::KEYS_5:
				Add( "5" );
				break;
			case KEYS::KEYS_6:
				Add( "6" );
				break;
			case KEYS::KEYS_7:
				Add( "7" );
				break;
			case KEYS::KEYS_8:
				Add( "8" );
				break;
			case KEYS::KEYS_9:
				Add( "9" );
				break;

			case KEYS::KEYS_SPACE:
				Add( " " );
				break;
			case KEYS::KEYS_PERIOD:
				Add( "." );
				break;
			case KEYS::KEYS_BACKSPACE:
				Pop();
				break;
			}
			break;
		}
	}
}

void InputSquare::Render()
{
	Image::Render();
	for(int i = 0; i < 2; i++)
	{
		mTitle->Render();
	}
	mText->WriteText( mCurrentText, (float)mUpperLeft.x + 40, (float)mUpperLeft.y + 40, 1.95f );
}

bool InputSquare::Update( float deltaTime )
{
	if( mClickTimer >= 0.0f )
	{
		mClickTimer -= deltaTime;
	}

	if( mActive )
	{
		HandleInput();

		if( mKeyTimer > 0.0f )
		{
			mKeyTimer -= deltaTime;
		}
	}

	return true;
}

void InputSquare::Reset( std::string text )
{
	mCurrentText = text;
	mActive = false;
	mLastPressed = -1;
}

void InputSquare::Initialize( std::string text, std::string imgName, float x, float y, float width, float height )
{
	if( !std::strcmp( imgName.c_str(), "Name" ) )
	{
		mMaxChars = 13;
	}
	Image::Initialize( "../Content/Assets/Textures/Menu/ip_portInput.dds", x, y, width, height );
	mCurrentText = text;
	mTitle = new Image();
	mTitle->Initialize( "../Content/Assets/Textures/Menu/" + imgName + ".dds", x, y, width, height );
	mText = new Font();
	mText->Initialize( "../Content/Assets/GUI/Fonts/final_font/" );
	mLastPressed = -1;
}

void InputSquare::Release()
{
	MovingButton::Release();
	mTitle->Release();
	SAFE_DELETE( mTitle );
	mText->Release();
	SAFE_DELETE( mText );
}

InputSquare::InputSquare()
{
	mCurrentText	= "";
	mActive			= false;
	mKeyTimer		= KEYCOOLDOWN;
	mMaxChars		= 15;
}


InputSquare::~InputSquare()
{
}
