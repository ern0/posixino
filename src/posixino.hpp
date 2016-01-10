# ifndef _posixino
# define _posixino


# define NO_OF_DIGI_OUTS (20)

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <ifaddrs.h>
# include <netdb.h>
# include <signal.h>
# include <termios.h>

# include <string>
# include <thread>
# include <mutex>

# ifdef SDL_DISPLAY
# include<SDL2/SDL.h>
# endif

using namespace std;


// Mac OS X hack

# ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL (0)
# endif


// Unix functions

int main();
void quit(int sig);
void cleanup();


// Arduino entry points

void setup();
void loop();


// Timer interrupts macro and stubs 

# define ISR(vector) void vector()

void TIMER0_COMPA_vect();
# ifdef TIMER0
# define __TIMER_USED
# else
ISR(TIMER0_COMPA_vect) { }
# endif

void TIMER1_COMPA_vect();
# ifdef TIMER1
# define __TIMER_USED
# else
ISR(TIMER1_COMPA_vect) { }
# endif

void TIMER2_COMPA_vect();
# ifdef TIMER2
# define __TIMER_USED
# else
ISR(TIMER2_COMPA_vect) { }
# endif

# ifdef __TIMER_USED
# if __cplusplus <= 199711L
# error Timer interrupt emulation requires std::thread, use -std=c++11 compiler option
# endif
# endif


// Timer interrupts globals and constats

int TCCR0A = -1;
int TCCR0B = -1;
int TCNT0  = -1;
int OCR0A = -1;
int TIMSK0 = -1;

# define WGM01 (0)
# define CS00 (0)
# define CS01 (0)
# define OCIE0A (0)
		
int TCCR1A = -1;
int TCCR1B = -1;
int TCNT1  = -1;
int OCR1A = -1;
int TIMSK1 = -1;

# define WGM12 (0)
# define CS10 (0)
# define CS11 (0)
# define CS12 (0)
# define	OCIE1A (0)

int TCCR2A = -1;
int TCCR2B = -1;
int TCNT2  = -1;
int OCR2A = -1;
int TIMSK2 = -1;

# define WGM21 (0)
# define CS21 (0)
# define	OCIE2A (0)

# define CS22 (0)

void callTimerThread();
	

// Arduino global functions

void delay(int ms);
void pinMode(int pin,int mode);
int digitalRead(int pin);
void digitalWrite(int pin,int value);
int analogRead(int pin);
void analogWrite(int pin,int value);
int millis();
void cli();
void sei();


// Arduino types and constants

# define INPUT 0
# define INPUT_PULLUP 0
# define OUTPUT 1
# define LOW 0
# define HIGH 1

typedef unsigned char byte;
typedef bool boolean;
typedef string String;

# define HEX 16
# define DEC 10
# define OCT 8
# define BIN 2


// Implementation

class SerialClass;
class LiquidCrystal;
class EthernetServer;
class Adafruit_NeoPixel;

class Posixino {

	friend SerialClass;
	friend LiquidCrystal;
	friend EthernetServer;
	friend Adafruit_NeoPixel;

	private:
    struct termios orig_term_attr;
    struct termios new_term_attr;	
		int pinModeList[NO_OF_DIGI_OUTS];
		int pinValueList[NO_OF_DIGI_OUTS];
		bool isDigitalOutsUsed;
		bool isDigitalOutsDisplayed;
		std::mutex digitalOutMutex;
		unsigned long long startMillis;
		int key;
		int interruptTiming[3];
		int interruptCounter[3];

	protected:
		void printErrorPrefix();
		void printPrefix(const char* str);
		void printPrefix();
		void fatal(const char* msg,int errorCode);
		unsigned long long millisSinceEpoch();
		void renderDigitalOuts();
		void eraseDigitalOuts();
		void restoreDigitalOuts();
		void setupTimerInterrupt(int num,int force,int a,int b);
	public:
		bool isKeyAvailable();
		int readKey();
		void setupTimerIntervals();
		void startTimerThread();
		void timerThread();

	public:
		void init();
		void cleanup();
		void delay(int ms);
		int millis();
		void pinMode(int no,int mode);
		int digitalRead(int pin);
		void digitalWrite(int pin,int value);		
		int analogRead(int pin);
		void analogWrite(int pin,int value);		
		void sei();
		void cli();

}; // Posixino


class SerialClass {

	private:
		bool initialized;
		int posx;
	protected:
		void checkInitialization();
		void printAtom(const char chr);
		void printChar(const char chr,bool lf);
		void printString(const char* str,bool lf);
		void printNum(int val,int radix,bool lf);
	public:
		SerialClass();
		void begin(int speed);
		void print(int val);
		void println(int val);
		void print(int val,int radix);
		void println(int val,int radix);
		void print(const char chr);
		void print(const char* str);
		void println();
		void println(const char* str);
		void write(char chr);
		int available();
		char read();

}; // SerialClass


