// tag::C++11check[]
#define STRING2(x) #x
#define STRING(x) STRING2(x)

#if __cplusplus < 201103L
	#pragma message("WARNING: the compiler may not be C++11 compliant. __cplusplus version is : " STRING(__cplusplus))
#endif
// end::C++11check[]

// tag::includes[]
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <string>
#include <cassert>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <chrono>

#define GLM_FORCE_RADIANS // suppress a warning in GLM 0.9.5
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
// end::includes[]

// tag::using[]
// see https://isocpp.org/wiki/faq/Coding-standards#using-namespace-std
// don't use the whole namespace, either use the specific ones you want, or just type std::
using std::cout;
using std::cerr;
using std::endl;
using std::max;
using std::string;
using namespace std::chrono;
// end::using[]


// tag::globalVariables[]
std::string exeName;
SDL_Window *win; //pointer to the SDL_Window
SDL_GLContext context; //the SDL_GLContext
int frameCount = 0;
std::string frameLine = "";
// end::globalVariables[]

// tag::loadShader[]
std::string loadShader(const string filePath) {
    std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
	if (fileStream)
	{
		string fileData( (std::istreambuf_iterator<char>(fileStream)),
		                 (std::istreambuf_iterator<char>()          ));

		cout << "Shader Loaded from " << filePath << endl;
		return fileData;
	}
	else
	{
        cerr << "Shader could not be loaded - cannot read file " << filePath << ". File does not exist." << endl;
        return "";
	}
}
// end::loadShader[]

//our variables
bool done = false;
high_resolution_clock::time_point timePrev;

// tag::vertexData[]
//the data about our geometry
const GLfloat paddleVertexData[] = {
//	    X        Y        Z        R     G     B      A
// Front face
	-0.250f, -0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 4
	-0.250f,  0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 1
	 0.250f, -0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 3
	-0.250f,  0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 1
	 0.250f, -0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 3
	 0.250f,  0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 2

// Back face
	-0.250f, -0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 8
	-0.250f,  0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 5
	 0.250f, -0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 7
	-0.250f,  0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 5
	 0.250f, -0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 7
	 0.250f,  0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 6

// Left Face
	-0.250f, -0.125f, -0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 4
	-0.250f,  0.125f, -0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 1
	-0.250f, -0.125f,  0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 8
	-0.250f,  0.125f, -0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 1
	-0.250f,  0.125f,  0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 5
	-0.250f, -0.125f,  0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 8

// Right Face
	 0.250f,  0.125f, -0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 2
	 0.250f, -0.125f, -0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 3
	 0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 7
	 0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 7
	 0.250f,  0.125f,  0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 6
	 0.250f,  0.125f, -0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 2

// Bottom Face
	-0.250f, -0.125f, -0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 4
	-0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 8
	 0.250f, -0.125f, -0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 3
	 0.250f, -0.125f, -0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 3
	 0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 7
	-0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 8

// Top Face
	-0.250f,  0.125f,  0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 5
	-0.250f,  0.125f, -0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 1
	 0.250f,  0.125f, -0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 2
	 0.250f,  0.125f, -0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 2
	 0.250f,  0.125f,  0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 6
	-0.250f,  0.125f,  0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 5
};

