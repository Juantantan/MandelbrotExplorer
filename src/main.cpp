#define CL_TARGET_OPENCL_VERSION 120
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#if defined __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <OpenCL/cl.h>
    #include <OpenCL/cl_gl.h>
    // For Platform specific shared contexts of OpneGL/OpenCL (Interoperability)
    #include <OpenCL/cl_gl_ext.h>
    // Possibly also one or both of these below
    #include <OpenGL/OpenGL.h>
    #include <OpenCL/opencl.h>
#elif defined _WIN32 || defined _WIN64 || defined __unix__ || defined linux
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <CL/cl.h>
    #include <CL/cl_gl.h>
    #include <CL/cl_ext.h>
    #if defined __unix__
        #include <GL/glx.h>
    #endif
#endif 

#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <fstream>

#define MAX_KERNEL_SIZE (0x100000)

// Window dimensions
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 1024;

bool init();
bool initOpenGL();

// Renders quad to the screen
void renderGLQuad();

// Frees media and shuts down SDL
void close();

static bool IsCLExtensionSupported( const char *extension );
static void exitOnFail(const char * const msg, cl_int sts);
static bool getOpenClContext();
static bool buildProgramCreateKernel();
static bool UpdateKernelArgsRewriteImage();

static const unsigned int FRACTAL_IMAGE_WIDTH  = 1024;
static const unsigned int FRACTAL_IMAGE_HEIGHT = 1024;
static const unsigned int FRACTAL_IMAGE_SIZE  = FRACTAL_IMAGE_WIDTH * FRACTAL_IMAGE_HEIGHT; // pixels (& texels if 1:1)
static const size_t GWSize[]= {FRACTAL_IMAGE_WIDTH, FRACTAL_IMAGE_HEIGHT, 1};
static const size_t LocalWorkSize[3] = { 32, 32, 1 };

// Global Variables
double minX = -2.05f; 
double maxX = 0.95f;  
double minY = -1.5f; 
double maxY = 1.0f; 
double midX = (maxX + minX) / 2.0f;
double midY = (maxY + minY) / 2.0f;
double dblXrange = 0.0f - minX + maxX;
double dblYrange = 0.0f - minY + maxY;
bool bZoomIn = false;
unsigned long long lZoomFactor = 1;
int mouseX;
int mouseY;

// Global OpenCL / OpenGL Variables
cl_context g_clContext;
cl_device_id devices[1];
cl_command_queue commands;
cl_kernel kernel;
cl_mem  writeToImage;
GLuint RenderFromTexture;
cl_platform_id platform;
cl_int status;

#ifdef main
# undef main
#endif /* main */

// SDL2 window
SDL_Window* glWindow = NULL;

// SDL2 OpenGL context
SDL_GLContext gContext;

// Render flag
bool gRenderQuad = true;

