# Welcome to Ironic Library!
The code presented here is some pieces of my home projects, that might be helpful to others. Here you will find:
 - ir_md5 - MD5 hash algorithm
 - ir_openmap - helpful mapping function
 - ir_database - simple ultra-light databases
 - ir_neuro - ultra-light neural network
 - ir_plot - ultra-light plotting function
 - ir_resource - very helpful class wrapper aroud non-class things

### Platforms
The code is developed and tested mostly under Windows. Some of libraries were specially tested under Linux. Some are cross-platform by chance. Some need just several corrections and renamings to make them work on your platform. Some are really stuck to WinAPI and could be barely ported. It that case, I beg your pardon.
 
### How to install?
I wanted to make the installation really simple. All you need is to insert
```c
#define IR_IMPLEMENT
```
before all the libraries, but only in one file - if you do it twice, the compiler might detect multiple implementations. If you want to manage yourself, which files the compiler needs to compile - try using
```c
#define IR_ ## FILENAME_IN_UPPER_CASE ## _IMPLEMENT
```
It could be helpful if you build some libraries or compile using object files. Also it is preferable to make a separate file for implementation so that the compiler must not do the same job every time.

### How to use?
The code is pretty self-documented. Every header file contains one, less often several functions or classes, which purpose can be read even with basic programming skills. Well, at least I beleave it can. If something is not obvious, there is high possibility that it is explained in comments. Later I will consider to use some documentation system.

### Ideology
The library is written following some principles:
 - The library does not claim to be full or serious.
 - The library does not claim to be new. I am sure that it's functionality was already implemented numerous times.
 - The library does not use any interpretable languages, it is fully native.
 - The library is old-fashioned. You will see a lot of C-style typecasts but no *std* code.
 - There is no code in headers. Only definitions.

###### P.S. My code is not dirty, it is alternatively clean.