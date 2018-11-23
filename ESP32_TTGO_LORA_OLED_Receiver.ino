 
#include "SSD1306Wire.h"
#include <LoRa.h>

#define SCK 5 // GPIO5 - SX1278's SCK
#define MISO 19 // GPIO19 - SX1278's MISO
#define MOSI 27 // GPIO27 - SX1278's MOSI
#define SS 18 // GPIO18 - SX1278's CS
#define RST 14 // GPIO14 - SX1278's RESET
#define DI0 26 // GPIO26 - SX1278's IRQ (interrupt request)
#define BAND 868E6 // 915E6

SSD1306Wire display (0x3c, 4, 15);
String rssi = "RSSI ";
String packSize = "-";
String packet;

void setup () {
  pinMode(2, OUTPUT);
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay (50);
  digitalWrite(16, HIGH); // while OLED is running, GPIO16 must go high,
  
  Serial.begin(115200);
  Serial.println("LoRa Receiver Callback");
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  if (! LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.onReceive(cbk);
  LoRa.receive();
  Serial.println("init ok");
  display.init();
  display.flipScreenVertically ();
  display.setFont(ArialMT_Plain_16);
  delay (1500);
}

void loop () {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {    // received a packet
    Serial.print("Received packet '");
    while (LoRa.available()) {    // read packet
      packet = LoRa.readString();
      Serial.print(packet);
    }
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());   // print RSSI of packet
    display.clear();
    display.setTextAlignment (TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, rssi+String(LoRa.packetRssi())+ "Rx: " + String(packetSize) + " Bytes");
    display.drawString(0, 20, packet);
    Serial.println(packet);
    packet = "";
    display.display ();
    digitalWrite(2, HIGH); // turn the LED on (HIGH is the voltage level)
    delay (500); // wait for a second
    digitalWrite(2, LOW); // turn the LED off by making the voltage LOW
    delay (500); // wait for a second
  }
}