const GLfloat worldBoundsVertexData[] = {
	//	    X        Y        Z        R     G     B      A
	// Front face
	-0.250f, -0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 4
	-0.250f,  0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 1
	0.250f, -0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 3
	-0.250f,  0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 1
	0.250f, -0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 3
	0.250f,  0.125f, -0.125f,    1.0f, 0.0f, 0.0f,  1.0f, // 2

														  // Back face
	-0.250f, -0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 8
	-0.250f,  0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 5
	0.250f, -0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 7
	-0.250f,  0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 5
	0.250f, -0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 7
	0.250f,  0.125f,  0.125f,    0.0f, 1.0f, 0.0f,  1.0f, // 6

														  // Left Face
	-0.250f, -0.125f, -0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 4
	-0.250f,  0.125f, -0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 1
	-0.250f, -0.125f,  0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 8
	-0.250f,  0.125f, -0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 1
	-0.250f,  0.125f,  0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 5
	-0.250f, -0.125f,  0.125f,    0.0f, 0.0f, 1.0f,  1.0f, // 8

														   // Right Face
	0.250f,  0.125f, -0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 2
	0.250f, -0.125f, -0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 3
	0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 7
	0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 7
	0.250f,  0.125f,  0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 6
	0.250f,  0.125f, -0.125f,    0.0f, 0.5f, 0.0f,  1.0f, // 2

														  // Bottom Face
	-0.250f, -0.125f, -0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 4
	-0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 8
	0.250f, -0.125f, -0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 3
	0.250f, -0.125f, -0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 3
	0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 7
	-0.250f, -0.125f,  0.125f,    0.0f, 0.5f, 0.5f,  1.0f, // 8

														   // Top Face
	-0.250f,  0.125f,  0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 5
	-0.250f,  0.125f, -0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 1
	0.250f,  0.125f, -0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 2
	0.250f,  0.125f, -0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 2
	0.250f,  0.125f,  0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 6
	-0.250f,  0.125f,  0.125f,    0.5f, 0.5f, 0.0f,  1.0f, // 5
};
// end::vertexData[]

// tag::gameState[]
//the translation vector we'll pass to our GLSL program
glm::vec3 paddle1Position = { 0.0f, 0.0f, 0.0f };
glm::vec3 paddle2Position = { 0.0f, 0.0f, -4.0f };

GLfloat paddleVelocity = 1.2;

GLfloat paddle1Direction = 0.0;
GLfloat paddle2Direction = 0.0;

// end::gameState[]

// tag::GLVariables[]
//our GL and GLSL variables
//programIDs
GLuint theProgram; //GLuint that we'll fill in to refer to the GLSL program (only have 1 at this point)

//attribute locations
GLint positionLocation; //GLuint that we'll fill in with the location of the `position` attribute in the GLSL
GLint vertexColorLocation; //GLuint that we'll fill in with the location of the `vertexColor` attribute in the GLSL

//uniform location
GLint modelMatrixLocation;
GLint viewMatrixLocation;
GLint projectionMatrixLocation;

GLuint paddleVertexDataBufferObject;
GLuint paddleVertexArrayObject;

GLuint worldBoundsVertexDataBufferObject;
GLuint worldBoundspaddleVertexArrayObject;
// end::GLVariables[]



// end Global Variables
/////////////////////////

// tag::initialise[]
void initialise()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	cout << "SDL initialised OK!\n";
}
// end::initialise[]

// tag::createWindow[]
void createWindow()
{
	//get executable name, and use as window title
	int beginIdxWindows = exeName.rfind("\\"); //find last occurrence of a backslash
	int beginIdxLinux = exeName.rfind("/"); //find last occurrence of a forward slash
	int beginIdx = max(beginIdxWindows, beginIdxLinux);
	std::string exeNameEnd = exeName.substr(beginIdx + 1);
	const char *exeNameCStr = exeNameEnd.c_str();

	//create window
	win = SDL_CreateWindow(exeNameCStr, 100, 100, 600, 600, SDL_WINDOW_OPENGL); //same height and width makes the window square ...

	//error handling
	if (win == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "SDL CreatedWindow OK!\n";
}
// end::createWindow[]

// tag::setGLAttributes[]
void setGLAttributes()
{
	int major = 3;
	int minor = 3;
	cout << "Built for OpenGL Version " << major << "." << minor << endl; //ahttps://en.wikipedia.org/wiki/OpenGL_Shading_Language#Versions
	// set the opengl context version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //core profile
	cout << "Set OpenGL context to versicreate remote branchon " << major << "." << minor << " OK!\n";
}
// tag::setGLAttributes[]

// tag::createContext[]
void createContext()
{
	setGLAttributes();

	context = SDL_GL_CreateContext(win);
	if (context == nullptr){
		SDL_DestroyWindow(win);
		std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "Created OpenGL context OK!\n";
}
// end::createContext[]

// tag::initGlew[]
void initGlew()
{
	GLenum rev;
	glewExperimental = GL_TRUE; //GLEW isn't perfect - see https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
	rev = glewInit();
	if (GLEW_OK != rev){
		std::cerr << "GLEW Error: " << glewGetErrorString(rev) << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLEW Init OK!\n";
	}
}
// end::initGlew[]

// tag::createShader[]
GLuint createShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	//error check
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}
// end::createShader[]

// tag::createProgram[]
GLuint createProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}
// end::createProgram[]

