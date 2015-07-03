# ifndef _posixino
# define _posixino

# include <stdio.h>
# include <unistd.h>


typedef unsigned char byte;

extern void setup();
extern void loop();


extern int main();
extern void delay(int ms);


class SerialClass {

	public:
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
