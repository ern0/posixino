# ifndef _posixino
# define _posixino


# define NO_OF_DIGI_OUTS (20)


# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <sys/time.h>


// Unix entry point
int main();

// Arduino entry points
void setup();
void loop();

// Arduino global functions
void delay(int ms);
void pinMode(int no,int mode);
void digitalWrite(int no,int value);
int millis();


// Arduino types and constants

# define INPUT 0
# define OUTPUT 1
# define LOW 0
# define HIGH 1

typedef unsigned char byte;


// Implementation

class SerialClass;
class LiquidCrystal;

class Posixino {

	friend SerialClass;
	friend LiquidCrystal;

	private:
		int pinModeList[NO_OF_DIGI_OUTS];
		int pinValueList[NO_OF_DIGI_OUTS];
		bool isDigitalOutsUsed;
		bool isDigitalOutsDisplayed;
		unsigned long long startMillis;

	protected:
		void outOfMem();
		unsigned long long millisSinceEpoch();
		void printErrorPrefix();
		void renderDigitalOuts();
		void eraseDigitalOuts();
		void restoreDigitalOuts();

	public:
		void init();
		void delay(int ms);
		int millis();
		void pinMode(int no,int mode);
		void digitalWrite(int no,int value);		

}; // Posixino


class SerialClass {

	private:
		bool isInitialized;
		int posx;
	protected:
		void checkInitialization();
		void printChar(const char chr);
	public:
		SerialClass();
		void begin(int speed);
		void print(const char chr);
		void print(const char* str);
		void println();
		void println(const char* str);

}; // SerialClass


class IPAddress {

	private:
		int a;
		int b;
		int c;
		int d;

	public:
		IPAddress(int pa,int pb,int pc,int pd);
		
		
}; // class IPAddress


class EthernetClient {

	public:
		bool connect(const char* host,int port);
		void println();
		void println(const char* str);
		bool connected();
		bool available();
		char read();
		void stop();

}; // class EthernetClient


class EthernetClass {

	public:
		bool begin(byte mac[6]);
		bool begin(byte mac[6],IPAddress& ip);
	
}; // class EthernetClass


class LiquidCrystal {

	private:
		bool isInitialized;
		int w;
		int h;
		int x;
		int y;
		int screenSize;
		unsigned char* lastScreen;
		unsigned char* actualScreen;
	protected:
		void checkInitialization();
		bool isChanged();
		void renderScreen();
	public:
		LiquidCrystal(int p1,int p2,int p3,int p4,int p5, int p6);
		void begin(int w,int h);
		void setCursor(int x,int y);
		void print(const char* str);
		void print(int v);

}; // class LiquidCrystal

# endif
