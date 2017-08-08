#include <SDL.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();

bool initGL();

void handleKeys( unsigned char key, int x, int y );

void update();

void render();

void close();

SDL_Window* gWindow = NULL;

SDL_GLContext gContext;

bool gRenderQuad = true;

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
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			gContext = SDL_GL_CreateContext( gWindow );
			if( gContext == NULL )
			{
				printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				if( SDL_GL_SetSwapInterval( 1 ) < 0 )
				{
					printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
				}

				if( !initGL() )
				{
					printf( "Unable to initialize OpenGL!\n" );
					success = false;
				}
			}
		}
	}

	return success;
}

bool initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}
	
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}
	
	return success;
}

void handleKeys( unsigned char key, int x, int y )
{
	if( key == 'q' )
	{
		gRenderQuad = !gRenderQuad;
	}
}

void update()
{
}

void render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	
	if( gRenderQuad )
	{
		glBegin( GL_QUADS );
			glVertex2f( -0.5f, -0.5f );
			glVertex2f( 0.5f, -0.5f );
			glVertex2f( 0.5f, 0.5f );
			glVertex2f( -0.5f, 0.5f );
		glEnd();
	}
}

void close()
{
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

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
		
		SDL_StartTextInput();

		while( !quit )
		{
			while( SDL_PollEvent( &e ) != 0 )
			{
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				else if( e.type == SDL_TEXTINPUT )
				{
					int x = 0, y = 0;
					SDL_GetMouseState( &x, &y );
					handleKeys( e.text.text[ 0 ], x, y );
				}
			}

			render();
			
			SDL_GL_SwapWindow( gWindow );
		}
		
		SDL_StopTextInput();
	}

	close();

	return 0;
}
