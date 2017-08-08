#include <SDL.h>
#include <stdio.h>
#include <string>
#include <sstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class LWindow
{
	public:
		LWindow();

		bool init();

		void handleEvent( SDL_Event& e );

		void focus();

		void render();

		void free();

		int getWidth();
		int getHeight();

		bool hasMouseFocus();
		bool hasKeyboardFocus();
		bool isMinimized();
		bool isShown();

	private:
		SDL_Window* mWindow;
		SDL_Renderer* mRenderer;
		int mWindowID;
		int mWindowDisplayID;

		int mWidth;
		int mHeight;

		bool mMouseFocus;
		bool mKeyboardFocus;
		bool mFullScreen;
		bool mMinimized;
		bool mShown;
};

bool init();

void close();

LWindow gWindow;

int gTotalDisplays = 0;
SDL_Rect* gDisplayBounds = NULL; 

LWindow::LWindow()
{
	mWindow = NULL;
	mRenderer = NULL;

	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mShown = false;
	mWindowID = -1;
	mWindowDisplayID = -1;
	
	mWidth = 0;
	mHeight = 0;
}

bool LWindow::init()
{
	mWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	if( mWindow != NULL )
	{
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT;

		mRenderer = SDL_CreateRenderer( mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
		if( mRenderer == NULL )
		{
			printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
			SDL_DestroyWindow( mWindow );
			mWindow = NULL;
		}
		else
		{
			SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

			mWindowID = SDL_GetWindowID( mWindow );
			mWindowDisplayID = SDL_GetWindowDisplayIndex( mWindow );

			mShown = true;
		}
	}
	else
	{
		printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
	}

	return mWindow != NULL && mRenderer != NULL;
}

void LWindow::handleEvent( SDL_Event& e )
{
	bool updateCaption = false;

	if( e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID )
	{
		switch( e.window.event )
		{
			case SDL_WINDOWEVENT_MOVED:
			mWindowDisplayID = SDL_GetWindowDisplayIndex( mWindow );
			updateCaption = true;
			break;

			case SDL_WINDOWEVENT_SHOWN:
			mShown = true;
			break;

			case SDL_WINDOWEVENT_HIDDEN:
			mShown = false;
			break;

			case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent( mRenderer );
			break;

			case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent( mRenderer );
			break;

			case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;
			
			case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;
			
			case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			case SDL_WINDOWEVENT_MINIMIZED:
            mMinimized = true;
            break;

			case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
            break;
			
			case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
            break;

			case SDL_WINDOWEVENT_CLOSE:
			SDL_HideWindow( mWindow );
			break;
		}
	}
	else if( e.type == SDL_KEYDOWN )
	{
		bool switchDisplay = false;

		switch( e.key.keysym.sym )
		{
			case SDLK_UP:
			++mWindowDisplayID;
			switchDisplay = true;
			break;

			case SDLK_DOWN:
			--mWindowDisplayID;
			switchDisplay = true;
			break;
		}

		if( switchDisplay )
		{
			if( mWindowDisplayID < 0 )
			{
				mWindowDisplayID = gTotalDisplays - 1;
			}
			else if( mWindowDisplayID >= gTotalDisplays )
			{
				mWindowDisplayID = 0;
			}

			SDL_SetWindowPosition( mWindow, gDisplayBounds[ mWindowDisplayID ].x + ( gDisplayBounds[ mWindowDisplayID ].w - mWidth ) / 2, gDisplayBounds[ mWindowDisplayID ].y + ( gDisplayBounds[ mWindowDisplayID ].h - mHeight ) / 2 );
			updateCaption = true;
		}
	}

	if( updateCaption )
	{
		std::stringstream caption;
		caption << "SDL Tutorial - ID: " << mWindowID << " Display: " << mWindowDisplayID << " MouseFocus:" << ( ( mMouseFocus ) ? "On" : "Off" ) << " KeyboardFocus:" << ( ( mKeyboardFocus ) ? "On" : "Off" );
		SDL_SetWindowTitle( mWindow, caption.str().c_str() );
	}
}

void LWindow::focus()
{
	if( !mShown )
	{
		SDL_ShowWindow( mWindow );
	}

	SDL_RaiseWindow( mWindow );
}

void LWindow::render()
{
	if( !mMinimized )
	{	
		SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( mRenderer );

		SDL_RenderPresent( mRenderer );
	}
}

void LWindow::free()
{
	if( mWindow != NULL )
	{
		SDL_DestroyWindow( mWindow );
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth()
{
	return mWidth;
}

int LWindow::getHeight()
{
	return mHeight;
}

bool LWindow::hasMouseFocus()
{
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
	return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
	return mMinimized;
}

bool LWindow::isShown()
{
	return mShown;
}

bool init()
{
	bool success = true;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		gTotalDisplays = SDL_GetNumVideoDisplays();
		if( gTotalDisplays < 2 )
		{
			printf( "Warning: Only one display connected!" );
		}
		
		gDisplayBounds = new SDL_Rect[ gTotalDisplays ];
		for( int i = 0; i < gTotalDisplays; ++i )
		{
			SDL_GetDisplayBounds( i, &gDisplayBounds[ i ] );
		}

		if( !gWindow.init() )
		{
			printf( "Window could not be created!\n" );
			success = false;
		}
	}

	return success;
}

void close()
{
	gWindow.free();

	delete[] gDisplayBounds;
	gDisplayBounds = NULL;

	SDL_Quit();
}

int main( int argc, char* args[] )
{
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		bool quit = false;

		SDL_Event e;

		while( !quit )
		{
			while( SDL_PollEvent( &e ) != 0 )
			{
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}

				gWindow.handleEvent( e );
			}

			gWindow.render();
		}
	}

	close();

	return 0;
}
