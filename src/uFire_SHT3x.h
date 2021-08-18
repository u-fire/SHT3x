#ifndef UFIRE_SHT3x
#define UFIRE_SHT3x

#include <Arduino.h>
#include <Wire.h>

#define SHT3x_I2C_ADDRESS 0x44
#define SHT3x_SOFT_RESET 0x30A2
#define SHT3x_MEASUREMENT 0x2400 // high accuracy, no clock stretching

namespace uFire
{
  class SHT3x
  {
  public:
    const float &tempC = _tempC;
    const float &tempF = _tempF;
    const float &vpd_kPa = _vpd_kPa;
    const float &dew_pointC = _dew_pointC;
    const float &dew_pointF = _dew_pointF;
    const float &RH = _RH;
    const int &status = _status;

    typedef enum SHT30_Error_e
    {
      STATUS_NO_ERROR,
      STATUS_NOT_CONNECTED,
      STATUS_CRC_ERROR
    } SHT30_Error_t;

    bool begin(TwoWire &wirePort = Wire, uint8_t address = SHT3x_I2C_ADDRESS);
    bool connected();
    int measure();

  private:
    static float _tempC;
    static float _tempF;
    static float _vpd_kPa;
    static float _dew_pointC;
    static float _dew_pointF;
    static float _RH;
    static int _status;

    uint8_t _address;
    TwoWire *_i2cPort;
    float _vpd();
    float _dew_point();
    void _measure_all();
    void _reset();
    bool _send_command(uint16_t command);
    bool _read_bytes(uint8_t n, uint8_t *val);
    bool _crc8(const uint8_t *data, uint8_t len);
  };
}
#endif // ifndef UFIRE_SHT3x
