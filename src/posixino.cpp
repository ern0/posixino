# include "posixino.hpp"


	int main() {
		setup();
		while(true) loop();
	} // main()
	

	void delay(int ms) {
		usleep(1000 * ms);
	} // delay()
	

	SerialClass Serial;
	
	
	bool operator !(SerialClass ser) {
		return false;
	}	// operator !(SerialClass)


	void SerialClass::begin(int speed) {	
		// this method is officially left blank	
	} // begin()
	

	void SerialClass::print(const char chr) {
		printf("%c",chr);
	} // print(char)


	void SerialClass::print(const char* str) {
		printf("%s",str);
	} // print()


	void SerialClass::println() {
		println("");
	} // println()


	void SerialClass::println(const char* str) {
		printf("%s\n",str);
	} // println(char*)
	

	EthernetClass Ethernet;
	
	
	bool EthernetClass::begin(byte mac[6]) {
		// this method is officially left blank		
	} // begin()


	bool EthernetClass::begin(byte mac[6],IPAddress& ip) {
		// this method is officially left blank		
	} // begin()
	

	IPAddress::IPAddress(int pa,int pb,int pc,int pd) {
		a = pa;
		b = pb;
		c = pc;
		d = pd;
	} // IPAddress() ctor


	bool EthernetClient::connect(const char* host,int port) {
	
		/// TODO
		
		return false;
	} // connect()
	

	void EthernetClient::println() {
		println("");
	} // println()

	
	void EthernetClient::println(const char* str) {
		// TODO
	} // println(char*)


	bool EthernetClient::connected() {
		// TODO
	} // connected()
	
	
	bool EthernetClient::available() {
		// TODO
	} // available()
	
	
	char EthernetClient::read() {
		// TODO
		
		return 'x';
	} // read()
	
	
	void EthernetClient::stop() {
	
		// TODO
		
	} // stop()
