# include "posixino.hpp"


	Posixino posixino;


	int main() {
	
		atexit(cleanup);
		signal(SIGINT,quit);
		
		posixino.init();		
		
		setup();
		while(true) loop();		
	} // main()
	
	
	void quit(int sig) {
	
		cleanup();
		fprintf(stdout,"READY. \n");
		exit(0);
		
	} // quit()


	void cleanup() {
		posixino.cleanup();
	} // cleanup()
	

	void delay(int ms) { posixino.delay(ms); }
	int millis() { return posixino.millis(); }
	void pinMode(int pin,int mode) { posixino.pinMode(pin,mode); }
	void digitalWrite(int pin,int value) { posixino.digitalWrite(pin,value); }
	int analogRead(int pin) { return posixino.analogRead(pin); }

		
	void Posixino::init() {
	
		isDigitalOutsUsed = false;
		isDigitalOutsDisplayed = false;
		
		for (int n = 0; n < NO_OF_DIGI_OUTS; n++) {
			pinModeList[n] = -1;
			pinValueList[n] = -1;
		}
		
		startMillis = millisSinceEpoch();
		
		key = -1;
    tcgetattr(fileno(stdin),&orig_term_attr);
    memcpy(&new_term_attr,&orig_term_attr,sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin),TCSANOW,&new_term_attr);
		
	} // init()


	void Posixino::cleanup() {
    tcsetattr(fileno(stdin),TCSANOW,&orig_term_attr);
	} // cleanup()


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
	
	
	void Posixino::digitalWrite(int pin,int value) {

		if (pinModeList[pin] == -1) {
			printErrorPrefix();
			fprintf(stderr,"mode must be set first for pin %d \n",pin);
			exit(1);
		}
		
		if (pinModeList[pin] != OUTPUT) {
			printErrorPrefix();
			fprintf(stderr,"mode must be write for pin %d \n",pin);
			exit(1);
		}

		if ((value != LOW) && (value != HIGH)) {
			printErrorPrefix();
			fprintf(stderr,"invalid pin value %d for pin %d \n",value,pin);
			exit(1);
		}		
		
		isDigitalOutsUsed = true;
		if (pinValueList[pin] == value) return;
		pinValueList[pin] = value;
		renderDigitalOuts();
	
	} // digitalWrite()
	
	
	int Posixino::analogRead(int pin) {
	
		/// TODO or what
		
		return 42;	
	} // analogRead()
	
	
	void Posixino::printErrorPrefix() {
		printPrefix("error");
	} // printErrorPrefix()
	
	
	void Posixino::printPrefix(const char* str) {
		posixino.eraseDigitalOuts();
		fprintf(stderr,"POSIXINO %s: ",str);
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
	
	
	bool Posixino::isKeyAvailable() {
	
		if (key != -1) return true;
    key = fgetc(stdin);
    
		return ( key != -1 );
	} // isKeyAvailable()
	
	
	int Posixino::readKey() {
		
		int k = key;
		key = -1;

		return k;	
	} // readKey

	
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
	

	void SerialClass::printAtom(const char chr) {
	
		if (posx == 0) printf("SER|");
		
		printf("%c",chr);
		
		if (chr == 0x0a) {
			posx = 0;
		} else {
			posx++;
		}
		
	} // print(char)


	void SerialClass::printChar(const char chr,bool lf) {
	
		checkInitialization();
		posixino.eraseDigitalOuts();

		printAtom(chr);
		if (lf) printAtom('\n');
		
		posixino.restoreDigitalOuts();
		
	} // printChar()
	
	
	void SerialClass::printString(const char* str,bool lf) {

		checkInitialization();
		posixino.restoreDigitalOuts();

		int len = strlen(str);
		for (int n = 0; n < len; n++) printAtom(str[n]);
		if (lf) printAtom('\n');

		posixino.restoreDigitalOuts();
	
	} // printString()


	void SerialClass::printNum(int val,int radix,bool lf) {
		char str[20];
		
		switch (radix) {
			case BIN: {
				str[0] = 0;
				for (int z = 128; z > 0; z >>= 1) {
					strcat(str,( ((val & z) == z) ? "1" : "0" ));
				}
			break; }
			case OCT: {
				sprintf(str,"%o",val);
			break; }
			case HEX: {
				sprintf(str,"%X",val);
			break; }
			default: {
				sprintf(str,"%d",val);
			}
		} // switch

		printString(str,lf);
		
	} // print()


	void SerialClass::print(const char chr) {
		printChar(chr,false);
	} // print(char)


	void SerialClass::print(const char* str) {
		printString(str,false);
	} // print(char*)


	void SerialClass::println() {
		printChar('\n',false);
	} // println()


	void SerialClass::println(const char* str) {
		printString(str,true);
	} // println(char*)
	
	
	void SerialClass::print(int val) {
		printNum(val,DEC,false);
	} // print(int)


	void SerialClass::println(int val) {
		printNum(val,DEC,true);
	} // println(int)
	

	void SerialClass::print(int val,int radix) {
		printNum(val,radix,false);
	} // print(int,int)

	
	void SerialClass::println(int val,int radix) {
		printNum(val,radix,true);
	} // println(int,int)


	void SerialClass::write(char chr) {
		printChar(chr,false);
	} // write()
	
	
	int SerialClass::available() {
		return ( posixino.isKeyAvailable() ? 1 : 0 );
	} // available()
	
	
	char SerialClass::read() {
		return posixino.readKey();
	} // read()
	

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
	

	void LiquidCrystal::writeChar(char chr) {
		screenBuffer[(y * 3 * w) + w + x] = chr;
		x++;		
	} // writeChar()
	
	
	
	void LiquidCrystal::begin(int pw,int ph) {	
		
		isInitialized = true;
		firstClear = true;
		
		w = pw;
		h = ph;
		
		int screenSize = w * h;
		bufferSize = 3 * screenSize;
		screenBuffer = (unsigned char*)malloc(bufferSize);
		if (screenBuffer == NULL) posixino.outOfMem();
		lastScreen = (unsigned char*)malloc(screenSize);
		if (lastScreen == NULL) posixino.outOfMem();

		clear();
		
	} // begin()

	
	void LiquidCrystal::clear() {

		memset(screenBuffer,0x20,bufferSize);
		memset(lastScreen,0x20,screenSize);					
		x = 0;
		y = 0;		
	
		if (isChanged() || firstClear) renderScreen();
		firstClear = false;
		
	} // clear()
	

	void LiquidCrystal::setCursor(int px,int py) {
		x = px;
		y = py;
	} // setCursor()
	
	
	void LiquidCrystal::write(char chr) {
		writeChar(chr);
		if (isChanged()) renderScreen();		
	} // write()
	
	
	void LiquidCrystal::print(const char* str) {
	
		int len = strlen(str);
		for (int n = 0; n < len; n++) writeChar(str[n]);

		if (isChanged()) renderScreen();
		
	} // print()
	
	
	void LiquidCrystal::print(int v) {
	
		char str[20];
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
	

	char* EthernetClass::localIP() {
		static char res[40];
		strcpy(res,"(n.a.)");

		struct ifaddrs* ifAddrStruct = NULL;
		struct ifaddrs* ifa = NULL;
    void* tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);
		for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		
			if (ifa ->ifa_addr->sa_family == AF_INET) {
				char mask[INET_ADDRSTRLEN];
				void* mask_ptr = &((struct sockaddr_in*) ifa->ifa_netmask)->sin_addr;
				inet_ntop(AF_INET,mask_ptr,mask,INET_ADDRSTRLEN);
				if (strcmp(mask,"255.0.0.0") == 0) continue;
				
				//printf("mask:%s\n", mask);
				tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
				char addressBuffer[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, tmpAddrPtr,addressBuffer,INET_ADDRSTRLEN);
				//printf("%s IP Address %s\n",ifa->ifa_name,addressBuffer);
				strcpy(res,addressBuffer);
				break;
			} // if inet4
			
		} // foreach
	    
		if (ifAddrStruct != NULL) freeifaddrs(ifAddrStruct);	
		
		return res;
	} // localIP()


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
	

	void EthernetClient::printAtom(const char* data,int len) {
	
		int x = send(fd,data,len,MSG_NOSIGNAL);
		if (x == -1) {
			if (errno == EPIPE) return;
			//if (errno == ECONNRESET) then what. nothing.
			stop();
		}
		
	} // printAtom()


	void EthernetClient::print(int value) {
		char data[40];
		sprintf(data,"%d",value);
		printAtom(data,strlen(data));
	} // print(int)


	void EthernetClient::print(const char* str) {
		printAtom(str,strlen(str));
	} // print(const char*)
	

	void EthernetClient::println() {
		printAtom("\n",1);
	} // println(void)

	
	void EthernetClient::println(const char* str) {
		
		int len = 1 + strlen(str);
		char* data = (char*)malloc(len);
		strcpy(data,str);
		data[len - 1] = '\n';
		data[len] = '\0';
		
		printAtom(data,len);
		
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
	

	EthernetClient::operator bool() const {
	
		/// TODO
		
		return false;
	} // operator bool
	
	
	
	EthernetServer::EthernetServer(int p) {
	
		port = p;
		if (port < 1024) port += 8000;
		
	} // EthernetServer()


	void EthernetServer::begin() {
	
	} // begin()


	EthernetClient EthernetServer::available() {
	
		EthernetClient ec;
		return ec;
	} // available()
