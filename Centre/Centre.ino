//Centre LoRa device

//Libraries
#include <LoRa.h>
#include <SPI.h>

//pin define
#define SS 15
#define RST 16
#define DIO0 4

//variable declaration
String data = "";                 //message to sent     
// byte localAddress = 0xFF;         //Local module address
// byte destination = 0xBB;          //destination module adress

void setup()
{
  Serial.begin(9600);                //Serial monitor begin
  while (!Serial);                   
  LoRa.setPins(SS, RST, DIO0);       //setting the chip select, reset and dgital I/O pins of LoRa module
  //Starting LoRa module
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed.");
    delay(100);
    while (1);
  }
}

void loop()
{
  //send any message through serial monitor
  sendMessage();
  //check the incoming packet
  int packetSize = LoRa.parsePacket();
  delay(100);
  //Read the message if packet is not null
  if (packetSize) {
    // byte recipient = LoRa.read();
    // byte sender = LoRa.read();
    while (LoRa.available()) {
      Serial.print("Message Received : ");
      Serial.println(LoRa.readString());
    }
  }
}


void sendMessage() {
  //detects any message on serial input line
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    data += inChar;
    //if pressed enter sends the message
    if (inChar == '\n') {
      Serial.print("Sending Message: ");
      Serial.println(data);
      LoRa.beginPacket();
      // LoRa.write(destination);
      // LoRa.write(localAddress);
      LoRa.print(data);
      LoRa.endPacket();
      data="";
      delay(200);
    }
  }
}