int main( int argc, char* args[] )
{
    double dx = 7.0f;
    double dy = 88.5f; 

    /* From khronos: "An OpenCL memory object must be created after the corresponding OpenGL VBO has been created,
    but before the OpenGL rendering starts". 
      This is the crux of how SDL/OpenGL and OpenGL communicate. It requires some careful thought and understanding */

	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Main loop flag
		bool quit = false;
        UpdateKernelArgsRewriteImage();
        renderGLQuad();
        //Update screen
        SDL_GL_SwapWindow( glWindow );;
		SDL_Event e;

		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
                if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    quit = true;
                    close();
                    exit(0);
                }
                char keyDown = e.key.keysym.scancode;
                
                if (keyDown == SDL_SCANCODE_SPACE) 
                {
                    quit = true;
                    close();
                    exit(0);
                }
                else if( e.type == SDL_MOUSEBUTTONDOWN)
                {
                switch (e.button.button)
                {
                        case SDL_BUTTON_LEFT:
                        {
                            // we want to zoom in *2, centred on Mouse-Click Coordinates
                            dx = (double)e.button.x;
                            dy = (double)e.button.y;
                            printf("Left Mouse clicked; x = %f pixels. ", dx);
                            printf("y = %f pixels\n", dy);
                            midX = minX + dx / FRACTAL_IMAGE_WIDTH * dblXrange;
                            midY = minY + dy / FRACTAL_IMAGE_HEIGHT * dblYrange;
                            dblXrange = dblXrange / 2;
                            dblYrange = dblYrange / 2;
                            minX = midX - (dblXrange / 2);
                            minY = midY - (dblYrange / 2);
                            maxX = minX + dblXrange;
                            maxY = minY + dblYrange;
                            midX = (maxX + minX) / 2;
                            midY = (maxY + minY) / 2;
                            dblXrange = 0 - minX + maxX;
                            dblYrange = 0 - minY + maxY;
                            bZoomIn = true;
                            lZoomFactor *=2;
                            printf("Zoom factor= %llu \n", lZoomFactor);
                            UpdateKernelArgsRewriteImage();
                            renderGLQuad();
                            //Update screen
                            SDL_GL_SwapWindow( glWindow );;
                            break;
                        }
                        case SDL_BUTTON_RIGHT:
                        {
                            // zoom out * 0.5
                            dx = (double)e.button.x;
                            dy = (double)e.button.y;
                            printf("Right Mouse clicked; x = %f pixels. ", dx);
                            printf("y = %f pixels\n", dy);
                            midX = minX + dx / FRACTAL_IMAGE_WIDTH * dblXrange;
                            midY = minY + dy / FRACTAL_IMAGE_HEIGHT * dblYrange;
                            dblXrange = dblXrange / 0.5;
                            dblYrange = dblYrange / 0.5;
                            minX = midX - (dblXrange / 2);
                            minY = midY - (dblYrange / 2);
                            maxX = minX + dblXrange;
                            maxY = minY + dblYrange;
                            midX = (maxX + minX) / 2;
                            midY = (maxY + minY) / 2;
                            dblXrange = 0 - minX + maxX;
                            dblYrange = 0 - minY + maxY;
                            bZoomIn = true;
                            lZoomFactor *=0.5;
                            printf("Zoom factor= %llu \n", lZoomFactor);
                            UpdateKernelArgsRewriteImage();
                            renderGLQuad();

                            //Update screen
                            SDL_GL_SwapWindow( glWindow );;
                            break;
                        }
                    }
                }
			}
		}
	}
    
	close();
    exit(0);
	return 0;
}
bool init()
{
	bool success = true;

	// Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		// Use OpenGL 3.1
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		// Create SDL window
		glWindow = SDL_CreateWindow( "Mandelbrot Explorer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
		if( glWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			// Create SDL context or use current
			gContext = SDL_GL_CreateContext( glWindow );
			if( gContext == NULL )
			{
				printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
                gContext = SDL_GL_GetCurrentContext();
                if (!getOpenClContext())
                {
                    printf( "Error: Initialisation function getOpenClContext failed\n" );
                    success = false;
                }
                if (!buildProgramCreateKernel())
                {
                    printf( "Error: Initialisation function buildProgramCreateKernel failed\n" );
                    success = false;
                }
				// Initialize OpenGL
				if( !initOpenGL() )
				{
					printf( "Unable to initialize OpenGL!\n" );
					success = false;
				}

			}
		}
	}

	return success;
}
bool initOpenGL()
{
    bool success = true;
	GLenum error = GL_NO_ERROR;

    glGenTextures( 1, &RenderFromTexture );
    glBindTexture( GL_TEXTURE_2D, RenderFromTexture );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, FRACTAL_IMAGE_WIDTH, FRACTAL_IMAGE_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Create OpenCL memobject from gl texture
    writeToImage = clCreateFromGLTexture(g_clContext, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, RenderFromTexture, &status );
    // writeToImage = clCreateFromGLTexture(g_clContext, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, RenderFromTexture, &status );

	//Initialize Projection Matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %i\n", error);
		success = false;
	}

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %i\n", error);
		success = false;
	}

	//Initialize clear color
	glClearColor( 0.f, 0.f, 0.f, 1.f );

	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %i\n", error);
		success = false;
	}

	return success;
}
void renderGLQuad()
{
	glClear( GL_COLOR_BUFFER_BIT );

	if( gRenderQuad )
	{
        // Render textured quad with OpenGL

        glDisable( GL_DEPTH_TEST );
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity( );
        gluOrtho2D( 0., 1., 0., 1. );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity( );
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, RenderFromTexture );
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

        glBegin( GL_QUADS );
            glTexCoord2f( 0., 0. );
            glVertex2f( 0., 0. );
            glTexCoord2f( 1., 0. );
            glVertex2f( 1., 0. );
            glTexCoord2f( 1., 1. );
            glVertex2f( 1., 1. );
            glTexCoord2f( 0., 1. );
            glVertex2f( 0., 1. );
        glEnd( );
        glDisable( GL_TEXTURE_2D );
	}
}
void close()
{
	//Destroy window
	SDL_DestroyWindow( glWindow );
	glWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}
