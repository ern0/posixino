clear
mkdir -p bin
#g++ test/Basics/Blink.cpp -o bin/blink
g++ -std=c++11 -pthread test/LiquidCrystal/HelloWorld.cpp -o bin/lcdhello
#g++ test/LiquidCrystal/Scroll.cpp -o bin/scroll
#g++ test/Ethernet/WebClient.cpp -o bin/webclient
#g++ test/Communication/ASCIITable.cpp -o bin/asciitable
#g++ test/LiquidCrystal/SerialDisplay.cpp -o bin/serialdisplay
#g++ test/Ethernet/WebServer.cpp -o bin/webserver
#g++ test/milan_autos.cpp -o bin/milan_autos
#g++ -std=c++11 -pthread test/interrupt.cpp -o bin/interrupt
#g++ -std=c++11 -pthread `sdl2-config --cflags --libs` test/Adafruit_NeoPixel/strandtest.cpp -o bin/strandtest -lSDL2
