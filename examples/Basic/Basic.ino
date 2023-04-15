/*!
   microfire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@microfire.co to get in touch with someone
*/
#include "Microfire_SHT3x.h"
Microfire::SHT3x sht30;

void setup()
{
  // start Serial and I2C
  Serial.begin(9600);
  Wire.begin();

  // start the sensor
  sht30.begin();
}

void loop()
{
  // take a measurement
  sht30.measure();

  // display the results
  switch (sht30.status)
  {
  case sht30.STATUS_NOT_CONNECTED:
    Serial.println("Error: Sensor not connected");
    break;
  case sht30.STATUS_CRC_ERROR:
    Serial.println("Error: CRC error");
    break;
  case sht30.STATUS_NO_ERROR:
    Serial.println((String)sht30.tempC + " °C");
    Serial.println((String)sht30.tempF + " °F");
    Serial.println((String)sht30.RH + " %RH");
    Serial.println((String)sht30.vpd_kPa + " VPD kPa");
    Serial.println((String)sht30.dew_pointC + " dew point °C");
    Serial.println((String)sht30.dew_pointF + " dew point °F");
    Serial.println((String)sht30.heat_indexC + " heat index °C");
    Serial.println((String)sht30.heat_indexF + " heat index °F");
    Serial.println((String)sht30.wet_bulbC + " wet bulb °C");
    Serial.println((String)sht30.wet_bulbF + " wet bulb °F");
    break;
  }
  delay(1000);
}