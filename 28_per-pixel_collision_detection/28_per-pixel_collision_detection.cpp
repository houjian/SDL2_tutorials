#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>

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

	private:
		SDL_Texture* mTexture;

		int mWidth;
		int mHeight;
};

class Dot
{
    public:
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		static const int DOT_VEL = 1;

		Dot( int x, int y );

		void handleEvent( SDL_Event& e );

		void move( std::vector<SDL_Rect>& otherColliders );

		void render();

		std::vector<SDL_Rect>& getColliders();

    private:
		int mPosX, mPosY;

		int mVelX, mVelY;
		
	    std::vector<SDL_Rect> mColliders;

		void shiftColliders();
};

bool init();

bool loadMedia();

void close();

bool checkCollision( std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b );

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

LTexture gDotTexture;

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

Dot::Dot( int x, int y )
{
    mPosX = x;
    mPosY = y;

    mColliders.resize( 11 );

    mVelX = 0;
    mVelY = 0;

    mColliders[ 0 ].w = 6;
    mColliders[ 0 ].h = 1;

    mColliders[ 1 ].w = 10;
    mColliders[ 1 ].h = 1;

    mColliders[ 2 ].w = 14;
    mColliders[ 2 ].h = 1;

    mColliders[ 3 ].w = 16;
    mColliders[ 3 ].h = 2;

    mColliders[ 4 ].w = 18;
    mColliders[ 4 ].h = 2;

    mColliders[ 5 ].w = 20;
    mColliders[ 5 ].h = 6;

    mColliders[ 6 ].w = 18;
    mColliders[ 6 ].h = 2;

    mColliders[ 7 ].w = 16;
    mColliders[ 7 ].h = 2;

    mColliders[ 8 ].w = 14;
    mColliders[ 8 ].h = 1;

    mColliders[ 9 ].w = 10;
    mColliders[ 9 ].h = 1;

    mColliders[ 10 ].w = 6;
    mColliders[ 10 ].h = 1;

	shiftColliders();
}

void Dot::handleEvent( SDL_Event& e )
{
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move( std::vector<SDL_Rect>& otherColliders )
{
    mPosX += mVelX;
    shiftColliders();

    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) || checkCollision( mColliders, otherColliders ) )
    {
        mPosX -= mVelX;
		shiftColliders();
    }

    mPosY += mVelY;
	shiftColliders();

    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) || checkCollision( mColliders, otherColliders ) )
    {
        mPosY -= mVelY;
		shiftColliders();
    }
}

void Dot::render()
{
	gDotTexture.render( mPosX, mPosY );
}

void Dot::shiftColliders()
{
    int r = 0;

    for( int set = 0; set < mColliders.size(); ++set )
    {
        mColliders[ set ].x = mPosX + ( DOT_WIDTH - mColliders[ set ].w ) / 2;

        mColliders[ set ].y = mPosY + r;

        r += mColliders[ set ].h;
    }
}

std::vector<SDL_Rect>& Dot::getColliders()
{
	return mColliders;
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
			}
		}
	}

	return success;
}

bool loadMedia()
{
	bool success = true;

	if( !gDotTexture.loadFromFile( "28_per-pixel_collision_detection/dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	gDotTexture.free();

	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b )
{
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    for( int Abox = 0; Abox < a.size(); Abox++ )
    {
        leftA = a[ Abox ].x;
        rightA = a[ Abox ].x + a[ Abox ].w;
        topA = a[ Abox ].y;
        bottomA = a[ Abox ].y + a[ Abox ].h;

        for( int Bbox = 0; Bbox < b.size(); Bbox++ )
        {
            leftB = b[ Bbox ].x;
            rightB = b[ Bbox ].x + b[ Bbox ].w;
            topB = b[ Bbox ].y;
            bottomB = b[ Bbox ].y + b[ Bbox ].h;

            if( ( ( bottomA <= topB ) || ( topA >= bottomB ) || ( rightA <= leftB ) || ( leftA >= rightB ) ) == false )
            {
                return true;
            }
        }
    }

    return false;
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

			Dot dot( 0, 0 );
			
			Dot otherDot( SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 );
			
			while( !quit )
			{
				while( SDL_PollEvent( &e ) != 0 )
				{
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					dot.handleEvent( e );
				}

				dot.move( otherDot.getColliders() );

				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );
				
				dot.render();
				otherDot.render();

				SDL_RenderPresent( gRenderer );
			}
		}
	}

	close();

	return 0;
}
