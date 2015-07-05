Posixino
========

## What ##

Implementation of some Arduino libraries 
for POSIX systems (Linux, BSD, Mac OS X).

# Scope #

Check `test` folder, these are the official example scketches
which will work with Posixino.

The main focus is to implement network-related
libraries (web client, web server).

## Why ##

So, why should you compile Arduino
*sketches* for a non-embedded system?

- You can write unit tests
- You can debug the code
- You can develop and test off-board
- Shorter compile-run cycle
- More comfortable

## How ##

Simply replace Arduino-specific includes:

```
# ifdef __unix
# include "../src/posixino.hpp"
# else

#include <SPI.h>
#include <Ethernet.h>

# endif
```

Quick-and-dirty: you may include `posixino.cpp`, 
so you can compile your sketch without makefile.
