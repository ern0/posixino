# include "posixino.hpp"


// ----[ Set up Unix environment ]-------------------------------------

	Posixino posixino;


	int main() {

		atexit(cleanup);
		signal(SIGINT,quit);
		
		posixino.init();
		setup();
		# ifdef __TIMER_USED
			posixino.setupTimerIntervals();
			posixino.startTimerThread();
		# endif
		while(true) {
			loop();		
			delay(10);
		}
	} // main()
	
	
	void quit(int sig) {
	
		cleanup();
		usleep(500);
		fprintf(stdout,"READY. \n");
		exit(0);
		
	} // quit()


	void cleanup() {
		posixino.cleanup();
	} // cleanup()
	

// ----[ Global API functions mappings to the singleton ]--------------


	void delay(int ms) { posixino.delay(ms); }
	int millis() { return posixino.millis(); }
	void pinMode(int pin,int mode) { posixino.pinMode(pin,mode); }
	void digitalWrite(int pin,int value) { posixino.digitalWrite(pin,value); }
	int digitalRead(int pin) { return posixino.digitalRead(pin); }
	void analogWrite(int pin,int value) { posixino.analogWrite(pin,value); }
	int analogRead(int pin) { return posixino.analogRead(pin); }
	void cli() { posixino.cli(); }
	void sei() { posixino.sei(); }

	void callTimerThread() { posixino.timerThread(); }
	

// ----[ Singleton environment ]---------------------------------------


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
    # ifdef SDL_DISPLAY
    SDL_Quit();
    # endif
	} // cleanup()


	void Posixino::printErrorPrefix() {
		printPrefix(" error");
	} // printErrorPrefix()
	
	
	void Posixino::printPrefix(const char* str) {
		posixino.eraseDigitalOuts();
		fprintf(stderr,"POSIXINO%s: ",str);
	} // printErrorPrefix(char*)
	
	
	void Posixino::printPrefix() {
		printPrefix("");
	} // printPrefix(void)


	void Posixino::fatal(const char* str,int exitCode) {
	
		printErrorPrefix();
		fprintf(stderr,"%s \n",str);
		exit(exitCode);
		
	} // fatal()
	

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


// ----[ delay() implementation ]--------------------------------------


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
	

// ----[ Digital and analog I/O ]--------------------------------------


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
	
	
	int Posixino::digitalRead(int pin) {
		return pinValueList[pin];
	} // digitalRead()
	
	
	
	void Posixino::analogWrite(int pin,int value) {
		/// TODO or what
	} // analogWrite()
	
	
	int Posixino::analogRead(int pin) {
		/// TODO or what
		return 42;	
	} // analogRead()
	
	
	void Posixino::renderDigitalOuts() {
	
		if (!isDigitalOutsUsed) return;

		digitalOutMutex.lock();
		
		fprintf(stderr,"\rDIG|");
		for (int n = 0; n < NO_OF_DIGI_OUTS; n++) {
			fprintf(stderr,"%c",( pinValueList[n] == HIGH ? 'X' : '.' ));
		}
		fprintf(stderr," ");		
		isDigitalOutsDisplayed = true;

		digitalOutMutex.unlock();
	
	} // renderDigitalOuts()
	
	
	void Posixino::eraseDigitalOuts() {	
		if (isDigitalOutsDisplayed) fprintf(stderr,"\r%c[K",27);
	} // eraseDigitalOuts()
	
	
	void Posixino::restoreDigitalOuts() {
		if (isDigitalOutsDisplayed) renderDigitalOuts();
	} // restoreDigitalOuts()
	
	
