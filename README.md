# Welcome to Ironic Library!
The code presented here is some pieces of my home projects. Here you will find:
 - ir_database		- ultra-light databases
 - ir_neuro			- neural network
 - ir_resource		- helpful class wrappers around non-class things (RAII)
 - ir_parallel		- parallel calculations, kind of OpenMP replacement
 - ir_utf			- encoding library
 - ir_container		- library's containers: vectors, blocks, registers, rings, strings
 - ir_math			- some mathematics algorithms
 - ir_md5			- MD5 hash algorithm
 - ir_openmap		- mapping function
 - ir_plot			- plotting function

### Platforms
The code is developed and tested mostly under Windows x86. Most of libraries were specially tested under Linux x86. Some are cross-platform by chance. Some need just several corrections and renamings to make them work on your platform. Some are really stuck to WinAPI, some may crash on ARM, POWER, SPARC etc. In that case, I beg your pardon.
 
### How to install?
I wanted to make the installation really simple. The simplest way to install the library is to choose one of `.cpp` files, define `IR_IMPLEMENT` and include all libraries you are interested in. After doing so you can use headers in every file like header-only libraries.
```c++
#define IR_IMPLEMENT
#include <ir_EXAMPLE.h>
```

Include principles can be more flexible, but you need to understand how they work. `IR_IMPLEMENT` makes the compiler to include all implementations, `IR_EXAMPLE_IMPLEMENT` makes it include specific implementation, `IR_EXAMPLE_NOT_IMPLEMENT` makes it exclude one. With this mechanism you can control compiler options which you compile Ironic code with and solve linker errors. For clarity, typical Ironic header looks like this:
```c++
//ir_example.h
int example();
#if (defined(IR_IMPLEMENT) || defined(IR_EXAMPLE_IMPLEMENT)) && !defined(IR_EXAMPLE_NOT_IMPLEMENT)
	#include <implementation/ir_example_implementation.h>
#endif
```

Templates are quite non-ironic-way thing, and there are difficulties. The Ironic way of using templates is (on example of `ir::Vector`):
```c++
#define IR_IMPLEMENT
#include <ir_vector.h>
template class ir::Vector<float>;
```

After doing so you can use your `ir::Vector<float>` in every file. But actually you can define `IR_VECTOR_IMPELMENT` and include `ir_vector.h` in every file, and use `ir::Vector` with any template parameters (almost like you do with STL). The compiler is smart enough and does not complain about multiple implementations. But doing so will increase the compile time.

### How to get help?
The code is pretty self-documented. But more importantly, I provide [Doxygen](https://www.doxygen.nl/manual/starting.html) documentation! It does not look too pretty since I am not an expert, but it is still quite informative. I would recommend to start with **Modules** page. And of course, feel free to contact me!

### About Natvis
For some classes I provide [Natvis](https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects) files! Include these files to your Visual Studio project and enjoy debugging.

###### P.S. My code is not dirty, it is alternatively clean.