Posixino
========

### About ###

Posixino is a framework for 
POSIX systems (Linux, BSD, Mac OS X),
which makes possible to compile and run
Arduino sketches on desktop computers.

The most important Arduino libraries are 
reimplemented for Unix systems,
check library and function list below 
for the complete list. 
The main focus is to implement network-related
libraries (web client, web server),
and essentials (digital inputs and outputs, LCD).

The li




Check `test/` folder, there are the official example scketches
which will work on your Linux  Posixino.

### Why ###

So, why should you compile Arduino
*sketches* for a non-embedded system?

- You can write unit tests
- You can debug the code
- You can develop without proper device (e.g. missing LCD)
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
- `LiquidCrystal` works, LCD display content is rendered on the screen upon each change

Stubs created for `WebClient`. 

### Example ###

Here's the output of the `LiquidCrystal/HelloWorld.cpp` example sketch:
    
```
    ________________
LCD|hello, world!   |
LCD|                |
    ________________
LCD|hello, world!   |
LCD|0               |
    ________________
LCD|hello, world!   |
LCD|1               |
    ________________
LCD|hello, world!   |
LCD|2               |
```


### Plans ###



One day in the future, 
I will split these big files one-class-per-pair-of-files fashion,
and contribution will be easier.
