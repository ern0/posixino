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
		
		startMillis = millisSinceEpoch();
		
	} // init()


	void Posixino::outOfMem() {
		printErrorPrefix();
		fprintf(stderr,"out of memory \n");
		exit(1);
	} // outOfMem()
	

	void Posixino::delay(int ms) {
		usleep(1000 * ms);
	} // delay()


	unsigned long long Posixino::millisSinceEpoch() {
	
		struct timeval tv;
		gettimeofday(&tv,NULL);

		unsigned long long millisecondsSinceEpoch =
			(unsigned long long)(tv.tv_sec) * 1000 +
			(unsigned long long)(tv.tv_usec) / 1000;

		return millisecondsSinceEpoch;
	} // millisSinceEpoch()


	int Posixino::millis() {
	
		unsigned long long now = millisSinceEpoch();
		int result = (int)(now - startMillis);
		
		return result;
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
		renderDigitalOuts();
	
	} // digitalWrite()
	
	
	void Posixino::printErrorPrefix() {
		posixino.eraseDigitalOuts();
		fprintf(stderr,"POSIXINO error: ");
	} // printErrorPrefix()
	
	
	void Posixino::renderDigitalOuts() {
	
		if (!isDigitalOutsUsed) return;

		fprintf(stderr,"\rDIG|");
		for (int n = 0; n < NO_OF_DIGI_OUTS; n++) {
			fprintf(stderr,"%c",( pinValueList[n] == HIGH ? 'X' : '.' ));
		}
		fprintf(stderr," ");
		
		isDigitalOutsDisplayed = true;
	
	} // renderDigitalOuts()
	
	
	void Posixino::eraseDigitalOuts() {	
		if (isDigitalOutsDisplayed) fprintf(stderr,"\r%c[K",27);
	} // eraseDigitalOuts()
	
	
	void Posixino::restoreDigitalOuts() {
		if (isDigitalOutsDisplayed) renderDigitalOuts();
	} // restoreDigitalOuts()
	
	
	SerialClass Serial;
	
	
	SerialClass::SerialClass() {
		isInitialized = false;
		posx = 0;
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
	

	void SerialClass::printChar(const char chr) {
	
		if (posx == 0) printf("SER|");
		
		printf("%c",chr);
		
		if (chr == 0x0a) {
			posx = 0;
		} else {
			posx++;
		}
		
	} // print(char)


	void SerialClass::print(const char chr) {
		checkInitialization();
		posixino.eraseDigitalOuts();
		printChar(chr);
	} // print()


	void SerialClass::print(const char* str) {
		checkInitialization();
		posixino.eraseDigitalOuts();
		int len = strlen(str);
		for (int n = 0; n < len; n++) printChar(str[n]);
	} // print()


	void SerialClass::println() {
		checkInitialization();
		posixino.eraseDigitalOuts();
		printChar('\n');
	} // println()


	void SerialClass::println(const char* str) {
		checkInitialization();
		posixino.eraseDigitalOuts();
		print(str);
		printChar('\n');
	} // println(char*)
	

	LiquidCrystal::LiquidCrystal(int p1,int p2,int p3,int p4,int p5,int p6) {
		// this method is officially left blank		
	} // LiquidCrystalClass() ctor


	void LiquidCrystal::checkInitialization() {
	
		if (isInitialized) return;
		
		posixino.printErrorPrefix();
		fprintf(stderr,"LCD must be initialized first \n");
		exit(1);
		
	} // checkInitialization()
	
	
	bool LiquidCrystal::isChanged() {

		for (int row = 0; row < h; row++) {
			for (int col = 0; col < w; col++) {
				if (screenBuffer[(row * 3 * w) + w + col] == lastScreen[row * w + col]) continue;
				return true;
			}
		}
		
		return false;
	} // isChanged()
	
	
	void LiquidCrystal::renderScreen() {

		posixino.eraseDigitalOuts();
		
		printf("    ");
		for (int n = 0; n < w; n++) printf("_");
		printf("\n");
		
		for (int row = 0; row < h; row++) {
			printf("LCD|");
			for (int col = 0; col < w; col++) {
				int c = screenBuffer[(row * 3 * w + w) + col];
				if (c < 0x20) c = '?';				
				printf("%c",c);
				lastScreen[row * w + col] = c;
			}
			printf("|\n");
		}
		
		posixino.renderDigitalOuts();
	
	} // renderScreen()
	
	
	void LiquidCrystal::begin(int pw,int ph) {	
		
		isInitialized = true;
		
		w = pw;
		h = ph;
		
		int screenSize = w * h;
		bufferSize = 3 * screenSize;
		screenBuffer = (unsigned char*)malloc(bufferSize);
		if (screenBuffer == NULL) posixino.outOfMem();
		memset(screenBuffer,0x20,bufferSize);
		lastScreen = (unsigned char*)malloc(screenSize);
		if (lastScreen == NULL) posixino.outOfMem();
		memset(lastScreen,0x20,screenSize);		
		
		x = 0;
		y = 0;		
		
	} // begin()


	void LiquidCrystal::setCursor(int px,int py) {
		x = px;
		y = py;
	} // setCursor()
	
	
	void LiquidCrystal::print(const char* str) {
	
		int len = strlen(str);
		for (int n = 0; n < len; n++) {
			screenBuffer[(y * 3 * w) + w + x] = str[n];
			x++;
		}

		if (isChanged()) renderScreen();
		
	} // print()
	
	
	void LiquidCrystal::print(int v) {
	
		char str[80];
		sprintf(str,"%d",v);
		print(str);
		
	} // print()
	
	
	void LiquidCrystal::scrollDisplayLeft() {
	
		for (int n = 1; n < bufferSize; n++) {
			screenBuffer[n - 1] = screenBuffer[n];
		} 		
		for (int row = 0; row < h; row++) {
			screenBuffer[row * 3 * w] = ' ';
		}
		
		if (isChanged()) renderScreen();
	
	} // scrollDisplayLeft()


	void LiquidCrystal::scrollDisplayRight() {
	
		for (int n = bufferSize; n > 0; --n) {
			screenBuffer[n] = screenBuffer[n - 1];
		} 
		for (int row = 0; row < h; row++) {
			screenBuffer[(row * 3 * w) + (3 * w) - 1] = ' ';
		}
		
		if (isChanged()) renderScreen();
	
	} // scrollDisplayLeft()
	

	EthernetClass Ethernet;
	
	
	bool EthernetClass::begin(byte mac[6]) {		
		return 1;
	} // begin()


	bool EthernetClass::begin(byte mac[6],IPAddress& ip) {
		// this method is officially left blank		
	} // begin()
	

	IPAddress::IPAddress(unsigned char pa,unsigned char pb,unsigned char pc,unsigned char pd) {
		sprintf(address,"%d.%d.%d.%d",pa,pb,pc,pd);
	} // IPAddress(unsigned char,unsigned char,unsigned char,unsigned char) ctor
	
	
	IPAddress::IPAddress(char* host) {
		strncpy(address,host,80);
	} // IPAddress(char*) ctor
	
	
	char* IPAddress::getAddress() {
		return address;
	} // getAddress()


	EthernetClient::EthernetClient() {
		fd = -1;
	} // EthernetClient() ctor
	

	bool EthernetClient::connect(const char* host,int port) {

		struct hostent* he = gethostbyname(host);
		if (he == NULL) return false;

		struct in_addr** al;
		al = (struct in_addr**) he->h_addr_list;		
		
		IPAddress a(inet_ntoa(*al[0]));
		return connect(a,port);
	} // connect(char*,...)
	
	
	bool EthernetClient::connect(IPAddress& host,int port) {

		fd = socket(AF_INET,SOCK_STREAM,0);
		if (fd == -1) return false;

		memset(&servaddr,0,sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr(host.getAddress());
		servaddr.sin_port = htons(port);

		int res = ::connect(fd,(struct sockaddr*)&servaddr,sizeof(servaddr));	
		
		return ( res != -1 );
	} // connect(IPAddress&,...)
	

	void EthernetClient::println() {
		println("");
	} // println()

	
	void EthernetClient::println(const char* str) {
		
		int len = 1 + strlen(str);
		char* data = (char*)malloc(len);

		strcpy(data,str);
		data[len - 1] = '\n';
		
		int x = send(fd,data,len,MSG_NOSIGNAL);
		if (x == -1) {
			if (errno == EPIPE) return;
			if (errno == ECONNRESET);
			stop();
			return;
		}

		free(data);
		
	} // println(char*)


	bool EthernetClient::connected() {
		return ( fd != -1 );
	} // connected()
	
	
	bool EthernetClient::available() {
		char data[1];

		int x = recv(fd,data,1,MSG_DONTWAIT | MSG_PEEK);		
		if (x == 1) return true;

		if (x == -1) {
			if (errno == EAGAIN) return false;
			if (errno == ECONNRESET) return false;
		}
		
		stop();
		return false;
	} // available()
	
	
	char EthernetClient::read() {
	
		char data[1];	
		int x = recv(fd,data,1,0);

		if (x != 1) {
			stop();
			return '?';
		}
		
		char r = data[0];
		return r;
	} // read()
	
	
	void EthernetClient::stop() {	
		close(fd);
		fd = -1;		
	} // stop()
