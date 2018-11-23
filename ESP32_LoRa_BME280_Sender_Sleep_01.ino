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
#include <SPI.h>
#include <LoRa.h> //https://github.com/sandeepmistry/arduino-LoRa
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C

const unsigned long UpdateInterval = 0.25 * (60L * 1000000L); // Update delay in microseconds, currently 15-secs (1/4 of minute)

void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Sender starting...");
  LoRa.setPins(18, 14, 26); // LoRa.setPins(CS, RESET, IRQ); 
  if (!LoRa.begin(868E6)) { // Set frequency to 433, 868 or 915MHz
    Serial.println("Could not find a valid LoRa transceiver, check pins used and wiring!");
    delay(1000);
  }
  bool status = bme.begin();  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    delay(1000);
  }
  Serial.println("Sending data packet...");
  Send_and_Display_Sensor_Data();
  start_sleep();  
}

void start_sleep(){
  esp_sleep_enable_timer_wakeup(UpdateInterval);
  pinMode(BUILTIN_LED,INPUT);     // Set pin-5 to an input as sometimes PIN-5 is used for SPI-SS
  digitalWrite(BUILTIN_LED,HIGH); // In case it's on, turn LED off, as sometimes PIN-5 on some boards is used for SPI-SS and can be left low
  Serial.println("Starting deep-sleep period... awake for "+String(millis())+"mS");
  delay(8); // Enough time for the serial port to finish at 115,200 baud
  esp_deep_sleep_start();         // Sleep for the prescribed time
}

void loop() {
}

void Send_and_Display_Sensor_Data(){
  LoRa.beginPacket();                                                // Start LoRa transceiver
  LoRa.print("Temp: "+String(bme.readTemperature(),1)+"°C\n");       // Send BME280 temperature reading
  LoRa.print("Humi: "+String(bme.readHumidity(),1)+"%\n");           // Send BME280 humidity reading
  LoRa.print("Pres: "+String(bme.readPressure()/100+3.7,1)+"hP  ");  // Send BME280 pressure reading, addition of 3.7 is for calibration purposes
  LoRa.endPacket();                                                  // Confirm end of LoRa data packet
  LoRa.sleep();                                                      // Send LoRa transceiver to sleep
  Serial.print("Temperature : "+String(bme.readTemperature(),1)+"°C\n");
  Serial.print("Humidity    : "+String(bme.readHumidity(),1)+"%\n");
  Serial.print("Pressure    : "+String(bme.readPressure()/100+3.7,1)+"hP\n"); // 3.7 is added for calibration purposes
}
