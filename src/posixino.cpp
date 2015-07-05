# include "posixino.hpp"


	Posixino posixino;


	int main() {
		posixino.init();		
		setup();
		while(true) loop();		
	} // main()


	void delay(int ms) { posixino.delay(ms); }
	int millis() { return posixino.millis(); }
	void pinMode(int no,int mode) { posixino.pinMode(no,mode); }
	void digitalWrite(int no,int value) { posixino.digitalWrite(no,value); }

		
	void Posixino::init() {
	
		isDigitalOutsUsed = false;
		isDigitalOutsDisplayed = false;
		
		for (int n = 0; n < NO_OF_DIGI_OUTS; n++) {
			pinModeList[n] = -1;
			pinValueList[n] = -1;
		}
		
	} // init()


	void Posixino::delay(int ms) {
		usleep(1000 * ms);
	} // delay()


	int Posixino::millis() {
	
		/// TODO
		
		return 0;
	} // millis()
	

	void Posixino::pinMode(int no,int mode) {		
	
		if (no >= NO_OF_DIGI_OUTS) {
			printErrorPrefix();
			fprintf(stderr,"pin number too high %d \n",no);
			exit(1);
		}
		
		pinModeList[no] = mode;		
		
	} // pinMode()
	
	
	void Posixino::digitalWrite(int no,int value) {

		if (pinModeList[no] == -1) {
			printErrorPrefix();
			fprintf(stderr,"mode must be set first for pin %d \n",no);
			exit(1);
		}
		
		if (pinModeList[no] != OUTPUT) {
			printErrorPrefix();
			fprintf(stderr,"mode must be write for pin %d \n",no);
			exit(1);
		}

		if ((value != LOW) && (value != HIGH)) {
			printErrorPrefix();
			fprintf(stderr,"invalid pin value %d for pin %d \n",value,no);
			exit(1);
		}		
		
		isDigitalOutsUsed = true;
		if (pinValueList[no] == value) return;
		pinValueList[no] = value;
		updateDigitalOuts();
	
	} // digitalWrite()
	
	
	void Posixino::printErrorPrefix() {
		posixino.eraseDigitalOuts();
		fprintf(stderr,"POSIXINO error: ");
	} // printErrorPrefix()
	
	
	void Posixino::updateDigitalOuts() {
	
		if (!isDigitalOutsUsed) return;

		fprintf(stderr,"\r");
		for (int n = 0; n < NO_OF_DIGI_OUTS; n++) {
			fprintf(stderr,"%c",( pinValueList[n] == HIGH ? 'X' : '.' ));
		}
		fprintf(stderr," ");
		
		isDigitalOutsDisplayed = true;
	
	} // updateDigitalOuts()
	
	
	void Posixino::eraseDigitalOuts() {	
		if (isDigitalOutsDisplayed) fprintf(stderr,"\r%c[K",27);
	} // eraseDigitalOuts()
	
	
	void Posixino::restoreDigitalOuts() {
		if (isDigitalOutsDisplayed) updateDigitalOuts();
	} // restoreDigitalOuts()
	
	
	SerialClass Serial;
	
	
	SerialClass::SerialClass() {
		isInitialized = false;
	} // SerialClass() ctor
	
	
	bool operator !(SerialClass ser) {
		return false;
	}	// operator !(SerialClass)


	void SerialClass::checkInitialization() {
	
		if (isInitialized) return;
		
		posixino.printErrorPrefix();
		fprintf(stderr,"serial port must be initialized first \n");
		exit(1);
		
	} // checkInitialization()
	
	
	void SerialClass::begin(int speed) {	
		isInitialized = true;
	} // begin()
	

	void SerialClass::print(const char chr) {
		checkInitialization();
		posixino.eraseDigitalOuts();
		printf("%c",chr);
	} // print(char)


	void SerialClass::print(const char* str) {
		checkInitialization();
		posixino.eraseDigitalOuts();
		printf("%s",str);
	} // print()


	void SerialClass::println() {
		checkInitialization();
		posixino.eraseDigitalOuts();
		println("");
	} // println()


	void SerialClass::println(const char* str) {
		checkInitialization();
		posixino.eraseDigitalOuts();
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
		/// TODO
	} // println(char*)


	bool EthernetClient::connected() {
		/// TODO
	} // connected()
	
	
	bool EthernetClient::available() {
		/// TODO
	} // available()
	
	
	char EthernetClient::read() {
		/// TODO
		
		return 'x';
	} // read()
	
	
	void EthernetClient::stop() {
	
		/// TODO
		
	} // stop()


	LiquidCrystal::LiquidCrystal(int p1,int p2,int p3,int p4,int p5,int p6) {
		// this method is officially left blank		
	} // LiquidCrystalClass() ctor


	void LiquidCrystal::checkInitialization() {
	
		if (isInitialized) return;
		
		posixino.printErrorPrefix();
		fprintf(stderr,"LCD must be initialized first \n");
		exit(1);
		
	} // checkInitialization()
	
	
	void LiquidCrystal::begin(int pw,int ph) {	
		
		isInitialized = true;
		
		w = pw;
		h = ph;
		x = 0;
		y = 0;		
		
	} // begin()


	void LiquidCrystal::setCursor(int px,int py) {
		x = px;
		y = py;
	} // setCursor)_
	
	
	void LiquidCrystal::print(const char* str) {
	
		/// TODO
		
	} // print()
	
	
	void LiquidCrystal::print(int v) {
	
		char str[80];
		sprintf(str,"%d",v);
		print(str);
		
	} // print()
