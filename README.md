Posixino
========

### What ###

Implementation of some Arduino libraries 
for POSIX systems (Linux, BSD, Mac OS X).

Check `test/` folder, these are the official example scketches
which will work with Posixino.

### Why ###

So, why should you compile Arduino
*sketches* for a non-embedded system?

- You can write unit tests
- You can debug the code
- You can develop and test off-board
- Shorter compile-run cycle
- More comfortable

But remember, I don't want to to throw your devices away,
developing for embedded platforms is **fun**!

### How ###

Simply replace Arduino-specific includes:

```
# ifdef __unix
# include "posixino/posixino.hpp"
# else

#include <SPI.h>
#include <Ethernet.h>

# endif
```

Quick-and-dirty: you may include `posixino.cpp`, 
so you can compile your sketch without makefile.

### Now ###

The library is in early stage:

- `Serial.print` works,
- `digitalWrite` works, output states are displayed on console.

Stubs created for `WebClient`.

### Plans ###

The main focus is to implement network-related
libraries (web client, web server).

Maybe I'll implement some kind of LCD display emulation.

One day in the future, 
I will split these big files one-class-per-pair-of-files fashion,
and contribution will be easier.
