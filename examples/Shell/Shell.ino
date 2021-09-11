/*
   This allows you to run functions in a command-line like interface.
   Enter:
   `m` to take and display all measurements.
*/

#include <Arduino.h>
#include "uFire_SHT3x.h"

uFire::SHT3x sht30;
String buffer, cmd, p1, p2;

void measure()
{
  sht30.measure();
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
    break;
  }
}

void cmd_run()
{
  if (cmd == "m")
    measure();
}

void cli_process()
{
  while (Serial.available())
  {
    char c = Serial.read();

    if (c == '\r')
      break;

    switch (c)
    {
    case '\n': // new line
      Serial.println();
      cmd = buffer.substring(0, buffer.indexOf(" ", 0));
      cmd.trim();
      buffer.remove(0, buffer.indexOf(" ", 0));
      buffer.trim();
      p1 = buffer.substring(0, buffer.indexOf(" ", 0));
      p1.trim();
      buffer.remove(0, buffer.indexOf(" ", 0));
      buffer.trim();
      p2 = buffer.substring(0, buffer.indexOf(" ", 0));
      p2.trim();
      cmd_run();
      Serial.print("> ");
      buffer = "";
      break;

    case '\b': // backspace
      buffer.remove(buffer.length() - 1);
      Serial.print("\b \b");
      break;

    default: // everything else
      buffer += c;
      Serial.print(c);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  sht30.begin();
  Serial.println("Type `m` to take all measurements");
  Serial.println(sht30.connected() ? "connected" : "*disconnected*");

  Serial.print("> ");
}

void loop()
{
  cli_process();
}
