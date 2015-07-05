# ifndef _posixino
# define _posixino

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>


// Unix entry point
int main();

// Arduino entry points
void setup();
void loop();

// Arduino global functions
void delay(int ms);
void pinMode(int no,int mode);
void digitalWrite(int no,int value);


// Arduino types and constants

# define INPUT 0
# define OUTPUT 1
# define LOW 0
# define HIGH 1

typedef unsigned char byte;


// Implementation

class SerialClass;

class Posixino {

	friend SerialClass;

	private:
		# define NO_OF_DIGI_OUTS (32)
		int pinModeList[NO_OF_DIGI_OUTS];
		int pinValueList[NO_OF_DIGI_OUTS];
		bool isDigitalOutsUsed;
		bool isDigitalOutsDisplayed;

	protected:
		void printErrorPrefix();
		void updateDigitalOuts();
		void eraseDigitalOuts();
		void restoreDigitalOuts();

	public:
		void init();
		void delay(int ms);
		void pinMode(int no,int mode);
		void digitalWrite(int no,int value);

}; // Posixino


class SerialClass {

	private:
		bool isInitialized;
	protected:
		void checkInitialization();
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


# endif