// ----[ Serial ]------------------------------------------------------
	
	SerialClass Serial;
	
	
	SerialClass::SerialClass() {
		initialized = false;
		posx = 0;
	} // SerialClass() ctor
	
	
	bool operator !(SerialClass ser) {
		return false;
	}	// operator !(SerialClass)


	void SerialClass::checkInitialization() {
	
		if (initialized) return;
		
		posixino.printErrorPrefix();
		fprintf(stderr,"serial port must be initialized first \n");
		exit(1);
		
	} // checkInitialization()
	
	
	void SerialClass::begin(int speed) {	
		initialized = true;
	} // begin()
	

	void SerialClass::printAtom(const char chr) {
	
		if (posx == 0) printf("SER|");
		
		printf("%c",chr);
		fflush(stdout);
		
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
		posixino.eraseDigitalOuts();

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
	
	
// ----[ LiquidCrystal ]-----------------------------------------------	


	LiquidCrystal::LiquidCrystal(int p1,int p2,int p3,int p4,int p5,int p6) {
		// this method is officially left blank		
	} // LiquidCrystalClass() ctor


	void LiquidCrystal::checkInitialization() {
	
		if (initialized) return;
		
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
		
		initialized = true;
		firstClear = true;
		
		w = pw;
		h = ph;
		
		int screenSize = w * h;
		bufferSize = 3 * screenSize;
		screenBuffer = (unsigned char*)malloc(bufferSize);
		lastScreen = (unsigned char*)malloc(screenSize);
		if ((screenBuffer == NULL) || (lastScreen == NULL)) {
			posixino.fatal("out of memory",1); // implicite exit()			
		}

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
	

// ----[ Ehternet ]----------------------------------------------------

	EthernetClass Ethernet;
	
	
	bool EthernetClass::begin(byte mac[6]) {		
		localAddress[0] = '\0';
		return true;
	} // begin()


	bool EthernetClass::begin(byte mac[6],IPAddress& ip) {
		// this method is officially left blank		
		return true;
	} // begin()


	bool EthernetClass::begin(byte mac[6],byte ip[4]) {
		// this method is officially left blank	
		return true;	
	} // begin()
	

	char* EthernetClass::localIP() {
		strcpy(localAddress,"(n.a.)");

		struct ifaddrs* ifAddrStruct = NULL;
		struct ifaddrs* ifa = NULL;
    void* tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);
		for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {		
			if (ifa ->ifa_addr->sa_family != AF_INET) continue;
			
			char mask[INET_ADDRSTRLEN];
			void* mask_ptr = &((struct sockaddr_in*) ifa->ifa_netmask)->sin_addr;
			inet_ntop(AF_INET,mask_ptr,mask,INET_ADDRSTRLEN);
			if (strcmp(mask,"255.0.0.0") == 0) continue;
				
			//printf("mask:%s\n", mask);
			tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET,tmpAddrPtr,localAddress,INET_ADDRSTRLEN);
			//printf("%s IP Address %s\n",ifa->ifa_name,addressBuffer);
			break;
				
		} // foreach
	    
		if (ifAddrStruct != NULL) freeifaddrs(ifAddrStruct);	
		
		return localAddress;
	} // localIP()


// ----[ IPAddress ] --------------------------------------------------


	IPAddress::IPAddress(unsigned char pa,unsigned char pb,unsigned char pc,unsigned char pd) {
		sprintf(address,"%d.%d.%d.%d",pa,pb,pc,pd);
	} // IPAddress(unsigned char,unsigned char,unsigned char,unsigned char) ctor
	
	
	IPAddress::IPAddress(char* host) {
		strncpy(address,host,80);
	} // IPAddress(char*) ctor
	
	
	char* IPAddress::getAddress() {
		return address;
	} // getAddress()


// ----[ EthernetClient ] ---------------------------------------------


	EthernetClient::EthernetClient() {
		server = NULL;
		fd = -1;
		firstDataReceived = false;
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


	void EthernetClient::connectedByServer(EthernetServer* s,int f,int d) {
		server = s;
		fd = f;
		firstDataReceived = d;
	} // connectedByServer()
	

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


	void EthernetClient::print(String s) {
		print(s.c_str());
	} // print(String)
	

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
		

	void EthernetClient::println(String s) {
		println(s.c_str());
	} // println(String)
	

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
		
		if (server == NULL) {
			close(fd);
			fd = -1;		
			return;
		}
			
		server->clientDisconnect();
		fd = -1;
		firstDataReceived = false;
		
	} // stop()
	

	EthernetClient::operator bool() const {	
		if (server == NULL) return true;		
		return firstDataReceived;
	} // operator bool
	

