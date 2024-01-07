//Hiker's Device LoRa module
 
//Define libraries
#include <Wire.h>
#include <LoRa.h> //For LoRa module
#include <SPI.h>  //since LoRa module uses SPI communication with the micro-controller
#include <LiquidCrystal_I2C.h>  //For lcd display
#include <TinyGPSPlus.h>    //For gps module
#include <SoftwareSerial.h> // to use any digital pins as tx and rx on the arduino board
 
 
// byte localAddress = 0xBB;  //Address of local device
// byte destination = 0xFF;   //Address of destination device
#define btn1 6
#define btn2 7
//define variables
unsigned long startTime = millis();   
const int interval = 5000;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
 
//Adressing lcd and creating object
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
// creating GPS library's object
TinyGPSPlus gps;
 
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
// SoftwareSerial blue(0,1);
 
String Mymessage;
void setup() {
  Serial.begin(9600); //stating serial monitor
  ss.begin(GPSBaud);  
  // blue.begin(GPSBaud); 
  pinMode(btn1,INPUT_PULLUP);
  pinMode(btn2,INPUT_PULLUP);
  
  lcd.init();   //LCD initialization
  lcd.backlight();  
  //Starting LoRa
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check your connection.");
    while (true);
  }
  //setting LoRa transmitting power
  LoRa.setTxPower(20);
  Serial.println("LoRa init succeeded");
}
int buttonstate1;
int buttonstate2;
void loop() {
 
  //GPS module code to send longitude and latitude to centre
  //Checks if the GPS module is sending location through serial pin
  while (ss.available() > 0) {
    //encode the message from the gps module from nmea format to understandable
    if (gps.encode(ss.read())){
      //interval
      if (millis() - startTime >= interval) {
        //LoRa communication to send message
        LoRa.beginPacket();
        // LoRa.write(destination);
        // LoRa.write(localAddress);
        LoRa.print("Latitude: " + (String)gps.location.lat() + " ");  //display latitude 
        LoRa.print("Longitude: " + (String)gps.location.lng());       //display longitude
        LoRa.endPacket();       //close the packet and broadcast
        startTime = millis();
      }
      buttonstate1 = digitalRead(btn1);
      buttonstate2 = digitalRead(btn2);

      if(buttonstate1 == LOW){
        sendMessage("Emergency!!");
      }
      if(buttonstate2 == LOW){
        sendMessage("Lost Way ");
      }
        
     }
  }
  // if no gps detected
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
 
  if (Serial.available() > 0) {
    //read data
    Mymessage = Serial.readString();
    sendMessage(Mymessage);
    Serial.print("Message sent: ");
    Serial.println(Mymessage);
    delay(100);
    Mymessage = "";
  }
  delay(100);
  //check for packet and parse the packet
  int packetSize = LoRa.parsePacket();
  delay(100);
  if (packetSize) {
    // byte recipient = LoRa.read();
    // byte sender = LoRa.read();
    String incoming = "";
    while (LoRa.available()) {
      Serial.print("Message Received : ");
      incoming = LoRa.readString();
      Serial.println(incoming);
      Serial.println(LoRa.packetRssi());
      //LCD code
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("msg");
      lcd.setCursor(0, 1);
      lcd.print(incoming);
    }
  }
  // bluetooth();
 
}
//send message received through serial monitor
void sendMessage(String outgoing) {
  LoRa.beginPacket();
  // LoRa.write(destination);
  // LoRa.write(localAddress);
  LoRa.print(outgoing);
  LoRa.endPacket();
}
// void bluetooth(){
//     // Feed any data from bluetooth to Terminal.
//   if (blue.available()){
    
//     Serial.write(blue.read());
//     sendMessage(String(blue.read()));
//   }
 
//   // Feed all data from termial to bluetooth
  
// }