clear
mkdir -p bin
#g++ test/Basics/Blink.cpp -o bin/blink
#g++ test/LiquidCrystal/HelloWorld.cpp -o bin/lcdhello
#g++ test/LiquidCrystal/Scroll.cpp -o bin/scroll
#g++ test/Ethernet/WebClient.cpp -o bin/webclient
#g++ test/Communication/ASCIITable.cpp -o bin/asciitable
#g++ test/LiquidCrystal/SerialDisplay.cpp -o bin/serialdisplay
#g++ test/Ethernet/WebServer.cpp -o bin/webserver
#g++ test/milan_autos.cpp -o bin/milan_autos
g++ -std=c++11 -pthread test/interrupt.cpp -o bin/interrupt


