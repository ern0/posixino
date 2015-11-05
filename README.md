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

Interrupt emulation is not 100% accurate yet,
three timer interrupt routines can be set up
which will be called at specified intervals.

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

### Timer interrupts ###

To activate a timer, define value of
`TIMER0`, `TIMER1` or `TIMER2`
to required interval (microsec, 1/1000 ms),
`ISR(TIMER0_COMPA_vect)`,
`ISR(TIMER1_COMPA_vect)` or
`ISR(TIMER2_COMPA_vect)` functions
will be called, respectively.
`ISR_TIMERx_OVF_vect` is not supported yet.

Non-matching `TIMERx` define - `ISR` function pairs
will occur compile-time error.

```
# if ( defined(__unix__) || defined(__APPLE__) )
# define TIMER1 (1000 * 1000)
# include "posixino/posixino.hpp"
# endif

	void setup() {
	
		cli();
		
		//set timer1 interrupt at 1Hz
		TCCR1A = 0; // set entire TCCR1A register to 0
		TCCR1B = 0; // same for TCCR1B
		TCNT1  = 0; // initialize counter value to 0
		// set compare match register for 1hz increments
		OCR1A = 15624; // = (16*10^6) / (1*1024) - 1 (must be <65536)
		// turn on CTC mode
		TCCR1B |= (1 << WGM12);
		// Set CS12 and CS10 bits for 1024 prescaler
		TCCR1B |= (1 << CS12) | (1 << CS10);  
		// enable timer compare interrupt
		TIMSK1 |= (1 << OCIE1A);
		
		sei();
		
	} // setup
	
	ISR(TIMER1_COMPA_vect) {
	
		// blink the led, whatever
		
	} // timer1 interrupt

```

All the register names and 
`cli()/sti()` functions are definied by Posixino,
but has no effect at all. 
Future versions may calculate timer interval values
based on register values set,
if `TIMERx` are set to invalid value (e.g. -1).

Timer interrupt emulation uses C++11 features
(thread, mutex), 
you may use `--std=c++11` compiler flag.

The emulated timer interrupt resolution is now 1 ms,
which is 1000 times longer than the real interrupts.
Don't forget, timer interrupt emulation will
be never as accurate as real timer interrupts.

#### Adafruit NeoPixel LED strip ####

Re-implemented Adafruit's LED strip/ring/whatever interface.
If `SDL_DISPLAY` is defined 
(use 0 for single monitor systems), 
a borderless black window will appear on the screen
with bars, which represents the leds.

The size and padding of the bars can be defined,
as well as window's gravity,
but all parameters have carefully selected 
default values,
so only `SDL_DISPLAY` should be defined
for displaying LED emulation.

The following parameters can be given:
```
	# define LED_WIDTH (24)
	# define GAP_WIDTH (4)

	# define LED_HEIGHT (24)
	# define GAP_HEIGHT (4)

	// 0:left 1:center 2:right
	# define WINDOW_X_POS (2)

	// 0:top 1:middle 2:bottom
	# define WINDOW_Y_POS (2)
```

When the number of LEDs is a square number,
they will be arranged into a square shape.

### Plans ###

- BSD support (a BSD buddy should help)
- Split the code to smaller units
- Direct UI for inputs
- Data generator for inputs
- Set timer interrupt intervals 
  based on register values
