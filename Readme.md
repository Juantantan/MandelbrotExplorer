# Mandelbrot explorer with OpenGL/OpenCL Interoperability
Mandelbrot explorer uses OpenGL/OpenCL interoperability to leverage the raw compute power of OpenCL
with the powerful graphics of OpenGL. 
The program was written and built using Code::Blocks & Microsoft VS Code. The source codebase
consists of only 2 files, main.cpp and mandel.cl. 'main.cpp' is a standard C++ file and mandel.cl
is an OpenCL kernel format file.

The program could equally be run and compiled at the command line without an IDE but the
extensions for VS Code make it very easy and pleasant to see what's going on, especillay with
debugging. If running with the command prompt, the linker flags will need to be set correctly.

If using VS Code, your project can include local config files in json format:
(launch.json/tasks.json/c_cpp_properties.json) and a VS Code workspace file. Basic ones are included 
which work on this project in linux. Here are some general instructions from the Visual Studio Code
website for setting these up for your local deployment using the GNU debugger (gdb/gcc) or CLang:
* Windows (with MinGW/MinGW-w64 & gdb)  : https://code.visualstudio.com/docs/cpp/config-mingw
* Linux (with gdb)                      : https://code.visualstudio.com/docs/cpp/config-linux
* Mac/OSX (with CLang compiler)         : https://code.visualstudio.com/docs/cpp/config-clang-mac

## 3rd party requirements 
The project requires the following libraries/source/headers:
* OpenGL - Graphics rendering (may already be present by default on Linux and Apple systems)
* OpenCL - For Compute operations (may already be present by default on Linux and Apple systems)
* SDL2 - For Screen and user interaction (requires runtime library install and header)
* SDL2_Image (optional) - For working with physical image files (requires header file include). 
  This is not strictly required unless physical images are being created from the raw image data
  sent to the screen in the base code.
  https://www.libsdl.org/
* glu.h - OpenGL helper header. (Usually already present in the include/GL folder)
Installing these will be different on different platfroms. Usually there will be an SDK 
which can be installed but may already exist on the development machine.

### OpenCL notes
The OpenCL specification is managed by Khronos Group. Khronos.org is an organisation of representatives
from many interested parties. Their homepage is here: https://www.khronos.org/
The OpenCL registry homepage is here: https://www.khronos.org/registry/OpenCL/
You can also download the specifications for OpenCL from there.
NB: It is worth noting that in 2020 the specification for OpenCL 3.0 came out. This project was written
in OpenCL 1.2, which is the core on which OpenCL 3.0 is based. This code has been tested and will run
on OpenCL 1.2 and OpenCL 3.0 compliant GPU hardware.
Read more about OpenCL 3.0 here: https://www.khronos.org/news/press/khronos-group-releases-opencl-3.0
There is a working group that also guides the development of OpenCL (and SYCL):
https://www.iwocl.org/


## Licensing 
This project code is licensed under the GNU GENERAL PUBLIC LICENSE version 3.0:
https://www.gnu.org/licenses/gpl-3.0.en.html

SDL2 and SDL_Image are made available under the zlib license: https://www.zlib.net/zlib_license.html

OpenCL/OpenGL: OpenCL and OpenGL are by definition open source. However, Graphics rendering/Compute
OpenCL capable hardware vendors usually have their own proprietory implementations and drivers.
This means that for OpenCL and OpenGL to work with your hardware, you will need to install the drivers
for your Operating System and hardware (GPU / CPU / FGPA). 
You will probably already have those installed on your machine and should keep them up-to-date.
Apart from the proprietory drivers, the actual OpenCL/OpenGL core header files are free to include and
can be obtained independently from the hardware vendors if desired but should exist on your machine
already.
Khronos are building an OpenCL SDK in GitHUb: https://github.com/KhronosGroup/OpenCL-SDK
This is licensed under the Apache 2.0 license.


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
