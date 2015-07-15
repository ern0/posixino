# ifdef __unix
# include "../src/posixino.cpp"
# else

#include <SPI.h>       
#include <Ethernet.h>
#include <SD.h>
#include <WString.h>
#include <String.h>
#include <LiquidCrystal.h>

# endif

#define REQ_BUF_SZ   60
#define MAX_STRING_LEN  20
#define DIGITAL_NUM 13
#define ANALOG_NUM 5

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192,168,1,20 };
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);


char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0; // index into HTTP_req buffer
char ajax;
char header[] = "HTTP/1.1 200 OK\n"
                "Content-Type: text/html\n"
                "Refresh: 1\n"
                "Connection: close\n";
String def_str;
int state = 0;
EthernetServer server(80);
char lcdtext[100];
int timer = 1;
int AnalogValue;

void setup()
{
  lcd.begin(20, 4);
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  Serial.println("device is started");
  analogWrite(9,650);
  pinMode(8, OUTPUT);
  server.begin();
}

int thermometer(int reading){
    float temperature = 0.0;  
    float ten_samples = 0.0;  
    temperature = ((float)reading * 5.0 / 1024.0) - 0.52;
    temperature = temperature / 0.01;
    return temperature;
}

String print_sensor_value(int pinType, int krit){
    String div;
    int AnalogValue = analogRead(pinType);
    div = String("");
    div+="<div style='width: ";
    div+= AnalogValue;
    div+="px; height: 50px; background-color: grey; margin: 5px;' id='pin'> Pin = ";
    div+=pinType;
    div+="<br> Value : ";
    div+=thermometer(AnalogValue);
    div+="</div>";
    return div;
}

String print_button_value(int pinType, int krit){
   
    String div_butt;
    if (analogRead(pinType) <= krit){
        div_butt+="<div style='width: 100px; height: 50px; background-color: red; margin: 5px;' id='pin'> Pin = ";
    }
    else div_butt+="<div style='width: 100px; height: 50px; background-color: green; margin: 5px;' id='pin'> Pin = ";

    div_butt+=pinType;
    div_butt+="<br> Value : ";
    div_butt+=analogRead(pinType);
    div_butt+="</div>";
    return div_butt;
}



void loop()
{
    timer = timer + 1 ;
    if (timer > 100){
        digitalWrite(8, HIGH);
        timer = 1;
        AnalogValue = analogRead(2);

    }
    if(timer > 50) {
        digitalWrite(8, LOW);
    }
    lcd.setCursor(5,0);
    lcd.print("CAR SYSTEM");
    lcd.setCursor(0,1);
    lcd.print("HOMERSEKLET: ");
    lcd.print(thermometer(AnalogValue));
    lcd.print(" C");
    lcd.setCursor(0,2);
    lcd.print("TURBO NYOMAS: ");
    lcd.print(thermometer(AnalogValue)/10);
    lcd.print(" BAR");
    delay(10);
    for (int i=3; i<=5; i++){
        if(analogRead(i) > 500) {
            lcd.setCursor(0,3);
            lcd.print("Utolso Gomb : ");
            lcd.print(i);
        } else{
            lcd.print("            ");
        }
    }
  EthernetClient client = server.available();
   if (client) {

    while (client.connected()) {
        if (client.available()) {
                client.println(header);
                client.println("<!DOCTYPE html><head><meta http-equiv='Content-Type' content='text/html; charset=UTF-8' /><META HTTP-EQUIV='Content-Language' Content='hu'>");
                client.println("<title>Arduino</title></head><body>");
                client.println("Analog pin values<br>");
                for (int i=3; i<=5; i++){
                    client.println(print_button_value(i,500));
                }
                client.println(print_sensor_value(2,170));
                client.println("</body></html>");

        } //  client available
    delay(1);
    client.stop();
    } // client connected
   } // if client
 // delay(100);
}
