# Welcome to Ironic Library!

### Contents
1. [Welcome to ironic Library](#welcome-to-ironic-library)
2. [Contents](#contents)
3. [Overview](#overview)
4. [Installation](#installation)
5. [Platforms](#platforms)
6. [Natvis](#natvis)
7. [Documentation](#documentation)

### Overview
Here is a brief but full overview of features of the library:
 - Containers: `block.h`, `map.h`, `quiet_hash_map.h`, `quiet_list.h`, `quiet_map.h`, `quiet_ring.h`, `quiet_vector.h`, `string.h`, `vector.h`
 - Definitions: `constants.h`, `types.h`
 - Encoding library: `encoding.h`
 - Mathematics: `fft.h`, `gauss.h`
 - File utilities: `file.h`, `mapping.h`
 - Hash algorithms: `fnv1a.h`, `md5.h`
 - Networking: `ip.h`, `tcp.h`, `udp.h`
 - Databases: `n2st_database.h`, `s2st_database.h`
 - Neuronal networks: `neuro.h`
 - High-performance computing: `parallel.h`, `matrix.h`
 - RAII wrapper: `resource.h`
 - Source and sink abstractions: `sink.h`, `source.h`
 - Cross-platform versions of standard C functions: `str.h`, `print.h`
 
### Installation
Installation was designed to be super-easy and super-flexible at the same time.

Easy way: templates and inlines will work right out of the box. To use non-template and non-inline methods, include `ironic.cpp` to your project.

Flexible way: there is a way to reduce compile time and compile parts Ironic library with different options, but for this a little theory is needed. The code is divided into three groups:
 - Inline \- it's implementation shall always be present in each `.cpp` file.
 - Template \- it's implementation shall always be present **and used** in one or more `.cpp` files. Note that `template class ir::EXAMPLE<int>` is also an "usage" and will force the compiler to compile all methods of `EXAMPLE<int>` class.
 - Non-inline and non-template \- it's implementation shell always be present in one `.cpp` file.

So the modes of compilation and correspondent macros are:
 - `#define IR_INCLUDE 'n'` will include no implementations. The compiler might complain about `inline function is not implemented` or equivalent warning.
 - `#define IR_INCLUDE 'i'` will include only implementation of inline functions.
 - `#define IR_INCLUDE 't'` will include implementation of inline and template functions (default behavior).
 - `#define IR_INCLUDE 'a'` will include include all implementations.

Also if you define `IR_EXCLUDE_%EXAMPLE%` the `%EXAMPLE%` implementation will be not included. But if you define `IR_EXCLUDE`, the compiler will include **only** implementations marked with `IR_INLCUDE_%EXAMPLE%`.

### Platforms
The code is tested for Windows x86 and x64, Linux x64 and Linux ARMv7.

### Natvis
For some classes [Natvis](https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects) files are provided. Include these files to your Visual Studio project and enjoy debugging.

### Documentation
[Doxygen](https://www.doxygen.nl/manual/starting.html) documentation is provided. I would recommend to start with **Modules** page.

###### P.S. My code is not dirty, it is alternatively clean.