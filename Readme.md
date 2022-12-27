# Mandelbrot explorer with OpenGL/OpenCL Interoperability
Mandelbrot explorer uses OpenGL/OpenCL interoperability to leverage the raw compute power of OpenCL
with the OpenGL display layer. 
The program was oroginally written and built using Code::Blocks but the current version was written using 
Microsoft VS Code. The source codebase consists of only 2 files, 'main.cpp' and 'mandel.cl'. 'main.cpp'
is a standard C++ file and 'mandel.cl' is an OpenCL kernel format file, which can be modified by the user using a
standard text editor to alter fractal appearance and complexity. The kernel performs parallel, recursive iterations
of a Mandelbrot algorithm. The kernel is written to the Grpahics hardware at compilation and returns complete
fractal RGBA pixel maps at each rendition: on program start and at each click event (zoom in / zoom out) performed 
by the user. 
NB: The kernel ('mandel.cl') has a lot of room for improvement and is by no means the most efficient code to perform
recursive mandelbrot algorithm pixel colouring functions. The same goes for the interactive navigation code in SDL2.
The centering and zoom maths is very basic, probably overly complicated and inefficient. Any mathematician who is 
interested is encouraged to improve upon it. What the software does provide is a tried and tested base from which 
fractal exploration can be done without having to worry about setting up an environment and writing all the 
bolier-plate code from scratch. It took me months to get the first basic version working many years ago and hopefully
the information here can help people avoid some of the hassle of going through that. 

## Vulkan
Vulkan is a new graphics api developed by AMD and Khronos Group. At the time of my early commits, my understanding is 
that parallel compute functionality may not be fully mature in Vulkan. OpenCL (might) still be the most direct and 
efficient api to use to do this type of parallel compute at the current time. I will probably not attempt to rewrite
this code using the Vulkan API once it has complete compute functionality but would be interested if others do. 


![Screenshot](/docs/images/rm-2.png)

The program could equally be run and compiled at the command line without an IDE but the
extensions for VS Code make it very easy to see what's going on using that, especillay when
debugging. If running with the command prompt, linker flags will need to be set correctly.
On linux, the following works. (You might have to preceed with the admin 'sudo' command, depending on your setup):

g++ main.cpp -Wall -lSDL2main -lSDL2 -lSDL2_image -lGL -lGLU -lglut -lOpenCL -o ../bin/main.out

If using VS Code, your project can include local config files in json format:
(launch.json/tasks.json/c_cpp_properties.json) and a VS Code workspace file. Basic versions are included 
which work on this project in linux on my PC. 

## Compiler 
Here are some general instructions from the Visual Studio Code website for setting these up for your local
deployment using the GNU compiler / debugger (gdb/gcc) or CLang:
* Windows (with MinGW/MinGW-w64 & gdb)  : https://code.visualstudio.com/docs/cpp/config-mingw
* Linux (with gdb)                      : https://code.visualstudio.com/docs/cpp/config-linux
* Mac/OSX (with CLang compiler)         : https://code.visualstudio.com/docs/cpp/config-clang-mac

