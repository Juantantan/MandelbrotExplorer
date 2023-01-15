![Screenshot](/docs/images/rm-2.png)
# Mandelbrot explorer with OpenGL/OpenCL Interoperability
Mandelbrot explorer uses OpenGL/OpenCL interoperability to leverage the raw compute power of OpenCL
with the display capabilities of OpenGL. It renders colourful, detailed images from pixel maps formed by iterating through 
the Mandelbrot algorithm and 'shading' each pixel based on the number of iterations each pixel coordinate value in the 
imaginary plane takes to escape the mandelbrot set. You can zoom in and out with Mouse L + R. You can alter the 'kernel' file,
which is written to your Graphics hardware (GPU / CPU /...) during compile. User interaction (zooming) is done via the 
3rd party SDL2 library.

At the time of writing, there was little to no open source code available on the web which demonstrated OpenCL/OpenGL
interoperability. This is important because it's all very well producing an image map but displaying it to screen has
to be done to enjoy the image visually. There aren't very many OpenCL offerings when compared to OpenGL and very,
very few which combine the two APIs. These Interoperability offerings are just code snippets in reference articles.

## Mentions and thanks
### Eric Bainville
A few years after the VB and C# versions were written, I did a search on rendering the Mandelbrot set and how others had
done that. I stumbled upon the excellent and extensive work done by Eric Bainville. My initial implementation using OpenCL
was based on Eric's work. Without that, which formed the central core of the OpenCL part, I probably would not have done 
anything more. Please take the opportunity to see Eric's work. Eric has a web site here: http://www.bealto.com/cv.html

### Mark Brown
When I got a recent version of this software to run some years ago, I passed a copy to Mark and he was kind enough to go 
through it and offer advice. In particular, he tidied up my poorly executed OpenCL error-handling method and sent that back 
to me. He also commented on various aspects of what I was trying to do and opened me up to other languages and techniques,
such as functional programming. Mark has written web guides for using Erlang, OpenGL with Lisp, Ray-tracing, OpenSSH, 
encryption and security and on many other subjects. 

### Richard Garnett and Jack Costin
Richard and Jack have been colleagues of mine at my work. I owe then so much thanks for their patience when I was slowly 
learning Python and Postgre Sql in Linux. Their precision and thoroughness helped to teach me to bring my PR's up to an
acceptable quality.

## OpenCl
OpenCL is very good for doing parallel, recursive compute operations on graphics hardware. Modern GPUs often have thousands
of very small processors and banks of very fast, locally available memory at various grouping levels. OpenCL is used to
do the 'raw maths' part, and forms the core of this software. If you are using your CPU to run the recursive mandelbrot 
algorithm, you are limited to running only as many parallel instances as you can have parallel threads running on your CPU
cores. Becasue each recursion of the algorithm is simple and not CPU intensive at all, it doesn't require big processors.
GPU processors are ideal! It is interesting to see how CPUs now contain increasingly greater numbers of cores, each core
often having several effective cores within. The very latest CPUS are beginning to onclude different type of cores, some 
which are good for intensive single-thread operations and some which are good for simpler, more parallel workloads. With
those simpler, parallel cores, they are working more like the small processors of GPUS. GPUs were mainly used just for
Graphics but now they are used for all sorts of creative work as well. Some don't even provide a display output port.  

As mentioned previously, when I first wrote a fractal explorer, it was done in Visual Basic and then C#. Both versions 
would use the CPU and were very slow, taking a few seconds to generate a ~4Mb bitmap.
When the program was written using an OpenCL kernel and a modest dedicated GPU, it could generate the same image in the
region of 400 times faster than on the dual core CPU used. That equates to billions of algorithm operations being 
completed for a fairly large pixel map in just a few milliseconds. Interoperability with OpenGL allows the image maps to
be dsiplayed straight to a screen, not having to be written to an image file to be opened later.
To reiterate, the key to understanding why this type of implementation works so well for rendering images using the
mandelbrot set (or other simple, recursive algoritms), is that because the madlebrot algorithm is highly recursive and
each recursion only requires a very small processor, you can spread the load into 'work groups' over the thousands of 
tiny processors available on modern GPU hardware. The kernel code compiles to the GPU itself and a 'swap buffer', which
passes the image map to the peripheral C++ code and on to the display, only runs once each time a complete fractal is 
generated. OpenCL can be used for many parallel programming applications, especially those which are highly recursive,
such as software used to generate molecular sequences for finding possible new pharmaceutical drugs or for many 
thermodynamics applications. 

NB: OpenCL can also run with CPU vendor API implementations, including this one, if the vendor SDK is obtaine. It will
be interesting to see how fast this type of kernel can work with the latest types of CPU, with their increasing use of
more cores each capable of running more truly parallel threads.

## SDL2
The interactive 'SDL2' layer listens for keypress or mouse events, allowing you to zoom in and out of a mandelbrot
rendition. There are many other possible libraries which could be used. I used SDL because it is very easy to set up and
very simple to use to intercept realtime user events. I also used glfw but found that trickier, mainly in terms of header
file versioning, among other things. There is room for adaptation here and any lightweight open source API could be used
in theory. Any speed differences between these user interaction APIs is largely irrelevant because they will all be able
to respond to user events more-or-less 'instantly'. 

## Core Code
The base program was orginally written and built using Code::Blocks IDE but the current version was written using 
Microsoft VS Code. The source codebase consists of only 2 files, 'main.cpp' and 'mandel.cl'. These are what are 
offered under the GNU license. All other information is offered only to help others to get an environment up and
running and people are advised to read about these parts themselves. All links and information are given to provide
a little help to get a fully functional, open source implementation of mandelbrot visualisations to render to your 
monitor. Please do refer to all 3rd party open source offerings for yourself, as links may cease to work and what 
little knowledge provided on the subject of parallel computing is very limited. The subject is huge and this code 
is offered only to help people to get started without too much fuss. If that first step works, people are encouraged
to experiment and improve upon the code as they see fit. 

