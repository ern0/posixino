Posixino
========

### About ###

Posixino is the re-implementation 
of the most important Arduino libraries for
POSIX systems (Linux, BSD and Mac OS X).
It makes possible to compile and run
Arduino sketches on desktop computers
without modifications.

### Focus ###

The main focus is to implement network-related
functions (web client, web server),
and basics (digital inputs and outputs, serial, LCD).
Check "coverage" section for actual state. 

Posixino is designed to produce CLI programs;
serial output, digital out states, LCD display content etc.
will appear on the console.

There is no complex embedded software without interrupts,
so timer interrupts will be emulated,
at least an interrupt routine will be called
at the specified interval.

### Benefits ###

But why should you compile Arduino
sketches for a non-embedded system?

- You can write unit tests
- You can debug your code
- You can work even without proper hardware (e.g. missing shield)
- Shorter compile-run cycle
- More comfortable

Of course, I don't want you to throw your device away,
embedded programming is not a fun without any device.

Posixino is trying to mimic the behavior of the original libraries,
but there is a good chance that something will work different way,
so testing your program on real hardware is still important. 
If you have no device, but you need exact simulation of it,
using emulator is a better choice.

### Demo (kinda) ###

Here's the output of the 
`LiquidCrystal/HelloWorld.cpp` example sketch
(it prints a static message and a counter):
    
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

Simply change add conditional Posixino header include to your program
(fun fact: there is no common pre-defined macro on Linux and OS X, e.g.
`__posix`):

```
# if ( defined(__unix__) || defined(__APPLE__) )
# include "posixino/posixino.hpp"
# else

#include <SPI.h>
#include <Ethernet.h>

# endif
```
Compile both Posixino and your program,
then link them together.

Quick-and-dirty: you may include `posixino.cpp`, 
so you will compile your sketch and Posixino as a single program.

### Coverage ###

Posixino is in early stage,
but most of 
digital out, web client and LCD 
functions are working.
Timer interrupts are not implemented yet.

As we're working on the framework,
we will add factory examples one by one
to the `test/` folder,
then implement library functions they're using.

Here is the actual list of working examples:

- `Communication/ASCIITable` (serial print)
- `Basics/Blink` (digital out)
- `Ethernet/WebClient` (HTTP request)
- `Ethernet/WebServer` (HTTP listener)
- `LiquidCrystal/HelloWorld` (LCD test)
- `LiquidCrystal/Scroll` (LCD scroll)
- `LiquidCrystal/SerialDisplay` (serial input)

Minor Ethernet server concept issue implemented:
`server.available()` returns a `client` object, 
which is `true` only when the first data arrives from the client
(should be `true` just as the client connects).
This also means that you can't even detect a client connection
which does not send any data 
(as far as I can remember this is a lwIP behavior,
not Arduino-specific).

Minor `String` support added by using standard library's `string` class.
Now strings can be concatenated using the `+` operator,
we're happy to support Java-to-C++ switchers.

Posixino is developed on Linux,
but releases are tested on Mac OS X as well
(but not on BSD).

### Plans ###

- BSD support (a BSD buddy should help)
- Some kind of interrupt emulation
- Split the code to smaller units
- Direct UI for inputs
- Data generator for inputs