## 3rd party requirements 
The project requires the following libraries/source/headers:
* OpenGL - Graphics rendering (may already be present by default on Linux and Mac OS)
* OpenCL - For Compute operations (may already be present by default on Linux and Mac OS)
* SDL2 - For user / screen interaction (requires runtime library install and header)
* SDL2_Image (optional) - For working with physical image files (requires header file include). 
  This is not strictly required unless physical images are being created from the raw image data
  sent to the screen in the base code.
  https://www.libsdl.org/
  (SDL2 is just one of several possible 3rd party interactive layers you could use. This code does 
  require it but could be modified to work with different user interaction libraries.
* glu.h - OpenGL helper header file. (Usually already present in the include/GL folder)
Installing these 3rd party dibraries will be different on different platforms. In Windows there is 
an SDK which can be installed but this may exist on the development machine in other OS.

### OpenCL notes
The OpenCL specification is managed by Khronos Group. Khronos.org is an organisation of representatives
from many interested parties. Their homepage is here: https://www.khronos.org/
The OpenCL registry homepage is here: https://www.khronos.org/registry/OpenCL/
You can also download the specifications for OpenCL from that page.
NB: It is worth noting that in 2020 the specification for OpenCL 3.0 came out. This project was written
in OpenCL 1.2, which is the core on which OpenCL 3.0 is based. This code has been tested and will run
using OpenCL 1.2 and / or with OpenCL 3.0 when using compliant GPU hardware if you wish.
Read more about OpenCL 3.0 here: https://www.khronos.org/news/press/khronos-group-releases-opencl-3.0
There is a working group that also guides the development of OpenCL (and SYCL):
https://www.iwocl.org/


## Licensing 
Ths project base code is licensed under the GNU GENERAL PUBLIC LICENSE version 3.0:
https://www.gnu.org/licenses/gpl-3.0.en.html

SDL2 and SDL_Image are made available under the zlib license: https://www.zlib.net/zlib_license.html
(Please refer to the SDL homepage for instructions and licensing)

OpenCL / OpenGL: OpenCL and OpenGL are by definition open source. However, graphics rendering / compute
OpenCL capable hardware vendors generally have their own proprietory implementations and drivers.
This means that for OpenCL and OpenGL to work with your hardware, you will often need to install graphics drivers
for your OS and hardware (GPU / CPU / FGPA). 
You will likely already have these installed on your machine with your graphics driver implementation and you 
will likely want keep them up-to-date.
Apart from the proprietory drivers, the actual OpenCL/OpenGL core header files are free to include and
can be obtained independently from the hardware vendors if desired.
Khronos Group are building an OpenCL SDK in GitHUb: https://github.com/KhronosGroup/OpenCL-SDK
This is licensed under the Apache 2.0 license.


## Extensions for VS code
VS Code has a host of helpful extensions available. For this project, some recommended ones are
below. To download them in the terminal, copy the 'ext...' command into the terminal and press enter. Or
install them by clicking on the extensions VS Code menu item and searching
* C/C++ (Cpp Tools)         : ext install ms-vscode.cpptools
* C/C++ Extension Pack      : ext install ms-vscode.cpptools-extension-pack
* OpenCL (author galarius)  : ext install galarius.vscode-opencl
* GitHuib Markdown Preview  : ext install bierner.github-markdown-preview


# OS specific instructions

 ## WINDOWS (64-bit)
(For Win32 substitute MinGW-64 for MinGW)
In windows, the project is most easily developed using the MinGW-64 gcc runtime:
This can be installed from the MSYS2 terminal. Instructions here: https://www.msys2.org/
You will also need the glu library and header. GLU is included as part of specific OS OpenGL implementations.
The library file is called 'glew32.dll' and the header is called 'glu.h'

To install SDL2, see the following:
https://wiki.libsdl.org/SourceCode & https://wiki.libsdl.org/Installation

Although probably unnecessary here, generally you should be able to download the source code
 (not binaries) and then compile your own libraries with MSYS2. An example for installing glew
  on Windows is here (2nd post of Infinite Loop)
https://community.khronos.org/t/how-do-i-build-glew-for-mingw-w64-using-cmake-on-windows/74967/3
These instructions can be followed for other C/C++ sources.


## APPLE
On MacOS, the OpenGL and OpenCL runtimes are likely already installed.
For tweaking OpenCL header and related installs, these instructions may be useful:
    https://rageandqq.github.io/blog/2018/03/09/opencl-mac-cpp.html
    https://www.py4u.net/discuss/95798

To install SDL2 and SDL2 Image with brew, follow instructions here:

* brew install sdl2
* brew install sdl2_image 
* OR see detailed instructions for SDL2 here: https://wiki.libsdl.org/Installation#macos

## LINUX
On linux, OpenCL, SDL2 and SDL2 Image are required and can be installed like this:
https://learncgames.com/tutorials/how-to-install-sdl-on-linux/

* sudo apt update
* sudo apt install ocl-icd-opencl-dev
* sudo apt install libsdl2-dev
* sudo apt-get install libsdl2-image-2.0-0-dev

As stated above, GLU may or may not already exist. GLU is an older helper header file from early versions of OpenGL

Extra build args (in tasks.json):
* "-lGL",         : OpenGL
* "-lGLU",        : GLU
* "-lSDL2main",   : SDL2 main entry
* "-lSDL2",       : SDL2
* "-lSDL2_image", : SDL Image
* "-lOpenCL"      : OpenCL

![Screenshot](/docs/images/rm-3.png)
