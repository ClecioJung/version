# version

## Overview

Simple library for parsing, comparing and checking compatibility of semantic versions implemented in C. It posseses the following characteristics:

- Compatible with C99;
- It is a [Header Only Library](https://github.com/nothings/stb);
- It was tested only on Linux (compatible with GNU GCC or clang);
- It doesn't use dynamic allocation of memory;

## Example

A complete example of usage can be found in [./example.c](./example.c).
To compile and run the example, follow these steps:

- Download this project
- Navigate to its folder
- Run the make command to compile the examples:

```console
$ make
gcc -pedantic -W -Wall -Wextra -Wconversion -Wswitch-enum -Werror -std=c99 -O0 -g example.c -o example
```

- Just run the example (it sorts an array of semantic versions and check it against specifications):

```console
$ ./example
Version             =1.2.3         =1.2.3-rc.2    >1.2.3         ^1.2.3         ~1.2.3         ~1.2.3-rc.2    @1.2.3-rc.2    
1.0                 false          false          false          false          false          false          false          
1.1.6+8             false          false          false          false          false          false          false          
1.1.6               false          false          false          false          false          false          false          
1.2-alpha           false          false          false          false          false          false          false          
1.2+8               false          false          false          false          false          false          false          
1.2                 false          false          false          false          false          false          false          
1.2.3-alpha         false          false          false          false          false          false          false          
1.2.3-alpha.1       false          false          false          false          false          false          false          
1.2.3-alpha.beta    false          false          false          false          false          false          false          
1.2.3-beta          false          false          false          false          false          false          false          
1.2.3-beta          false          false          false          false          false          false          false          
1.2.3-beta+2        false          false          false          false          false          false          false          
1.2.3-rc.1          false          false          false          false          false          false          false          
1.2.3-rc.2          false          true           false          false          false          true           true           
1.2.3-rc.9          false          false          false          false          false          true           true           
1.2.3-rc.10         false          false          false          false          false          true           true           
1.2.3-rc.11         false          false          false          false          false          true           true           
1.2.3-rc.11.test    false          false          false          false          false          true           true           
1.2.3-rc.test       false          false          false          false          false          true           true           
1.2.3               true           false          true           true           true           true           true           
1.2.4               false          false          true           true           true           true           false          
1.3.0               false          false          true           true           false          false          false          
1.3.4               false          false          true           true           false          false          false          
1.5.0.rc.1          false          false          true           true           false          false          false          
2.0.0.rc.1          false          false          true           false          false          false          false          
2                   false          false          true           false          false          false          false          
3.0.0               false          false          true           false          false          false          false          
4                   false          false          true           false          false          false          false 
```

Feel free to reach out to us if you need further assistance or have any questions. Enjoy using the library!
