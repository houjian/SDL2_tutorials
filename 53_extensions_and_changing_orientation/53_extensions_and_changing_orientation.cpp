#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <string.h>

class LTexture
{
	public:
		LTexture();

		~LTexture();

		bool loadFromFile( std::string path );

		#ifdef _SDL_TTF_H
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		bool createBlank( int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING );

		void free();

		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		void setBlendMode( SDL_BlendMode blending );

		void setAlpha( Uint8 alpha );

		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		void setAsRenderTarget();

		int getWidth();
		int getHeight();

		bool lockTexture();
		bool unlockTexture();
		void* getPixels();
		void copyPixels( void* pixels );
		int getPitch();
		Uint32 getPixel32( unsigned int x, unsigned int y );

	private:
		SDL_Texture* mTexture;
		void* mPixels;
		int mPitch;

		int mWidth;
		int mHeight;
};

bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

SDL_Rect gScreenRect = { 0, 0, 320, 240 };

LTexture gPortraitTexture;
LTexture gLandscapeTexture;

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mPixels = NULL;
	mPitch = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		SDL_Log( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, NULL );
		if( formattedSurface == NULL )
		{
			SDL_Log( "Unable to convert loaded surface to display format! %s\n", SDL_GetError() );
		}
		else
		{
			newTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
			if( newTexture == NULL )
			{
				SDL_Log( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
			}
			else
			{
				SDL_SetTextureBlendMode( newTexture, SDL_BLENDMODE_BLEND );

				SDL_LockTexture( newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch );

				memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

				mWidth = formattedSurface->w;
				mHeight = formattedSurface->h;

				Uint32* pixels = (Uint32*)mPixels;
				int pixelCount = ( mPitch / 4 ) * mHeight;

				Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
				Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00 );

				for( int i = 0; i < pixelCount; ++i )
				{
					if( pixels[ i ] == colorKey )
					{
						pixels[ i ] = transparent;
					}
				}

				SDL_UnlockTexture( newTexture );
				mPixels = NULL;
			}

			SDL_FreeSurface( formattedSurface );
		}

		SDL_FreeSurface( loadedSurface );
	}

	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	free();

	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			SDL_Log( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		SDL_FreeSurface( textSurface );
	}
	else
	{
		SDL_Log( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	return mTexture != NULL;
}
#endif

bool LTexture::createBlank( int width, int height, SDL_TextureAccess access )
{
	mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height );
	if( mTexture == NULL )
	{
		SDL_Log( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
	}
	else
	{
		mWidth = width;
		mHeight = height;
	}

	return mTexture != NULL;
}

void LTexture::free()
{
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		mPixels = NULL;
		mPitch = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

void LTexture::setAsRenderTarget()
{
	SDL_SetRenderTarget( gRenderer, mTexture );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool LTexture::lockTexture()
{
	bool success = true;

	if( mPixels != NULL )
	{
		SDL_Log( "Texture is already locked!\n" );
		success = false;
	}
	else
	{
		if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
		{
			SDL_Log( "Unable to lock texture! %s\n", SDL_GetError() );
			success = false;
		}
	}

	return success;
}

bool LTexture::unlockTexture()
{
	bool success = true;

	if( mPixels == NULL )
	{
		SDL_Log( "Texture is not locked!\n" );
		success = false;
	}
	else
	{
		SDL_UnlockTexture( mTexture );
		mPixels = NULL;
		mPitch = 0;
	}

	return success;
}

void* LTexture::getPixels()
{
	return mPixels;
}

void LTexture::copyPixels( void* pixels )
{
	if( mPixels != NULL )
	{
		memcpy( mPixels, pixels, mPitch * mHeight );
	}
}

int LTexture::getPitch()
{
	return mPitch;
}

Uint32 LTexture::getPixel32( unsigned int x, unsigned int y )
{
    Uint32 *pixels = (Uint32*)mPixels;

    return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}

bool init()
{
	bool success = true;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		SDL_Log( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			SDL_Log( "Warning: Linear texture filtering not enabled!" );
		}

        SDL_DisplayMode displayMode;
        if( SDL_GetCurrentDisplayMode( 0, &displayMode ) == 0 )
        {
            gScreenRect.w = displayMode.w;
            gScreenRect.h = displayMode.h;
        }

		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gScreenRect.w, gScreenRect.h, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			SDL_Log( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				SDL_Log( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					SDL_Log( "SDL_image could not initialize! %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	bool success = true;

	if( !gPortraitTexture.loadFromFile( "53_extensions_and_changing_orientation/portrait.png" ) )
	{
		SDL_Log( "Failed to load portrait texture!\n" );
		success = false;
	}
	if( !gLandscapeTexture.loadFromFile( "53_extensions_and_changing_orientation/landscape.png" ) )
	{
		SDL_Log( "Failed to load landscape texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	gPortraitTexture.free();
	gLandscapeTexture.free();

	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	if( !init() )
	{
		SDL_Log( "Failed to initialize!\n" );
	}
	else
	{
		if( !loadMedia() )
		{
			SDL_Log( "Failed to load media!\n" );
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
					else if( e.type == SDL_WINDOWEVENT )
					{
						if( e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
						{
							gScreenRect.w = e.window.data1;
							gScreenRect.h = e.window.data2;
							
							SDL_RenderPresent( gRenderer );
						}
					}
				}

				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				if( gScreenRect.h >= gScreenRect.w )
				{
					gPortraitTexture.render( ( gScreenRect.w - gPortraitTexture.getWidth() ) / 2, ( gScreenRect.h - gPortraitTexture.getHeight() ) / 2 );	
				}
				else
				{
					gLandscapeTexture.render( ( gScreenRect.w - gLandscapeTexture.getWidth() ) / 2, ( gScreenRect.h - gLandscapeTexture.getHeight() ) / 2 );	
				}

				SDL_RenderPresent( gRenderer );
			}
		}
	}

	close();

	return 0;
}