static bool UpdateKernelArgsRewriteImage()
{
    cl_int status = CL_SUCCESS;

    struct timeval tvalBefore;
    struct  timeval tvalAfter;

    gettimeofday(&tvalBefore, NULL);

    // Send / update kernel arguments
    status = clSetKernelArg( kernel, 0, sizeof(cl_mem), &writeToImage );
    exitOnFail("clSetKernelArg 0", status);
    status = clSetKernelArg( kernel, 1, sizeof(double), &minX );
    exitOnFail("clSetKernelArg 1", status);
    status = clSetKernelArg( kernel, 2, sizeof(double), &maxX );
    exitOnFail("clSetKernelArg 2", status);
    status = clSetKernelArg( kernel, 3, sizeof(double), &minY );
    exitOnFail("clSetKernelArg 3", status);

    cl_event event[3];
    // Aquire texture
    status = clEnqueueAcquireGLObjects( commands, 1, &writeToImage, 0, NULL, &event[0] );
    exitOnFail("clEnqueueAcquireGLObjects", status);
    // Execute kernel
    status = clEnqueueNDRangeKernel( commands, kernel, 2, NULL, GWSize, LocalWorkSize, 1, &event[0], &event[1] );
    exitOnFail("clEnqueueAcquireGLObjects", status);
    // Release texture
    status = clEnqueueReleaseGLObjects( commands, 1, &writeToImage, 1, &event[1], &event[2] );
    exitOnFail("clEnqueueAcquireGLObjects", status);
    clWaitForEvents( 1, &event[2] );
    clFinish(commands);

    gettimeofday(&tvalAfter, NULL);
    uint milliSecondsElapsed = (uint)((tvalAfter.tv_usec - tvalBefore.tv_usec) / 1000); 
    printf( "\n\nTime to create Mandelbrot = %u milliseconds\n", milliSecondsElapsed );

    return true;
}
bool getOpenClContext()
{
    cl_int status = 0;
    status = CL_SUCCESS;
    // Variables for Platforms
    cl_uint num_platforms = 0;
    cl_platform_id *platforms = NULL;
    cl_uint  num_devices = 0;
    cl_uint i;
    cl_uint correctPlatformIndex = 0;

    // Get Platforms
    status = clGetPlatformIDs(num_platforms, platforms, &num_platforms);
    exitOnFail("clGetPlatformIDs 1st pass", status);

    platforms = new cl_platform_id[num_platforms];

    status = clGetPlatformIDs(num_platforms, platforms, &num_platforms);
    exitOnFail("clGetPlatformIDs 2nd pass", status);

    for (i=0; i<num_platforms; i++)
    {
        char platformVendor[10240];
        char platformVersion[10240];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platformVendor), platformVendor, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(platformVersion), platformVersion, NULL);
		printf("PLATFORM_VENDOR = %s\n\t VERSION = %s\n", platformVendor, platformVersion);

        // If the Platform Vendor is AMD/NVIDIA/Intel (Change as appropriate for GPU or Intel if none)
        // NB: If we are relying on Intel, there is no detected GPU and this might fail on AMD chips
        const char *s1(platformVendor);
        const char *s2("NVIDIA Corporation");
        const char *s3("Advanced Micro Devices, Inc.");
        const char *s4("Intel(R) Corporation");
        if(strcmp(s1,s2) == 0 || strcmp(s1,s3) == 0)
        {
            correctPlatformIndex = i;
            platform = platforms[i];
            printf("PlatformID: %s Platform Name: %s\n", __STRING(platforms[correctPlatformIndex]), s2);
            break;
        }
        else if(strcmp(s1,s3) == 0)
        {
            correctPlatformIndex = i;
            platform = platforms[i];
            printf("PlatformID: %s Platform Name: %s\n", __STRING(platforms[correctPlatformIndex]), s3);
            break;
        }
        else if(strcmp(s1,s4) == 0)
        {
            correctPlatformIndex = i;
            platform = platforms[i];
            printf("PlatformID: %s Non GPU Platform Name: %s\n", __STRING(platforms[correctPlatformIndex]), s4);
            break;
        }
    }

	clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, devices, &num_devices);
    exitOnFail("clGetDeviceIDs", status);
	printf("=== %d OpenCL device(s) found on platform:\n", 1);

	for (cl_uint i=0; i<num_devices; i++)
	{
		char buffer[10240];
		cl_uint buf_uint;
		cl_ulong buf_ulong;

		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
		printf("  DEVICE_NAME = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
		printf("  DEVICE_VENDOR = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
		printf("  DEVICE_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
		printf("  DRIVER_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
		printf("  DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
		printf("  DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
		printf("  DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);

		// Get the Double-precision Floating Point capabilities of the device
        cl_device_fp_config flag;

        status = clGetDeviceInfo(devices[i], CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(flag), &flag, NULL);

        if (!flag)
        {
            printf("Double precision not supported \n\n");
        }
        else
        {
            printf("Following double precision features supported:\n");
        }
        if(flag & CL_FP_INF_NAN)
            {
                printf("  INF and NaN values\n");
            }
        if(flag & CL_FP_DENORM)
        {
            printf("  Denormalized numbers\n");
        }
        if(flag & CL_FP_ROUND_TO_NEAREST)
        {
            printf("  Round To Nearest Even mode\n");
        }
        if(flag & CL_FP_ROUND_TO_INF)
        {
            printf("  Round To Infinity mode\n");
        }
        if(flag & CL_FP_ROUND_TO_ZERO)
        {
            printf("  Round To Zero mode\n");
        }
        if(flag & CL_FP_FMA)
        {
            printf("  Floating-point multiply-and-add operation\n\n");
        }
	}
	if( IsCLExtensionSupported( "cl_khr_gl_sharing" ) )
    {
        printf( "cl_khr_gl_sharing is supported.\n" );
    }
    else
    {
        printf( "cl_khr_gl_sharing is not supported -- sorry.\n" );
    }

    // ################################
    // ### Platform Independent Context
    // ################################ 

    // ### Mac OS
    #ifdef __APPLE__
        CGLContextObj glContext = CGLGetCurrentContext();
        CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);
        cl_context_properties properties[] = {
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
        (cl_context_properties)shareGroup,
        0};

    // ### Windows
    #elif defined _WIN32 || defined _WIN64
        cl_context_properties properties[] = {
        CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
        CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
        0};
        HGLRC hGLRC = wglGetCurrentContext();
        //get the device context
        HDC hDC = wglGetCurrentDC();
        if (!wglMakeCurrent( hDC, hGLRC))
        {
            printf( "Error: wglMakeCurrent failed\n" );
        }
    #elif defined __unix__
        // ### Linux
        cl_context_properties properties[] = {
        CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
        CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
        CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
        0};
    #endif

    // #####################################
    // ### END Platform Independent Context
    // ##################################### 

    // Finally we can create the shared context
    g_clContext = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &status);
    exitOnFail("clCreateContextFromType", status);

    return true;
}
static bool buildProgramCreateKernel()
{
    cl_int status;
    cl_command_queue_properties commandQueueProperties = CL_QUEUE_PROFILING_ENABLE;     // No properties used for now

    // Create command queue using the (shared) context created in getOpenClContext()
    commands = clCreateCommandQueue(g_clContext, devices[0], commandQueueProperties, &status);
    exitOnFail("clCreateCommandQueue", status);

    // Read 'Mandel.cl" kernel source file
    FILE *fp;
    char *source_str;
    size_t source_size;
    fp = fopen("Mandel.cl", "r");
    if (!fp) {
        printf("Failed to load kernel");
        exit(1);
    }
    source_str = (char*)malloc(MAX_KERNEL_SIZE);
    source_size = fread( source_str, 1, MAX_KERNEL_SIZE, fp);
    fclose( fp );

    cl_program program = clCreateProgramWithSource(g_clContext, 1,
            (const char **)&source_str, (const size_t *)&source_size, &status);
    exitOnFail("clCreateProgramWithSource", status);
    
    // ### Build Program
    char*               compileOptions = NULL;  
    const cl_device_id* deviceList = NULL;      
    cl_uint             devicesCount = 0;       
    status = clBuildProgram( program, devicesCount, deviceList, compileOptions, NULL, NULL );
    exitOnFail("clBuildProgram", status);

    // ### Create the kernel
    kernel = clCreateKernel( program, "mandel", &status );
    exitOnFail("clCreateKernel", status);

    clReleaseProgram(program);
    return true;
}
static bool IsCLExtensionSupported( const char *extension )
{
	// see if the extension is bogus:

	if( extension == NULL  ||  extension[0] == '\0' )
		return false;

	char * where = (char *) strchr( extension, ' ' );
	if( where != NULL )
		return false;

	// get the full list of extensions:

	size_t extensionSize;
	clGetDeviceInfo( devices[0], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize );
	char *extensions = new char [extensionSize];
	clGetDeviceInfo( devices[0], CL_DEVICE_EXTENSIONS, extensionSize, extensions, NULL );
	
    for( char * start = extensions; ; )
	{
		where = (char *) strstr( (const char *) start, extension );
		if( where == 0 )
		{
			delete [ ] extensions;
			return false;
		}

		char * terminator = where + strlen(extension);	// points to what should be the separator

		if( *terminator == ' '  ||  *terminator == '\0'  ||  *terminator == '\r'  ||  *terminator == '\n' )
		{
			delete [ ] extensions;
			return true;
		}
		start = terminator;
	}

	delete [ ] extensions;
	return false;
}
static const char *clerror(cl_int const status)
{
    const char * definedError = "Error code not found";
    switch (status)
    {
        case CL_INVALID_GL_OBJECT:
        {
            definedError = " CL_INVALID_GL_OBJECT";
            break;
        }
        case CL_INVALID_MIP_LEVEL:
        {
            definedError =  " CL_INVALID_MIP_LEVEL";
            break;
        }
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        {
            definedError =  " CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
            break;
        }
        case CL_INVALID_PLATFORM:
        {
            definedError =  " Invalid Platform";
            break;
        }
        case CL_INVALID_DEVICE:
        {
            definedError =  " Invalid Device";
            break;
        }
        case CL_DEVICE_NOT_AVAILABLE:
        {
            definedError =  " Device Not Available";
            break;
        }
        case CL_OUT_OF_HOST_MEMORY:
        {
            definedError =  " Out of Host Memory";
            break;
        }
        // clSetKernelArg() Errors
        case CL_INVALID_KERNEL:
        {
            definedError =  " kernel is not a valid kernel object";
            break;
        }
        case CL_INVALID_ARG_INDEX:
        {
            //if arg_index is not a valid argument index.
            definedError =  " arg_index is not a valid argument index.";
            break;
        }
        case CL_INVALID_ARG_VALUE:
        {
            //if arg_value specified is NULL for an argument that is not declared with the __local qualifier or vice-versa.
            definedError =  " arg_value specified is NULL for an argument that is not declared with the __local qualifier or vice-versa.";
            break;
        }
        case CL_INVALID_MEM_OBJECT:
        {
            //for an argument declared to be a memory object when the specified arg_value is not a valid memory object.
            definedError =  " an argument declared to be a memory object when the specified arg_value is not a valid memory object.";
            break;
        }
        case CL_INVALID_SAMPLER:
        {
            //for an argument declared to be of type sampler_t when the specified arg_value is not a valid sampler object.
            definedError =  " an argument declared to be of type sampler_t when the specified arg_value is not a valid sampler object.";
            break;
        }
        case CL_INVALID_ARG_SIZE:
        {
            definedError =  " Invalid Arg Size.";
            break;
        }
        case CL_INVALID_PROGRAM_EXECUTABLE:
        {
            definedError =  " Invalid Program Executable";
            break;
        }
        case CL_INVALID_COMMAND_QUEUE :
        {
            definedError =  " Invalid Command Queue.";
            break;
        }
        case CL_INVALID_CONTEXT :
        {
            definedError =  " Invalid Context.";
            break;
        }
        case CL_INVALID_KERNEL_ARGS :
        {
            definedError =  " Invalid Kernel Args.";
            break;
        }
        case CL_INVALID_GLOBAL_WORK_SIZE :
        {
            definedError =  " Invalid GLOBAL_WORK_SIZE.";
            break;
        }

        case CL_INVALID_WORK_DIMENSION :
        {
            definedError =  " Invalid Work Dimension.";
            break;
        }
        case CL_INVALID_WORK_GROUP_SIZE :
        {
            definedError =  " Invalid Work Group Size.";
            break;
        }
        case CL_INVALID_WORK_ITEM_SIZE :
        {
            definedError =  " Invalid Work Item Size.";
            break;
        }
        case CL_INVALID_GLOBAL_OFFSET :
        {
            definedError =  " Invalid Global Offset.";
            break;
        }
        case CL_MISALIGNED_SUB_BUFFER_OFFSET :
        {
            definedError =  " MISALIGNED_SUB_BUFFER_OFFSET.";
            break;
        }

        case CL_INVALID_IMAGE_SIZE :
        {
            definedError =  " INVALID_IMAGE_SIZE.";
            break;
        }
        case CL_OUT_OF_RESOURCES :
        {
            definedError =  " Out of Resources.";
            break;
        }
        case CL_MEM_OBJECT_ALLOCATION_FAILURE  :
        {
            definedError =  " Memory Oblect Allocation Failure.";
            break;
        }
        case CL_INVALID_EVENT_WAIT_LIST  :
        {
            definedError =  " Invalid Event Wait List.";
            break;
        }

        case CL_BUILD_PROGRAM_FAILURE  :
        {
            definedError =  " Build Program Failure.";
            break;
        }
        case CL_INVALID_VALUE :
        {
            definedError =  " CL_INVALID_VALUE.";
            break;
        }
        case CL_INVALID_IMAGE_DESCRIPTOR :
        {
            definedError =  " CL_INVALID_IMAGE_DESCRIPTOR.";
            break;
        }
        case CL_INVALID_HOST_PTR :
        {
            definedError =  " CL_INVALID_HOST_PTR.";
            break;
        }
        case CL_IMAGE_FORMAT_NOT_SUPPORTED :
        {
            definedError =  " CL_IMAGE_FORMAT_NOT_SUPPORTED.";
            break;
        }
        case CL_INVALID_OPERATION :
        {
            definedError =  " CL_INVALID_OPERATION.";
            break;
        }
        default:
        {
            break;
        }
    }
    return definedError;
}
static void exitOnFail(const char * const msg, cl_int sts)
{
    if (CL_SUCCESS == sts)
         return;
    printf("Error at %s: %s\n", msg, clerror(sts));
    exit(1);
}
