Posixino
========

### About ###

Posixino is the re-implementation 
of the most important Arduino libraries for
POSIX systems (Linux, BSD, Mac OS X).
It makes possible to compile and run
Arduino sketches on desktop computers
without modifications.

### Focus ###

The main focus is to implement network-related
functions (web client, web server),
and basics (digital inputs and outputs, LCD).
Check library and function list for coverage. 

Posixino is designed to produce CLI programs;
serial output, digital out states, LCD display content etc.
will appear on the console.

### Benefits ###

But why should you compile Arduino
sketches for a non-embedded system?

- You can write unit tests
- You can debug your code
- You can work even without proper hardware (e.g. missing shield)
- Shorter compile-run cycle
- More comfortable

Of course, I don't want you to throw your device away,
embedded programming is fun and joy. 

Posixino is trying to mimic the behavior of the original libraries,
but there is a good chance that something will work different way,
so testing your program on real hardware is important.

### Demo (kinda) ###

Here's the output of the 
`LiquidCrystal/HelloWorld.cpp` example sketch:
    
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

### How to use ###

Simply add Posixino header include to your program:

```
# ifdef __unix
# include "posixino/posixino.hpp"
# else

#include <SPI.h>
#include <Ethernet.h>

# endif
```
Compile both Posixino and your program,
then link them together.

Quick-and-dirty: you may include `posixino.cpp`, 
so you will compile your sketch with Posixino as a single program.

### Coverage ###

Check `test/` folder, there are the official example scketches
which will work on your Unix system.

The Posixino framework is in early stage:

- `Serial.print` works.
- `digitalWrite` works, output states are displayed on console.
- `LiquidCrystal` works, 
LCD display content is rendered on the screen upon each change.

Method stubs created for `WebClient` 
(it will be compiled, but does not work). 

Posixino is only tested on Linux.