'main.cpp' is a standard C++ file and 'mandel.cl' is an OpenCL kernel format file, which can be modified by the user
using a standard text editor to alter rendering speed, fractal appearance and complexity. The kernel performs all the 
parallel, recursive iterations of the Mandelbrot algorithm. The kernel is written to the Grpahics hardware at compilation
time and returns complete fractal RGBA pixel image maps at each rendition at runtime: on program start and at each
mouse-click event (zoom in / zoom out) performed by the user, via the SDL2 API.

NB: The kernel ('mandel.cl') has a lot of room for improvisation and is by no means the most efficient code to perform
pixel colouring functions to produce image maps. In particular, the colour banding and blending is clunky and slower
than it could be. The same goes for the interactive navigation code in SDL2. The centering and zoom maths is very basic,
probably overly complicated and inefficient. Any mathematician who is interested is encouraged to improve upon it. 
I would love to see that part improved upon and simplified. Having lost my latest version (oops), it seems that this
SDL code doesn't even centre correctly, 

What the software does provide is a tried and tested base from which mandelbrot exploration can be done without having to
worry about setting up an environment and writing all the bolier-plate code from scratch. It took me months to get the
first basic OpenCL utilised version working many years ago, and hopefully the code and info here can help people avoid
some of the hassle of going through that, just to get started. 

## Vulkan
Vulkan is a new graphics api developed by AMD and Khronos Group. At the time of my early commits, my understanding is 
that parallel compute functionality was not fully mature in Vulkan. OpenCL might even still be the most direct and 
efficient api to use to do highly recursive parallel compute operations without having to write too much bolierplate code.
I may not attempt to rewrite this code using the Vulkan API but would be interested if others do. I have gone through an
excellnt Vulkan learning reference twice and still find it hard to understand at the lowest level when compared to OpenCL.
One great aspect of the Vulkan API, is that it has been adopted by the major GPU verndors and is as a result able to compete
with their other APIs or API wrappers. It is intersting to read about CUDA and DirectX, OpenCL and OpenGL and then to 
compare them with Vulkan across different types of implementations and different vendor hardware. Vulkan is increasingly 
being used for writing game graphics engines. The boilerplate code takes a long time to write but once done, it seems a 
very effifient API to use to write all sorts of graphical (and hopefully compute) intensive software.


The program could equally be run and compiled at the command line without an IDE but the extensions for VS Code make it
very easy to see what's going on during compilation using that, especillay when debugging. If running with the command
prompt, linker flags will need to be set correctly. On linux, the following should work. 
(You might have to preceed with the admin 'sudo' command, depending on your setup):

g++ main.cpp -Wall -lSDL2main -lSDL2 -lSDL2_image -lGL -lGLU -lglut -lOpenCL -o ../bin/main.out

If using VS Code, your project can include local config files in json format:
(launch.json/tasks.json/c_cpp_properties.json) and a VS Code workspace file. Basic versions are included, 
which work on this project in linux on my PC. 

## Compiler 
Here are some general instructions from the Visual Studio Code website for setting these up for your local
deployment using the GNU compiler / debugger (gdb/gcc) or CLang (probably best Mac and/or with OpenCL V 3.0):
* Windows (with MinGW/MinGW-w64 & gdb)  : https://code.visualstudio.com/docs/cpp/config-mingw
* Linux (with gdb)                      : https://code.visualstudio.com/docs/cpp/config-linux
* Mac/OSX (with CLang compiler)         : https://code.visualstudio.com/docs/cpp/config-clang-mac

## 3rd party requirements 
The project requires the following libraries/source/headers:
* OpenGL - Graphics rendering (may already be present by default on Linux and/or Mac OS)
* OpenCL - For Compute operations (may already be present by default on Linux and/or Mac OS)
* SDL2 - For user / screen interaction (requires runtime library install and header)
* SDL2_Image (optional) - For working with physical image files (requires header file include). 
  This is not strictly required unless physical images are being created from the raw image data
  sent to the screen in the base code.
  https://www.libsdl.org/
  (SDL2 is just one of several possible 3rd party interactive layers you could use. This code does 
  require it but could be modified to work with different user interaction libraries, such as glfw, as mentioned
  previously.
* glu.h - OpenGL helper header file. (Usually already present in the include/GL folder)
Installing these 3rd party dibraries will be different on different platforms. In Windows there is 
an SDK which can be installed but this may exist already on your machine in the other OS'. Of course, you can
often find the open source header files offered on their own or as part of other open source offerings.

### OpenCL notes
The OpenCL specification is managed by Khronos Group. Khronos.org is an organisation of representatives
from many interested parties. Their homepage is here: https://www.khronos.org/
The OpenCL registry homepage is here: https://www.khronos.org/registry/OpenCL/
You can also download the specifications for OpenCL versions from that page. I printed out the OpenCL 1.2 version
on nice photo quality glossy paper. Using these, it was not hard to implement different OpenCL API calls.
NB: It is worth noting that in 2020 the specification for OpenCL 3.0 came out. This project was written
in OpenCL 1.2, which is the core on which OpenCL 3.0 is based. This code has been tested and will run
using OpenCL 1.2 and / or with OpenCL 3.0 when using compliant GPU hardware if you wish and CLang compilers.
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


# OS specific environment setup notes

 ## WINDOWS (64-bit)
(For Win32 substitute MinGW for MinGW-64)
In windows, the project is easily developed using the MinGW-64 gcc runtime:
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
