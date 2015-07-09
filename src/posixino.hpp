# ifndef _posixino
# define _posixino


# define NO_OF_DIGI_OUTS (20)


# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>

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


class LiquidCrystal {

	private:
		bool isInitialized;
		int w;
		int h;
		int x;
		int y;
		int bufferSize;
		unsigned char* screenBuffer;
		int screenSize;
		unsigned char* lastScreen;
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
		void scrollDisplayLeft();
		void scrollDisplayRight();

}; // class LiquidCrystal


class IPAddress;

class EthernetClass {

	public:
		bool begin(byte mac[6]);
		bool begin(byte mac[6],IPAddress& ip);
	
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

	private:
		int fd;
		struct sockaddr_in servaddr;
	public:
		EthernetClient();
		bool connect(const char* host,int port);
		bool connect(IPAddress& host,int port);
		void println();
		void println(const char* str);
		bool connected();
		bool available();
		char read();
		void stop();

}; // class EthernetClient


# endif