// ----[ EthernetServer ] ---------------------------------------------
	
	
	EthernetServer::EthernetServer(int p) {
	
		initialized = false;
		fd = -1;
		clientFd = -1;
		clientFirstDataReceived = false;
	
		port = p;
		devicePort = p;
		if (port < 1024) port += 8000;
		
	} // EthernetServer()


	void EthernetServer::begin() {
		
		initialized = true;

		posixino.printPrefix();		
		fprintf(stderr,"listening on port %d ",port);
		if (devicePort != port) fprintf(stderr,"(was: %d)",devicePort);
		fprintf(stderr,"\n");
		
	} // begin()


	void EthernetServer::checkInitialization() {
	
		if (initialized) return;
		
		posixino.printErrorPrefix();
		fprintf(stderr,"ethernet must be initialized first \n");
		exit(1);
		
	} // checkInitialization()
	
	
	void EthernetServer::listen() {	

	  fd = socket(AF_INET,SOCK_STREAM,0);
	  if (fd == -1) {
			posixino.printErrorPrefix();
			perror("failed opening socket");
			exit(1);
		} // if socket open fail

		int flags = fcntl(fd,F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(fd,F_SETFL,flags);

		memset(&servaddr,0,sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = INADDR_ANY;
		servaddr.sin_port = htons(port);

		int yes = 1;
		setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));

		int retry = 0;
		while (true) {

			if (::bind(fd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1) {

				if (errno == EADDRINUSE) {
					if (retry == 0) {
						posixino.printPrefix();
						fprintf(stderr,": bind failed, retrying");
					} else {
						fprintf(stderr,".");
					}
					retry++;
					sleep(2);
					continue;
				} // if recoverable error

				fprintf(stderr,"\n");
				posixino.printErrorPrefix();
				perror("error on binding");
				exit(1);

			} // if bind failed
			
			else break;

		} // while forever
		
		if (retry > 0) fprintf(stderr,"succeed \n");

		if (::listen(fd,5) == -1) {
			posixino.printErrorPrefix();
			perror("listen");
			exit(1);
		}	
		
	} // listen()


	void EthernetServer::accept() {

		clientFd = ::accept(fd,(struct sockaddr*)&cliaddr,&clilen);
		clientFirstDataReceived = false;

	} // accept()
	
	
	EthernetClient EthernetServer::available() {
		checkInitialization();

		if (fd == -1) listen();
		if (clientFd == -1) accept();
		
		if (!clientFirstDataReceived) {
			char data[1];
			int x = recv(clientFd,data,1,MSG_DONTWAIT | MSG_PEEK);		
			if (x == 1) clientFirstDataReceived = true;		
		}

		client.connectedByServer(this,clientFd,clientFirstDataReceived);
	
		return client;
	} // available()


	void EthernetServer::clientDisconnect() {

		shutdown(clientFd,SHUT_RDWR);
		close(clientFd);
		clientFd = -1;
		clientFirstDataReceived = false;
	
	} // clientDisconnect()


// ----[ Interrupts ]--------------------------------------------------


	void Posixino::cli() {
		// this method is officially left blank
	} // cli()


	void Posixino::sei() {
		setupTimerIntervals();
	} // sei()
	
	
	void Posixino::setupTimerIntervals() {
	
		interruptCounter[0] = -1;
		interruptCounter[1] = -1;
		interruptCounter[2] = -1;
	
		# ifdef TIMER0
			do {
				if ((TCCR0A == -1) || (TCCR0B == -1)) fatal("timer0 not set",2);
				setupTimerInterrupt(0,TIMER0,TCCR0A,TCCR0B);
			} while (false);
		# endif

		# ifdef TIMER1
			do {
				if ((TCCR1A == -1) || (TCCR1B == -1)) fatal("timer1 not set",2);
				setupTimerInterrupt(1,TIMER1,TCCR1A,TCCR1B);
			} while (false);
		# endif

		# ifdef TIMER2
			do {
				if ((TCCR2A == -1) || (TCCR2B == -1)) fatal("timer2 not set",2);
				setupTimerInterrupt(2,TIMER2,TCCR2A,TCCR2B);
			} while (false);
		# endif
	
	} // setupTimerIntervals()


	void Posixino::setupTimerInterrupt(int num,int force,int a,int b) {

		interruptCounter[num] = 0;
		
		if (force > 0) {
			interruptTiming[num] = force;
			return;
		}
		
		/// TODO: set timing based on register values
		interruptTiming[num] = 999;
		
	} // setupTimerInterrupt()


	void Posixino::startTimerThread() {
		
		static std::thread timerThread(callTimerThread);
		timerThread.detach();
		
	} // startTimerThread()
	
	
	void Posixino::timerThread() {	
	
		while (true) {
		
			usleep(1000);
			
			for (int n = 0; n < 3; n++) {		
				if (interruptCounter[n] < 0) continue;
		
				interruptCounter[n] += 1000;
				if (interruptCounter[n] < interruptTiming[n]) continue;
				interruptCounter[n] -= interruptTiming[n];
					
				if (n == 0) {
					# ifdef TIMER0
						TIMER0_COMPA_vect();
					# endif
				}
				if (n == 1) {
					# ifdef TIMER1
						TIMER1_COMPA_vect();
					# endif
				}
				if (n == 2) {
					# ifdef TIMER2
						TIMER2_COMPA_vect();
					# endif
				}		
			
			} // for interrupts
		} // forever	
		
	} // timerThread()


// ----[ Adafruit_NeoPixel ]-------------------------------------------


	Adafruit_NeoPixel::Adafruit_NeoPixel(int numOfPx,int pin, int flags) {

		numberOfPixels = numOfPx;

		# ifdef SDL_DISPLAY

		emuPixels = new EmuPixel[numOfPx]; 
		emuSetGridScreenAnchor("southwest");
		emuSetGridScreenPercent(8);
		emuSetGridCells(numOfPx,1);
		for (int n = 0; n < numOfPx; n++) emuSetPixelPos(n,n,0);
		
		sdlInitialized = false;
		smallestError = 0;
		biggestError = 0;

		# endif

	} // Adafruit_NeoPixel() ctor


	int Adafruit_NeoPixel::numPixels() {
		return numberOfPixels;
	} // numPixels()
	

	void Adafruit_NeoPixel::begin() {
		pixels = (uint32_t*)malloc(numberOfPixels * sizeof(uint32_t));
		for (int n = 0; n < numberOfPixels; n++) pixels[n] = 0;
	} // begin()
	
	
	void Adafruit_NeoPixel::setBrightness(int v) {
		// this method is intentionally left blank
	} // setBrightness()
	
	
	uint32_t Adafruit_NeoPixel::Color(int r,int g,int b) {
		return r << 16 | g << 8 | b; 
	} // Color()
	
	
	void Adafruit_NeoPixel::setPixelColor(int numero,int r,int g,int b) {
		setPixelColor(numero,Color(r,g,b));
	} // setPixelColor()
	
	
	void Adafruit_NeoPixel::setPixelColor(int numero,uint32_t color) {

		if ((numero < 0) || (numero >= numberOfPixels)) {
			bool report = false;
			if (numero < smallestError) {
				smallestError = numero;
				report = true;
			}
			if (numero > biggestError) {
				biggestError = numero;
				report = true;
			}
			if (!report) return;
			
			posixino.printErrorPrefix();
			fprintf(stderr,"invalid pixel index: %d \n",numero);

		} // if error

		pixels[numero] = color;

	} // setPixelColor()
	
	
	# ifndef SDL_DISPLAY
	void Adafruit_NeoPixel::show() { 
		
		// TODO: some terminal magic
		
	} // show()
	# endif


// ---- SDL2 ----
	# ifdef SDL_DISPLAY


	void Adafruit_NeoPixel::emuSetGridScreenAnchor(const char* anchor) {
		gridAnchor = anchor;
	} // emuSetGridScreenAnchor()


	void Adafruit_NeoPixel::emuSetGridScreenPercent(int percent) {
		gridPercent = percent;
	} // emuSetGridScreenPercent()
	
	
	void Adafruit_NeoPixel::emuSetGridCells(int cols,int rows) {
		gridCellCols = cols;
		gridCellRows = rows;
	} // emuSetGridCells()
	
	
	void Adafruit_NeoPixel::emuCheckPixelIndex(int i) {
	
		do {
			if (i < 0) break;
			if (i > numPixels()) break;
			return;
		} while (false);
		
		posixino.printErrorPrefix();
		fprintf(stderr,"invalid pixel index: %d \n",i);
		exit(1);
		
	} // emuCheckPixelIndex()
	
	
	void Adafruit_NeoPixel::emuSetPixelPos(int i,int x,int y) {
		emuCheckPixelIndex(i);
		emuPixels[i].setPos(x,y);
	} // emuSetPixelPos()
	

	void Adafruit_NeoPixel::emuSetPixelCellSize(int i,int width,int height) {
		emuCheckPixelIndex(i);
		emuPixels[i].setSize(width,height);
	} // emuSetPixelSize()
	
	
	void Adafruit_NeoPixel::emuSetPixelPixGap(int i,int horizontal,int vertical) {
		emuCheckPixelIndex(i);
		emuPixels[i].setGap(horizontal,vertical);
	} // emuSetPixelPixGap()
	

	void Adafruit_NeoPixel::show() {
	
		initializeSdl();
		
		for (int n = 0; n < numberOfPixels; n++) {
			EmuPixel& epix = emuPixels[n];
			
			uint32_t rgb = pixels[n];
			int r = rgb >> 16;
			int g = rgb >> 8 & 0xff;
			int b = rgb & 0xff;

			SDL_Rect rect;
			rect.x = epix.x;
			rect.y = epix.y;
			rect.w = epix.width;
			rect.h = epix.height;
			
			SDL_FillRect(
				screenSurface,
				&rect, 
				SDL_MapRGB(screenSurface->format,r,g,b)
			);
			
		} // for pixels
	
		SDL_UpdateWindowSurface(window);
		quitOnKey();
			
	} // show()	

	
	void Adafruit_NeoPixel::initializeSdl() {
		
		if (sdlInitialized) return;
		sdlInitialized = true;
		
		if (SDL_Init(SDL_INIT_VIDEO) != 0) posixino.fatal("SDL init failed",3);

		SDL_GetCurrentDisplayMode(SDL_DISPLAY,&current);
		
		calcDims();

		window = SDL_CreateWindow(
			"Posixino",
			windowPosX,
			windowPosY,
			windowWidth,
			windowHeight,
			SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS
		);		
		
		screenSurface = SDL_GetWindowSurface(window);
		SDL_FillRect(screenSurface,NULL,0);
		SDL_UpdateWindowSurface(window);
	
	} // initializeSdl()


	void Adafruit_NeoPixel::calcDims() {
	
		bool debug = false;

		if (debug) printf("params \n");
		if (debug) printf(" screen w=%d h=%d \n",current.w,current.h);
		if (debug) printf(" grid percent=%d \n",gridPercent);
		if (debug) printf(" grid cols=%d rows=%d \n",gridCellCols,gridCellRows);

		if (debug) printf("calc \n");
	
		int cellSizeByWidth = (current.w * gridPercent / 100) / gridCellCols;
		int cellSizeByHeight = (current.h * gridPercent / 100) / gridCellRows ;
		int cellSizeSel = ( cellSizeByWidth > cellSizeByHeight ? cellSizeByWidth : cellSizeByHeight );
		
		windowWidth = gridCellCols * cellSizeSel;
		windowHeight = gridCellRows * cellSizeSel;
		cellWidth = cellSizeSel;
		cellHeight = cellSizeSel;
		if (windowWidth > current.w) cellWidth = current.w / gridCellCols;
		if (windowHeight > current.h) cellHeight = current.h / gridCellRows;
		if (cellWidth > 2 * cellHeight) cellWidth = 2 * cellHeight;
		if (cellHeight > 2 * cellWidth) cellHeight = 2 * cellWidth;
		windowWidth = gridCellCols * cellWidth;
		windowHeight = gridCellRows * cellHeight;
	
		if (debug) printf(" cell_size by_width=%d by_height=%d selected=%d \n",cellSizeByWidth,cellSizeByHeight,cellSizeSel);
		if (debug) printf(" cell_adjusted_size width=%d height=%d \n",cellWidth,cellHeight);

		char horiz = 'C';
		char vert = 'M';
		if (strcasestr(gridAnchor,"west") != NULL) horiz = 'L';
		if (strcasestr(gridAnchor,"east") != NULL) horiz = 'R';
		if (strcasestr(gridAnchor,"north") != NULL) vert = 'T';
		if (strcasestr(gridAnchor,"south") != NULL) vert = 'B';
		if (strcasecmp(gridAnchor,"w") == 0) horiz = 'L';
		if (strcasecmp(gridAnchor,"e") == 0) horiz = 'R';
		if (strcasecmp(gridAnchor,"n") == 0) vert = 'T';
		if (strcasecmp(gridAnchor,"s") == 0) vert = 'B';
		if (strcasecmp(gridAnchor,"nw") == 0) {
			horiz = 'L';
			vert = 'T';
		}
		if (strcasecmp(gridAnchor,"ne") == 0) {
			horiz = 'R';
			vert = 'T';
		} 
		if (strcasecmp(gridAnchor,"sw") == 0) {
			horiz = 'L';
			vert = 'B';
		}
		if (strcasecmp(gridAnchor,"se") == 0) {
			horiz = 'R';
			vert = 'B';
		}
		if (debug) printf(" anchor param=%s horiz=%c vert=%c \n",gridAnchor,horiz,vert);

		switch (horiz) {
		case 'L':
			windowPosX = 0;
			break;
		case 'C':
			windowPosX = (current.w - windowWidth) / 2;
			break;
		case 'R':
			windowPosX = current.w - windowWidth;
			break;
		}

		switch (vert) {
		case 'T':
			windowPosY = 0;
			break;
		case 'M':
			windowPosY = (current.h - windowHeight) / 2;
			break;
		case 'B':
			windowPosY = current.h - windowHeight;
			break;
		}
	
		if (debug) printf("result \n");
		if (debug) printf(" window x=%d y=%d w=%d h=%d \n",windowPosX,windowPosY,windowWidth,windowHeight);
		
		for (int n = 0; n < numberOfPixels; n++) {
			EmuPixel& epix = emuPixels[n];
			
			epix.x = epix.parmX * cellWidth;
			epix.width = epix.parmWidth * cellWidth;
			int left = epix.parmGapHoriz / 2;
			int right = epix.parmGapHoriz % 2 + left;
			epix.x += left;
			epix.width -= right;
			
			epix.y = epix.parmY * cellHeight;
			epix.height = epix.parmHeight * cellHeight;
			int top = epix.parmGapVert / 2;
			int bottom = epix.parmGapVert % 2 + top;
			epix.y += top;
			epix.height -= bottom;
		
		} // for pixels
		
	} // calcDims()


	void Adafruit_NeoPixel::quitOnKey() {

		SDL_Event event;
		if (!SDL_PollEvent(&event)) return;

		switch (event.type) {
    case SDL_KEYDOWN:    
			quit(0);
		}
		
	} // quitOnKey()


	EmuPixel::EmuPixel() {
		setPos(0,0);
		setSize(1,1);
		setGap(1,1);
	} // EmuPixel() ctor
	
	
	void EmuPixel::setPos(int x,int y) {
		parmX = x;
		parmY = y;
	} // setPos()
	
	
	void EmuPixel::setSize(int w,int h) {
		parmWidth = w;
		parmHeight = h;
	} // setSize()
	
	
	void EmuPixel::setGap(int h,int v) {
		parmGapHoriz = h;
		parmGapVert = v;
	} // setGap()
	

	# endif

// ----[ more to come... ]---------------------------------------------
