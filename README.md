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
so testing your program on real hardware is important. 
If you have no device, but you need exact simulation of it,
using emulator is a better choice.

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

Posixino is in early stage,
but most of 
digital out, web client and LCD 
functions are working.

As I'm working on the framework,
I will add factory examples one by one
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

Minor Ethernet server concept bug implemented:
`server.available()` returns a `client` object, 
which is `true` only when the first data arrives 
(should be `true` when a client connects).

Some `String` support added by using standard library's `string` class.
Now strings can be concatenated using the `+` operator.

Posixino is now tested on Linux only,
but it will be tested on Mac OS X before major releases.
