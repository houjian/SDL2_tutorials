#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
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

void printProgramLog( GLuint program );
void printShaderLog( GLuint shader );

SDL_Window* gWindow = NULL;

SDL_GLContext gContext;

bool gRenderQuad = true;

GLuint gProgramID = 0;
GLint gVertexPos2DLocation = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;

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
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

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
				glewExperimental = GL_TRUE; 
				GLenum glewError = glewInit();
				if( glewError != GLEW_OK )
				{
					printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
				}

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

	gProgramID = glCreateProgram();

	GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );

	const GLchar* vertexShaderSource[] =
	{
		"#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
	};

	glShaderSource( vertexShader, 1, vertexShaderSource, NULL );

	glCompileShader( vertexShader );

	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vShaderCompiled );
	if( vShaderCompiled != GL_TRUE )
	{
		printf( "Unable to compile vertex shader %d!\n", vertexShader );
		printShaderLog( vertexShader );
        success = false;
	}
	else
	{
		glAttachShader( gProgramID, vertexShader );


		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		const GLchar* fragmentShaderSource[] =
		{
			"#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
		};

		glShaderSource( fragmentShader, 1, fragmentShaderSource, NULL );

		glCompileShader( fragmentShader );

		GLint fShaderCompiled = GL_FALSE;
		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
		if( fShaderCompiled != GL_TRUE )
		{
			printf( "Unable to compile fragment shader %d!\n", fragmentShader );
			printShaderLog( fragmentShader );
			success = false;
		}
		else
		{
			glAttachShader( gProgramID, fragmentShader );


			glLinkProgram( gProgramID );

			GLint programSuccess = GL_TRUE;
			glGetProgramiv( gProgramID, GL_LINK_STATUS, &programSuccess );
			if( programSuccess != GL_TRUE )
			{
				printf( "Error linking program %d!\n", gProgramID );
				printProgramLog( gProgramID );
				success = false;
			}
			else
			{
				gVertexPos2DLocation = glGetAttribLocation( gProgramID, "LVertexPos2D" );
				if( gVertexPos2DLocation == -1 )
				{
					printf( "LVertexPos2D is not a valid glsl program variable!\n" );
					success = false;
				}
				else
				{
					glClearColor( 0.f, 0.f, 0.f, 1.f );

					GLfloat vertexData[] =
					{
						-0.5f, -0.5f,
						 0.5f, -0.5f,
						 0.5f,  0.5f,
						-0.5f,  0.5f
					};

					GLuint indexData[] = { 0, 1, 2, 3 };

					glGenBuffers( 1, &gVBO );
					glBindBuffer( GL_ARRAY_BUFFER, gVBO );
					glBufferData( GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW );

					glGenBuffers( 1, &gIBO );
					glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
					glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW );
				}
			}
		}
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
		glUseProgram( gProgramID );

		glEnableVertexAttribArray( gVertexPos2DLocation );

		glBindBuffer( GL_ARRAY_BUFFER, gVBO );
		glVertexAttribPointer( gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
		glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

		glDisableVertexAttribArray( gVertexPos2DLocation );

		glUseProgram( NULL );
	}
}

void close()
{
	glDeleteProgram( gProgramID );

	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	SDL_Quit();
}

void printProgramLog( GLuint program )
{
	if( glIsProgram( program ) )
	{
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );
		
		char* infoLog = new char[ maxLength ];
		
		glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			printf( "%s\n", infoLog );
		}
		
		delete[] infoLog;
	}
	else
	{
		printf( "Name %d is not a program\n", program );
	}
}

void printShaderLog( GLuint shader )
{
	if( glIsShader( shader ) )
	{
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
		
		char* infoLog = new char[ maxLength ];
		
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			printf( "%s\n", infoLog );
		}

		delete[] infoLog;
	}
	else
	{
		printf( "Name %d is not a shader\n", shader );
	}
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
