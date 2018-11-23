/*This software, the ideas and concepts is Copyright (c) David Bird 2014 and beyond.
All rights to this software are reserved.
It is prohibited to redistribute or reproduce of any part or all of the software contents in any form other than the following:
 1. You may print or download to a local hard disk extracts for your personal and non-commercial use only.
 2. You may copy the content to individual third parties for their personal use, but only if you acknowledge the author David Bird as the source of the material.
 3. You may not, except with my express written permission, distribute or commercially exploit the content.
 4. You may not transmit it or store it in any other website or other form of electronic retrieval system for commercial purposes.
 5. You MUST include all of this copyright and permission notice ('as annotated') and this shall be included in all copies or substantial portions of the software and where the software use is visible to an end-user.
THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT.
FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
