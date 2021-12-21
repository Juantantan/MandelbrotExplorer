# Mandelbrot explorer with GL/CL Interoperability
Mandelbrot explorer uses OpenGL/OpenCL interoperability to leverage the raw compute power of OpenCL
with the powerful graphics of OpenGL. 
The program was written and built using Code::Blocks & MS VS Code. The source codebase consists of 
only 2 files, main.cpp and Mandel.cl. 'main.cpp' is a standard C++ file and mandel.cl is an OpenCL
kernel format file.
The program could equally well be run and compiled at the command line without an IDE but the
extensions for VS Code make it very easy and pleasant to see what's going on, especillay with
debugging. If running with the command prompt, the linker flags will need to be set correctly.

If using VS Code, your project can include local config files in json format:
(launch.json/tasks.json/c_cpp_properties.json) and a VS Code workspace file. Here are some instructions 
for setting those up in your local deployment using the GNU debugger (gdb/gcc) or CLang:
* Windows (with MinGW/MinGW-w64 & gdb)  : https://code.visualstudio.com/docs/cpp/config-mingw
* Linux (with gdb)                      : https://code.visualstudio.com/docs/cpp/config-linux
* Mac/OSX (with CLang compiler)         : https://code.visualstudio.com/docs/cpp/config-clang-mac

## 3rd party requirements 
The project requires the following libraries/source/headers:
* OpenGL - Graphics rendering (may already be present on Linux and Apple systems)
* OpenCL - For Compute operations (may already be present on Linux and Apple systems)
* SDL2 - For Screen and user interaction (requires runtime library install and header)
* SDL2_Image (optional) - For working with physical image files (requires header file include). 
  This is not strictly required unless physical images are being created from the raw image data
  sent to the screen in the base code
* glu.h - OpenGL helper header. (Usually already present in the include/GL folder)
Installing these will be different on different platfroms. Usually there will be an SDK 
which can be installed but may already exist on the development machine.

## Extensions for VS code
VS Code has a host of helpful extensions available. For this project, some recommended ones are
here. To download them in the terminal, copy the 'ext...' command into the terminal and enter. Or
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
You will also need glu. GLU is often a part of the OpenGL/GL implementations.
The library file is called glew32.dll and the header is glu.h
To install SDL2, see the following:
https://wiki.libsdl.org/SourceCode & https://wiki.libsdl.org/Installation

Although probably unnecessary here, generally you should be able to download the source code
 (not binaries) and then compile your own libraries with MSYS2. An example for installing glew
  on Windows is here (2nd post of Infinite Loop)
https://community.khronos.org/t/how-do-i-build-glew-for-mingw-w64-using-cmake-on-windows/74967/3
These instructions can be followed for other C/C++ sources.


## APPLE
On MacOS, the OpenGL and OpenCL runtimes are already installed in theory.
For tweaking OpenCL header and related installs, these instructions may be useful:
    https://rageandqq.github.io/blog/2018/03/09/opencl-mac-cpp.html
    https://www.py4u.net/discuss/95798

To install SDL2 and SDL2 Image with brew, follow instructions here:

* brew install sdl2
* brew install sdl2_image 
* OR see detailed instructions for SDL2 here: https://wiki.libsdl.org/Installation#macos

## LINUX
On linux, OpenCL, SDL2 and SDL2 Image are required and can be installed like this:
* sudo apt update
* sudo apt install ocl-icd-opencl-dev
* sudo apt-get install libsdl2-2.0
* sudo apt-get install libsdl2-image-2.0-0-dev

GLU may or may not already exist. GLU is an older helper header file from early versions of OpenGL

Extra build args (in tasks.json):
* "-lGL",         : OpenGL
* "-lGLU",        : GLU
* "-lSDL2main",   : SDL2 main entry
* "-lSDL2",       : SDL2
* "-lSDL2_image", : SDL Image
* "-lOpenCL"      : OpenCL