class LiquidCrystal {

	private:
		bool initialized;
		int w;
		int h;
		int x;
		int y;
		int bufferSize;
		unsigned char* screenBuffer;
		int screenSize;
		unsigned char* lastScreen;
		bool firstClear;
	protected:
		void checkInitialization();
		void writeChar(char chr);
		bool isChanged();
		void renderScreen();
	public:
		LiquidCrystal(int p1,int p2,int p3,int p4,int p5, int p6);
		void begin(int w,int h);
		void clear();
		void setCursor(int x,int y);
		void write(char chr);
		void print(const char* str);
		void print(int v);
		void scrollDisplayLeft();
		void scrollDisplayRight();

}; // class LiquidCrystal


class IPAddress;

class EthernetClass {

	private:
		char localAddress[INET_ADDRSTRLEN];
	public:
		bool begin(byte mac[6]);
		bool begin(byte mac[6],IPAddress& ip);
		bool begin(byte mac[6],byte ip[4]);
		char* localIP();
	
}; // class EthernetClass


class IPAddress {

	private:
		char address[80];

	public:
		char* getAddress();

	public:
		IPAddress(char* host);
		IPAddress(unsigned char pa,unsigned char pb,unsigned char pc,unsigned char pd);
		
		
}; // class IPAddress


class EthernetClient {

	friend EthernetServer;

	private:
		EthernetServer* server;
		int fd;
		struct sockaddr_in servaddr;
		bool firstDataReceived;
	protected:
		void printAtom(const char* data,int len);
		void connectedByServer(EthernetServer* server,int fd,int dr);
	public:
		EthernetClient();
		bool connect(const char* host,int port);
		bool connect(IPAddress& host,int port);
		void print(int value);
		void print(const char* str);
		void print(String s);
		void println();
		void println(const char* str);
		void println(String s);
		bool connected();
		bool available();
		char read();
		void stop();
		operator bool() const;

}; // class EthernetClient



class EthernetServer {

	friend EthernetClient;

	private:
		bool initialized;
		int fd;
		struct sockaddr_in servaddr;		
		int devicePort;
		int port;
		EthernetClient client;	
	  socklen_t clilen;
	  struct sockaddr_in cliaddr;
	  int clientFd;
	  bool clientFirstDataReceived;

	protected:
		void checkInitialization();
		void listen();
		void accept();
		void clientDisconnect();
	
	public:
		EthernetServer(int port);
		void begin();
		EthernetClient available();

}; // class EthernetServer


# ifdef SDL_DISPLAY
class EmuPixel {

	public:
		int parmX;
		int parmY;
		int parmWidth;
		int parmHeight;
		int parmGapHoriz;
		int parmGapVert;
		int x;
		int y;
		int width;
		int height;

	public:
		static void setGridScreenAnchor(const char* anchor);
		static void setGridScreenPercent(int height);
		static void setGridCells(int w,int h);

	public:
		EmuPixel();
		void setPos(int x,int y);
		void setSize(int w,int h);
		void setGap(int h,int v);

}; // class EmuPixel
# endif


class Adafruit_NeoPixel {

 	# define NEO_KHZ800 (0)
	# define NEO_KHZ400 (0)
	# define NEO_GRB (0)
	# define NEO_RGB (0)
	
	private:
		int numberOfPixels;
		uint32_t* pixels;
		
	public:
		Adafruit_NeoPixel(int numberOfPixels,int pin, int flags);
		int numPixels();
		void begin();
		void setBrightness(int v);
		uint32_t Color(int r,int g,int b);
		void setPixelColor(int numero,uint32_t color);
		void setPixelColor(int numero,int r,int g,int b);
		void show();

	private:
		int smallestError;
		int biggestError;

	# ifdef SDL_DISPLAY

	private:
		bool sdlInitialized;
		SDL_Window* window;
		SDL_Surface* screenSurface;
		EmuPixel* emuPixels;

		const char* gridAnchor;
		int gridCellCols;
		int gridCellRows;
		int gridPercent;
		SDL_DisplayMode current;
		int windowPosX;
		int windowPosY;
		int windowWidth;
		int windowHeight;		
		int cellWidth;
		int cellHeight;

	protected:
		void initializeSdl();
		void calcDims();
		void quitOnKey();
		void emuCheckPixelIndex(int i);
		void emuUpdatePixel(int i);

	public:
		void emuSetGridCells(int cols,int rows);
		void emuSetGridScreenAnchor(const char* a);
		void emuSetGridScreenPercent(int p);
		void emuSetPixelPos(int i,int cellX,int cellY);
		void emuSetPixelCellSize(int i,int cellWidth,int cellHeight);
		void emuSetPixelPixGap(int i,int pixelVertical,int pixelHorizontal);

	# endif
	
}; // class Adafruit_NeoPixel


# endif
