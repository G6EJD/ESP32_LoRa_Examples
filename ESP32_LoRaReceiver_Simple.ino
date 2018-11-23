#include <SPI.h>
#include <LoRa.h> //https://github.com/sandeepmistry/arduino-LoRa

void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Receiver");
  LoRa.setPins(18, 14, 26);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    // Serial.print("Received packet ");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print(" RSSI was: ");
    Serial.println(LoRa.packetRssi());
  }
}