// tag::initializeProgram[]
void initializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(createShader(GL_VERTEX_SHADER, loadShader("vertexShader.glsl")));
	shaderList.push_back(createShader(GL_FRAGMENT_SHADER, loadShader("fragmentShader.glsl")));

	theProgram = createProgram(shaderList);
	if (theProgram == 0)
	{
		cerr << "GLSL program creation error." << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLSL program creation OK! GLUint is: " << theProgram << std::endl;
	}

	// tag::glGetAttribLocation[]
	positionLocation = glGetAttribLocation(theProgram, "position");
	vertexColorLocation = glGetAttribLocation(theProgram, "vertexColor");
	// end::glGetAttribLocation[]

	// tag::glGetUniformLocation[]
	modelMatrixLocation = glGetUniformLocation(theProgram, "modelMatrix");
	viewMatrixLocation = glGetUniformLocation(theProgram, "viewMatrix");
	projectionMatrixLocation = glGetUniformLocation(theProgram, "projectionMatrix");

	//only generates runtime code in debug mode
	SDL_assert_release( modelMatrixLocation != -1);
	SDL_assert_release( viewMatrixLocation != -1);
	SDL_assert_release( projectionMatrixLocation != -1);
	// end::glGetUniformLocation[]

	//clean up shaders (we don't need them anymore as they are no in theProgram
	for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}
// end::initializeProgram[]

// tag::initializeVertexArrayObject[]
//setup a GL object (a VertexArrayObject) that stores how to access data and from where
void initializeVertexArrayObject()
{
	glGenVertexArrays(1, &paddleVertexArrayObject); //create a Vertex Array Object
	cout << "Vertex Array Object created OK! GLUint is: " << paddleVertexArrayObject << std::endl;

	glGenVertexArrays(1, &worldBoundspaddleVertexArrayObject); //create a Vertex Array Object
	cout << "World bounds Vertex Array Object created OK! GLUint is: " << worldBoundspaddleVertexArrayObject << std::endl;

	glBindVertexArray(paddleVertexArrayObject); //make the just created vertexArrayObject the active one

		glBindBuffer(GL_ARRAY_BUFFER, paddleVertexDataBufferObject); //bind vertexDataBufferObject

		glEnableVertexAttribArray(positionLocation); //enable attribute at index positionLocation
		glEnableVertexAttribArray(vertexColorLocation); //enable attribute at index vertexColorLocation

		// tag::glVertexAttribPointer[]
		glVertexAttribPointer(positionLocation,    3, GL_FLOAT, GL_FALSE, (7 * sizeof(GL_FLOAT)), (GLvoid *) (0 * sizeof(GLfloat))); //specify that position data contains four floats per vertex, and goes into attribute index positionLocation
		glVertexAttribPointer(vertexColorLocation, 4, GL_FLOAT, GL_FALSE, (7 * sizeof(GL_FLOAT)), (GLvoid *) (3 * sizeof(GLfloat))); //specify that position data contains four floats per vertex, and goes into attribute index vertexColorLocation
		// end::glVertexAttribPointer[]

	glBindVertexArray(worldBoundspaddleVertexArrayObject); //make the just created vertexArrayObject the active one

		glBindBuffer(GL_ARRAY_BUFFER, worldBoundsVertexDataBufferObject); //bind vertexDataBufferObject

		glEnableVertexAttribArray(positionLocation); //enable attribute at index positionLocation
		glEnableVertexAttribArray(vertexColorLocation); //enable attribute at index vertexColorLocation

														// tag::glVertexAttribPointer[]
		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, (7 * sizeof(GL_FLOAT)), (GLvoid *)(0 * sizeof(GLfloat))); //specify that position data contains four floats per vertex, and goes into attribute index positionLocation
		glVertexAttribPointer(vertexColorLocation, 4, GL_FLOAT, GL_FALSE, (7 * sizeof(GL_FLOAT)), (GLvoid *)(3 * sizeof(GLfloat))); //specify that position data contains four floats per vertex, and goes into attribute index vertexColorLocation
																																	// end::glVertexAttribPointer[]

	glBindVertexArray(0); //unbind the vertexArrayObject so we can't change it

	//cleanup
	glDisableVertexAttribArray(positionLocation); //disable vertex attribute at index positionLocation
	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind array buffer

}
// end::initializeVertexArrayObject[]

