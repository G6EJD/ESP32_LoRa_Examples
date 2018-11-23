#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C

const unsigned long UpdateInterval = 0.25 * (60L * 1000000L); // Update delay in microseconds

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
