# Welcome to Ironic Library!
The code presented here is some pieces of my home projects. Here you will find:
 - ir_database	- ultra-light databases
 - ir_neuro		- neural network
 - ir_resource	- helpful class wrappers around non-class things (RAII)
 - ir_utf		- encoding library
 - ir_vector	- library's vector
 - ir_math		- some math algorithms
 - ir_md5		- MD5 hash algorithm
 - ir_openmap	- mapping function
 - ir_plot		- plotting function

### Platforms
The code is developed and tested mostly under Windows. Some of libraries were specially tested under Linux. Some are cross-platform by chance. Some need just several corrections and renamings to make them work on your platform. Some are really stuck to WinAPI and could be barely ported. It that case, I beg your pardon.
 
### How to install?
I wanted to make the installation really simple. The simplest way to install the library is to choose one of .cpp files and paste 
```c++
#define IR_IMPLEMENT
#include <ir_NAME.h>
```

If you want to manage yourself, which files to compile (for example, you want to compile one part as debug, one as release), you can also use these symbols:
```c++
#define IR_NAME_IMPLEMENT
#define IR_NAME_NOT_IMPLEMENT
```

Templates are quite non-ironic-way thing, and there are difficulties. The Ironic way of using templates is (on example of vector):
```c++
#define IR_IMPLEMENT
#include <ir_vector.h>
template class ir::vector<float>;
```

And then you can use your `ir::vector\<float\>` in every file. But actually you can just include the header in every file, the compiler is smart enough and does not complain about multiple implementations. But doing so will increase the compile time.

### How to get help?
The code is pretty self-documented. Every header file (without \_implementation suffix) contains one, less often several functions or classes, which purpose can be read even with basic programming skills. Well, at least I believe it can. If something is not obvious, there is high possibility that it is explained in comments. Later I will consider to use wiki or some documentation system. And feel free to contact me!

### About Natvis
For some classes I provide Natvis files! Include theese files to your Visual Studio project and enjoy debugging. [More](https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects).

###### P.S. My code is not dirty, it is alternatively clean.