// tag::initializeVertexBuffer[]
void initializeVertexBuffer()
{
	glGenBuffers(1, &paddleVertexDataBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, paddleVertexDataBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(paddleVertexData), paddleVertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	cout << "vertexDataBufferObject created OK! GLUint is: " << paddleVertexDataBufferObject << std::endl;

	glGenBuffers(1, &worldBoundsVertexDataBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, worldBoundsVertexDataBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(worldBoundsVertexData), worldBoundsVertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	cout << "World Bounds vertexDataBufferObject created OK! GLUint is: " << paddleVertexDataBufferObject << std::endl;

	initializeVertexArrayObject();
}
// end::initializeVertexBuffer[]

// tag::loadAssets[]
void loadAssets()
{
	initializeProgram(); //create GLSL Shaders, link into a GLSL program, and get IDs of attributes and variables

	initializeVertexBuffer(); //load data into a vertex buffer

	cout << "Loaded Assets OK!\n";

	timePrev = high_resolution_clock::now(); // set the last time
}
// end::loadAssets[]

// tag::handleInput[]
void handleInput()
{
	//Event-based input handling
	//The underlying OS is event-based, so **each** key-up or key-down (for example)
	//generates an event.
	//  - https://wiki.libsdl.org/SDL_PollEvent
	//In some scenarios we want to catch **ALL** the events, not just to present state
	//  - for instance, if taking keyboard input the user might key-down two keys during a frame
	//    - we want to catch based, and know the order
	//  - or the user might key-down and key-up the same within a frame, and we still want something to happen (e.g. jump)
	//  - the alternative is to Poll the current state with SDL_GetKeyboardState

	SDL_Event event; //somewhere to store an event

	//NOTE: there may be multiple events per frame
	while (SDL_PollEvent(&event)) //loop until SDL_PollEvent returns 0 (meaning no more events)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			done = true; //set donecreate remote branch flag if SDL wants to quit (i.e. if the OS has triggered a close event,
							//  - such as window close, or SIGINT
			break;

			//keydown handling - we should to the opposite on key-up for direction controls (generally)
		case SDL_KEYDOWN:
			//Keydown can fire repeatable if key-repeat is on.
			//  - the repeat flag is set on the keyboard event, if this is a repeat event
			//  - in our case, we're going to ignore repeat events
			//  - https://wiki.libsdl.org/SDL_KeyboardEvent
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
					//hit escape to exit
					case SDLK_ESCAPE: done = true;
						break;
					case SDLK_a:
						paddle1Direction += 1.0;
						break;
					case SDLK_s:
						paddle1Direction -= 1.0;
						break;
					case SDLK_LEFT:
						paddle2Direction += 1.0;
						break;
					case SDLK_RIGHT:
						paddle2Direction -= 1.0;
						break;
				}
			break;
		case SDL_KEYUP:
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
					case SDLK_a:
						paddle1Direction -= 1.0;
						break;
					case SDLK_s:
						paddle1Direction += 1.0;
						break;
					case SDLK_LEFT:
						paddle2Direction -= 1.0;
						break;
					case SDLK_RIGHT:
						paddle2Direction += 1.0;
						break;
				}
			break;
		}
	}
}
// end::handleInput[]

