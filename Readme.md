# Mandelbrot explorer with OpenGL/OpenCL Interoperability
Mandelbrot explorer uses OpenGL/OpenCL interoperability to leverage the raw compute power of OpenCL
with the OpenGL display layer. It renders colourful, detailed images from pixel maps formed by iterating through 
the Mandelbrot algorithm and 'shading' each pixel based on the number of iterations each pixel coordinate value in the 
imaginary plane takes to escape the mandelbrot set. You can zoom in and out and alter the 'kernel' file, which is
written to your Graphics hardware (GPU / CPU /...). User interaction (zooming) is done via the 3rd party SDL2 library.

At the time of writing, there was very little open source code available on the web which demonstrated OpenCL/OpenGL
interoperability. This is important because it's all very well producing an image map but displaying it to screen has
to be done to enjoy the image visually. There aren't very many OpenCL offerings when compared to OpenGL and very,
very little which combines the two APIs. 

## Mentions and thanks
Although I wrote a very simple Mandelbrot set visualiser in Visual Basic 6 in 2003, it was just for a bit of fun and very 
slow!
# Eric Bainville
A few years later, I did a search on rendering the Mandelbrot set and how others had done that. I stumbled upon the 
excellent and extensive work done by Eric Bainville. My initial implementation using OpenCL was largely a copy-and-paste job
of Eric's work and without that, which still forms the central core of the OpenCL core, I probably could not have done anything
more. I think I am right in remembering that Eric also used SDL in his implementation.
Eric has a web site here: http://www.bealto.com/cv.html

# Mark Brown
Mark was a very good friend of mine at school and is a very capable programmer. His lovely father was my geography teacher. 
Mark can (quote): 'get by in 20 or so' programming languages, which, as far as I can tell, actually means he is highly proficient,
expressed with the wonderful humility that experts in their field often seem to have.
When I got a recent version of this software to run some years ago, I passed a copy to Mark and he was kind enough to go through
it and offer advice. In particular, he tidied up my poorly executed OpenCL error-handling method and sent that back to me. 
He also commented on various aspects of what I was trying to do and opened me up to other languages and techniques, such as 
functional programming. Mark has written web guides for using Erlang, OpenGL with Lisp, OpenSSH, encryption and security and on
many other subjects. Software engineering is currently my job but I do this in a limited capacity. Mark is an expert in many
fields and has had a formal tertiary education, and gone on to study further under some eminent and revered scholars. 

# Richard Garnett and Jack Costin
Richard and Jack have been colleagues of mine at my work. I owe then so much thanks for their patience when I was slowly 
learning Python and Postgre Sql. Their precision and thoroughness helped to teach me to bring my PR's up to an acceptable 
quality. Jack wrote some really good stuff in the Rust language, which hugely speeded up some slower bits of code. I started 
to try and use a Rust wrapper early on in this project but I stopped there. At the time, the wrapper was a work in progress 
and most people were still ploughing on with C or C++ to do parallel programming with OpenCL. For those interested, one key
quality of Rust is that it can be seen as a successful attempt at simplifying the environment for writing the type of
low-level code which people have used C and C++ for over the last few decades. Maybe in some ways, it is a lanaguage
for today, built with the benefit of hindsight of other, older low-level languages.

## OpenCl
OpenCL is very good for doing parallel, recursive compute operations on graphics hardware, which often have thousands
of very small processors and banks of local memory at various group levels. OpenCL is used to do the 'raw maths'. 

When I first wrote a fractal explorer, it was done in Visual Basic. I then rewrote it in C# some years later. Both
versions would use the CPU and were very slow, taking a few seconds to generate a ~4Mb bitmap. When the program was
written using an OpenCL kernel and a modest dedicated GPU, it could generate the same image in the region of 400 times
faster than on the dual core CPU used previously. That equates to millions of iterations being completed for a fairly large
pixel map in just a few milliseconds.
The key to understanding why this type of implementation works so well for rendering images using the mandelbrot set is that
because the madlebrot algorithm is highly recursive and each recursion only requires a very small processor, you can spread
the load into 'work groups' over the hundreds or thousands of tiny processors available on modern GPU hardware. The kernel
code compiles to the GPU itself and a 'swap buffer', which passes the image map to the peripheral C++ code and on to the 
display only runs once each time a complete fractal is generated. The interactive 'SDL2' layer listens for keypress or
mouse events, allowing you to zoom in and out of a mandelbrot rendition.

The base program was orginally written and built using Code::Blocks IDE but the current version was written using 
Microsoft VS Code. The source codebase consists of only 2 files, 'main.cpp' and 'mandel.cl'. These are what are 
offered under the GNU license. All other information is offered only to help others to get an environment up and
running and people are advised to read about these parts themselves. All links and information are given to provide
a little help to get a fully functional, open source implementation of mandelbrot visualisations to render to your 
monitor. Please do refer to all 3rd party open source offerings for yourself, as links may cease to work and what 
little knowledge provided on the subject of parallel computing is very limited. The subject is huge and this code 
is offered only to help people to get started withoiut too much fuss. If that first step works, people are encouraged
to experiment and improve upon or alter the code as they see fit.

'main.cpp' is a standard C++ file and 'mandel.cl' is an OpenCL kernel format file, which can be modified by the user
using a standard text editor to alter fractal appearance and complexity. The kernel performs parallel, recursive 
iterations of a Mandelbrot algorithm. The kernel is written to the Grpahics hardware at compilation and returns complete
fractal RGBA pixel image maps at each rendition at runtime: on program start and at each click event (zoom in / zoom out) 
performed by the user using the SDL2 libraries referred to.

NB: The kernel ('mandel.cl') has a lot of room for improvement and is by no means the most efficient code to perform
recursive mandelbrot algorithm pixel colouring functions to produce image maps. The same goes for the interactive
navigation code in SDL2. The centering and zoom maths is very basic, probably overly complicated and inefficient. 
Any mathematician who is interested is encouraged to improve upon it. I would love to see it improved upon and extended.
What the software does provide is a tried and tested base from which fractal exploration can be done without having to
worry about setting up an environment and writing all the bolier-plate code from scratch. It took me months to get the
first basic OpenCL utilised version working many years ago and hopefully the code and info here can help people avoid
some of the hassle of going through that 'just to get started'. 

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
