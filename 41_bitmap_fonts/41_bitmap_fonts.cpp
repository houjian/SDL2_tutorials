#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class LTexture
{
	public:
		LTexture();

		~LTexture();

		bool loadFromFile( std::string path );
		
		#ifdef _SDL_TTF_H
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		void free();

		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		void setBlendMode( SDL_BlendMode blending );

		void setAlpha( Uint8 alpha );
		
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		int getWidth();
		int getHeight();

		bool lockTexture();
		bool unlockTexture();
		void* getPixels();
		int getPitch();
		Uint32 getPixel32( unsigned int x, unsigned int y );

	private:
		SDL_Texture* mTexture;
		void* mPixels;
		int mPitch;

		int mWidth;
		int mHeight;
};

class LBitmapFont
{
    public:
		LBitmapFont();

		bool buildFont( LTexture *bitmap );

		void renderText( int x, int y, std::string text );

    private:
		LTexture* mBitmap;

		SDL_Rect mChars[ 256 ];

		int mNewLine, mSpace;
};

bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

LTexture gBitmapTexture;
LBitmapFont gBitmapFont;

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
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, NULL );
		if( formattedSurface == NULL )
		{
			printf( "Unable to convert loaded surface to display format! %s\n", SDL_GetError() );
		}
		else
		{
			newTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
			if( newTexture == NULL )
			{
				printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
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
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
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
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	return mTexture != NULL;
}
#endif

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
		printf( "Texture is already locked!\n" );
		success = false;
	}
	else
	{
		if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
		{
			printf( "Unable to lock texture! %s\n", SDL_GetError() );
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
		printf( "Texture is not locked!\n" );
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

int LTexture::getPitch()
{
	return mPitch;
}

Uint32 LTexture::getPixel32( unsigned int x, unsigned int y )
{
    Uint32 *pixels = (Uint32*)mPixels;

    return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}

LBitmapFont::LBitmapFont()
{
    mBitmap = NULL;
    mNewLine = 0;
    mSpace = 0;
}

bool LBitmapFont::buildFont( LTexture* bitmap )
{
	bool success = true;
	
	if( !bitmap->lockTexture() )
	{
		printf( "Unable to lock bitmap font texture!\n" );
		success = false;
	}
	else
	{
		Uint32 bgColor = bitmap->getPixel32( 0, 0 );

		int cellW = bitmap->getWidth() / 16;
		int cellH = bitmap->getHeight() / 16;

		int top = cellH;
		int baseA = cellH;

		int currentChar = 0;

		for( int rows = 0; rows < 16; ++rows )
		{
			for( int cols = 0; cols < 16; ++cols )
			{
				mChars[ currentChar ].x = cellW * cols;
				mChars[ currentChar ].y = cellH * rows;

				mChars[ currentChar ].w = cellW;
				mChars[ currentChar ].h = cellH;

				for( int pCol = 0; pCol < cellW; ++pCol )
				{
					for( int pRow = 0; pRow < cellH; ++pRow )
					{
						int pX = ( cellW * cols ) + pCol;
						int pY = ( cellH * rows ) + pRow;

						if( bitmap->getPixel32( pX, pY ) != bgColor )
						{
							mChars[ currentChar ].x = pX;

							pCol = cellW;
							pRow = cellH;
						}
					}
				}

				for( int pColW = cellW - 1; pColW >= 0; --pColW )
				{
					for( int pRowW = 0; pRowW < cellH; ++pRowW )
					{
						int pX = ( cellW * cols ) + pColW;
						int pY = ( cellH * rows ) + pRowW;

						if( bitmap->getPixel32( pX, pY ) != bgColor )
						{
							mChars[ currentChar ].w = ( pX - mChars[ currentChar ].x ) + 1;

							pColW = -1;
							pRowW = cellH;
						}
					}
				}

				for( int pRow = 0; pRow < cellH; ++pRow )
				{
					for( int pCol = 0; pCol < cellW; ++pCol )
					{
						int pX = ( cellW * cols ) + pCol;
						int pY = ( cellH * rows ) + pRow;

						if( bitmap->getPixel32( pX, pY ) != bgColor )
						{
							if( pRow < top )
							{
								top = pRow;
							}

							pCol = cellW;
							pRow = cellH;
						}
					}
				}

				if( currentChar == 'A' )
				{
					for( int pRow = cellH - 1; pRow >= 0; --pRow )
					{
						for( int pCol = 0; pCol < cellW; ++pCol )
						{
							int pX = ( cellW * cols ) + pCol;
							int pY = ( cellH * rows ) + pRow;

							if( bitmap->getPixel32( pX, pY ) != bgColor )
							{
								baseA = pRow;

								pCol = cellW;
								pRow = -1;
							}
						}
					}
				}

				++currentChar;
			}
		}

		mSpace = cellW / 2;

		mNewLine = baseA - top;

		for( int i = 0; i < 256; ++i )
		{
			mChars[ i ].y += top;
			mChars[ i ].h -= top;
		}

		bitmap->unlockTexture();
		mBitmap = bitmap;
	}

	return success;
}

void LBitmapFont::renderText( int x, int y, std::string text )
{
    if( mBitmap != NULL )
    {
		int curX = x, curY = y;

        for( int i = 0; i < text.length(); ++i )
        {
            if( text[ i ] == ' ' )
            {
                curX += mSpace;
            }
            else if( text[ i ] == '\n' )
            {
                curY += mNewLine;

                curX = x;
            }
            else
            {
                int ascii = (unsigned char)text[ i ];

				mBitmap->render( curX, curY, &mChars[ ascii ] );

                curX += mChars[ ascii ].w + 1;
            }
        }
    }
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

		srand( SDL_GetTicks() );

		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
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

	if( !gBitmapTexture.loadFromFile( "41_bitmap_fonts/lazyfont.png" ) )
	{
		printf( "Failed to load corner texture!\n" );
		success = false;
	}
	else
	{
		gBitmapFont.buildFont( &gBitmapTexture );
	}

	return success;
}

void close()
{
	gBitmapTexture.free();

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
		printf( "Failed to initialize!\n" );
	}
	else
	{
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
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
				}

				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				gBitmapFont.renderText( 0, 0, "Bitmap Font:\nABDCEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789" );

				SDL_RenderPresent( gRenderer );
			}
		}
	}

	close();

	return 0;
}