// Get Delta Function - used to make sure the animation is smooth on all computers
GLdouble getDelta()
{
	auto timeCurrent = high_resolution_clock::now();

	auto timeDiff = duration_cast<nanoseconds>(timeCurrent - timePrev);

	GLdouble delta = timeDiff.count();

	delta /= 1000000000;

	timePrev = timeCurrent;

	return delta;
}

// tag::updateSimulation[]
void updateSimulation(double simLength = 0.02) //update simulation with an amount of time to simulate for (in seconds)
{
	//WARNING - we should calculate an appropriate amount of time to simulate - not always use a constant amount of time
			// see, for example, http://headerphile.blogspot.co.uk/2014/07/part-9-no-more-delays.html

	GLdouble delta = getDelta();

	paddle1Position.x += (paddleVelocity * delta * paddle1Direction);
	paddle2Position.x += (paddleVelocity * delta * paddle2Direction);

}
// end::updateSimulation[]

// tag::preRender[]
void preRender()
{
	glViewport(0, 0, 600, 600); //set viewpoint
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //set clear colour
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //clear the window (technical the scissor box bounds)
}
// end::preRender[]

// tag::render[]
void render()
{
	glUseProgram(theProgram); //installs the program object specified by program as part of current rendering state

	glBindVertexArray(paddleVertexArrayObject);

	//set projectionMatrix - how we go from 3D to 2D
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f); // perspective - makes things further away smaller
	glUniformMatrix4fv(projectionMatrixLocation, 1, false, glm::value_ptr(projectionMatrix));

	//set viewMatrix - how we control the view (viewpoint, view direction, etc)
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(paddle1Position.x, 2, 3), paddle1Position + glm::vec3(0,1,0), glm::vec3(0, 1, 0)); // looks at the closest paddle
	glUniformMatrix4fv(viewMatrixLocation, 1, false, glm::value_ptr(viewMatrix));

	// PADDLES ------------------------------------------------------------------------------------

	glm::mat4 modelMatrix = glm::mat4(1.0);
	modelMatrix = glm::translate(modelMatrix, paddle1Position);

	glUniformMatrix4fv(modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	modelMatrix = glm::mat4(1.0);

	modelMatrix = glm::translate(modelMatrix, paddle2Position);

	// rotate so a different side is showing
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(1, 0, 0));

	glUniformMatrix4fv(modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// WORLD BOUNDS -------------------------------------------------------------------------------

	glBindVertexArray(worldBoundspaddleVertexArrayObject);

	modelMatrix = glm::mat4(1.0);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0,0,0));

	glUniformMatrix4fv(modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);

	glUseProgram(0); //clean up
}
// end::render[]

// tag::postRender[]
void postRender()
{
	SDL_GL_SwapWindow(win);; //present the frame buffer to the display (swapBuffers)
	frameLine += "Frame: " + std::to_string(frameCount++);
	cout << "\r" << frameLine << std::flush;
	frameLine = "";
}
// end::postRender[]

// tag::cleanUp[]
void cleanUp()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);
	cout << "Cleaning up OK!\n";
}
// end::cleanUp[]

// tag::main[]
int main( int argc, char* args[] )
{
	exeName = args[0];
	//setup
	//- do just once
	initialise();
	createWindow();

	createContext();

	initGlew();

	glViewport(0,0,600,600); //should check what the actual window res is?

	//do stuff that only needs to happen once
	//- create shaders
	//- load vertex data
	loadAssets();

	while (!done) //loop until done flag is set)
	{
		handleInput(); // this should ONLY SET VARIABLES

		updateSimulation(); // this should ONLY SET VARIABLES according to simulation

		preRender();

		render(); // this should render the world state according to VARIABLES -

		postRender();

	}

	//cleanup and exit
	cleanUp();
	SDL_Quit();

	return 0;
}
// end::main[]
