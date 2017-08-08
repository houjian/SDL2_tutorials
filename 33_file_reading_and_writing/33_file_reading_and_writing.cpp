#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int TOTAL_DATA = 10;

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

	private:
		SDL_Texture* mTexture;

		int mWidth;
		int mHeight;
};

bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font *gFont = NULL;

LTexture gPromptTextTexture;
LTexture gDataTextures[ TOTAL_DATA ];

Sint32 gData[ TOTAL_DATA ];

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
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
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
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

				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	SDL_Color textColor = { 0, 0, 0, 0xFF };
	SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };
	
	bool success = true;

	gFont = TTF_OpenFont( "33_file_reading_and_writing/lazy.ttf", 28 );
	if( gFont == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		if( !gPromptTextTexture.loadFromRenderedText( "Enter Data:", textColor ) )
		{
			printf( "Failed to render prompt text!\n" );
			success = false;
		}
	}

	SDL_RWops* file = SDL_RWFromFile( "33_file_reading_and_writing/nums.bin", "r+b" );

	if( file == NULL )
	{
		printf( "Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );
		
		file = SDL_RWFromFile( "33_file_reading_and_writing/nums.bin", "w+b" );
		if( file != NULL )
		{
			printf( "New file created!\n" );

			for( int i = 0; i < TOTAL_DATA; ++i )
			{
				gData[ i ] = 0;	
				SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), 1 );
			}
			
			SDL_RWclose( file );
		}
		else
		{
			printf( "Error: Unable to create file! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
	}
	else
	{
		printf( "Reading file...!\n" );
		for( int i = 0; i < TOTAL_DATA; ++i )
		{
			SDL_RWread( file, &gData[ i ], sizeof(Sint32), 1 );
		}

		SDL_RWclose( file );
	}

	gDataTextures[ 0 ].loadFromRenderedText( std::to_string( gData[ 0 ] ), highlightColor );
	for( int i = 1; i < TOTAL_DATA; ++i )
	{
		gDataTextures[ i ].loadFromRenderedText( std::to_string( gData[ i ] ), textColor );
	}

	return success;
}

void close()
{
	SDL_RWops* file = SDL_RWFromFile( "33_file_reading_and_writing/nums.bin", "w+b" );
	if( file != NULL )
	{
		for( int i = 0; i < TOTAL_DATA; ++i )
		{
			SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), 1 );
		}

		SDL_RWclose( file );
	}
	else
	{
		printf( "Error: Unable to save file! %s\n", SDL_GetError() );
	}

	gPromptTextTexture.free();
	for( int i = 0; i < TOTAL_DATA; ++i )
	{
		gDataTextures[ i ].free();
	}

	TTF_CloseFont( gFont );
	gFont = NULL;

	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	TTF_Quit();
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

			SDL_Color textColor = { 0, 0, 0, 0xFF };
			SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };

			int currentData = 0;

			while( !quit )
			{
				while( SDL_PollEvent( &e ) != 0 )
				{
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					else if( e.type == SDL_KEYDOWN )
					{
						switch( e.key.keysym.sym )
						{
							case SDLK_UP:
							gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), textColor );
							--currentData;
							if( currentData < 0 )
							{
								currentData = TOTAL_DATA - 1;
							}
							
							gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), highlightColor );
							break;
							
							case SDLK_DOWN:
							gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), textColor );
							++currentData;
							if( currentData == TOTAL_DATA )
							{
								currentData = 0;
							}
							
							gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), highlightColor );
							break;

							case SDLK_LEFT:
							--gData[ currentData ];
							gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), highlightColor );
							break;
							
							case SDLK_RIGHT:
							++gData[ currentData ];
							gDataTextures[ currentData ].loadFromRenderedText( std::to_string( gData[ currentData ] ), highlightColor );
							break;
						}
					}
				}

				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				gPromptTextTexture.render( ( SCREEN_WIDTH - gPromptTextTexture.getWidth() ) / 2, 0 );
				for( int i = 0; i < TOTAL_DATA; ++i )
				{
					gDataTextures[ i ].render( ( SCREEN_WIDTH - gDataTextures[ i ].getWidth() ) / 2, gPromptTextTexture.getHeight() + gDataTextures[ 0 ].getHeight() * i );
				}

				SDL_RenderPresent( gRenderer );
			}
		}
	}

	close();

	return 0;
}
