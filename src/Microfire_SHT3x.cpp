#include "Microfire_SHT3x.h"

float Microfire::SHT3x::_tempC = 0;
float Microfire::SHT3x::_tempF = 0;
float Microfire::SHT3x::_vpd_kPa = 0;
float Microfire::SHT3x::_dew_pointC = 0;
float Microfire::SHT3x::_dew_pointF = 0;
float Microfire::SHT3x::_heat_indexF = 0;
float Microfire::SHT3x::_heat_indexC = 0;
float Microfire::SHT3x::_wet_bulbF = 0;
float Microfire::SHT3x::_wet_bulbC = 0;
float Microfire::SHT3x::_RH = 0;
int Microfire::SHT3x::_status = 0;

namespace Microfire
{
  bool SHT3x::begin(TwoWire &wirePort, uint8_t address)
  {
    _address = address;
    _i2cPort = &wirePort;

    return connected();
  }

  bool SHT3x::connected()
  {
    _i2cPort->beginTransmission(_address);
    uint8_t retval = _i2cPort->endTransmission();

    if (retval)
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  int SHT3x::measure()
  {
    _measure_all();
    if (_status == STATUS_NO_ERROR)
    {
      _vpd();
      _dew_point();
      _heat_index();
      _wet_bulb();
    }
    else
    {
      _tempC = 0;
      _tempF = 0;
      _vpd_kPa = 0;
      _dew_pointC = 0;
      _dew_pointF = 0;
      _RH = 0;
    }

    return status;
  }

  float SHT3x::_dew_point()
  {
    float tem = -1.0 * _tempC;
    float esdp = 6.112 * exp(-1.0 * 17.67 * tem / (243.5 - tem));
    float ed = _RH / 100.0 * esdp;
    float eln = log(ed / 6.112);
    _dew_pointC = -243.5 * eln / (eln - 17.67);

    _dew_pointF = (_dew_pointC * 1.8) + 32;
    return _dew_pointC;
  }

  float SHT3x::_heat_index()
  {
    float _hi_f;
    float adjustment;

    // step 1
    _hi_f = 0.5 * (_tempF + 61.0 + ((_tempF - 68.0) * 1.2) + (_RH * 0.094));
    if (_hi_f <= 80.0)
    {
      _heat_indexF = _hi_f;
      _heat_indexC = (_heat_indexF - 32.0) * (0.5556);

      return _heat_indexC;
    }

    // step 2: heat index calc was over 80 F
    _heat_indexF = -42.379 + 2.04901523 * _tempF + 10.14333127 * _RH - .22475541 * _tempF * _RH - .00683783 * _tempF * _tempF - .05481717 * _RH * _RH + .00122874 * _tempF * _tempF * _RH + .00085282 * _tempF * _RH * _RH - .00000199 * _tempF * _tempF * _RH * _RH;

    // step 3: is RH less than 13 and temp F between 80 - 112
    if (_RH <= 13.0)
    {
      if ((_tempF >= 80) && (_tempF <= 110.0))
      {
        adjustment = ((13.0 - _RH) / 4.0) * sqrt((17.0 - abs(_tempF - 95.0)) / 17.0);
        _heat_indexF = _heat_indexF - adjustment;
      }
    }

    // step 4: is RH more than 85 and tempF between 80 - 87
    if (_RH >= 85.0)
    {
      if ((_tempF >= 80) && (_tempF <= 87.0))
      {
        adjustment =  ((_RH-85.0)/10.0) * ((87.0-_tempF)/5.0);
        _heat_indexF = _heat_indexF + adjustment;
      }
    }
    _heat_indexC = (_heat_indexF - 32.0) * (0.5556);
    return _heat_indexC;
  }

  float SHT3x::_wet_bulb()
  {
    double a = _tempC * atan(0.151977 * pow((_RH + 8.313659), 0.5));
    double b = atan(_tempC + _RH) - atan(_RH - 1.676331);
    double c = 0.00391838 * pow(_RH, 1.5) * atan(0.023101 * _RH);
    double tw = a + b + c - 4.686035;

    _wet_bulbC = tw;
    _wet_bulbF = (_wet_bulbC * 1.8) + 32;
    return _wet_bulbC;
  }

  void SHT3x::_measure_all()
  {
    uint8_t buffer[6];
    uint16_t raw;

    _reset();
    _send_command(SHT3x_MEASUREMENT);
    delay(15);
    _read_bytes(6, (uint8_t *)&buffer[0]);

    if ((buffer[2] = _crc8(buffer, 2)))
    {
      raw = (buffer[0] << 8) + buffer[1];
      _tempC = raw * (175.0 / 65535) - 45;
      _tempF = (_tempC * 1.8) + 32;
    }
    else
    {
      _tempC = 0;
      _tempF = 0;
      _vpd_kPa = 0;
      _dew_pointC = 0;
      _dew_pointF = 0;
      _RH = 0;
      _status = STATUS_CRC_ERROR;
      return;
    }
    if ((buffer[5] = _crc8(buffer + 3, 2)))
    {
      raw = (buffer[3] << 8) + buffer[4];
      _RH = raw * (100.0 / 65535);
    }
    else
    {
      _tempC = 0;
      _tempF = 0;
      _vpd_kPa = 0;
      _dew_pointC = 0;
      _dew_pointF = 0;
      _RH = 0;
      _status = STATUS_CRC_ERROR;
      return;
    }
  }

  void SHT3x::_reset()
  {
    _send_command(SHT3x_SOFT_RESET);
    delay(1);
  }

  float SHT3x::_vpd()
  {
    float es = 0.61078 * exp(17.2694 * _tempC / (_tempC + 238.3));
    float ae = _RH / 100 * es;
    _vpd_kPa = es - ae;

    return _vpd_kPa;
  }

  bool SHT3x::_send_command(uint16_t command)
  {
    _i2cPort->beginTransmission(_address);
    _i2cPort->write(command >> 8);
    _i2cPort->write(command & 0xFF);
    _i2cPort->endTransmission();

    if (_i2cPort->endTransmission() != 0)
    {
      _status = STATUS_NOT_CONNECTED;
      return false;
    }

    return true;
  }

  bool SHT3x::_read_bytes(uint8_t n, uint8_t *val)
  {
    int r = _i2cPort->requestFrom(_address, n);
    if (r == n)
    {
      for (uint8_t i = 0; i < n; i++)
      {
        val[i] = _i2cPort->read();
      }
      _status = STATUS_NO_ERROR;
      return true;
    }
    else
    {
      _status = STATUS_NOT_CONNECTED;
    }
    return false;
  }

  bool SHT3x::_crc8(const uint8_t *data, uint8_t len)
  {
    const uint8_t pol(0x31);
    uint8_t crc(0xFF);

    for (uint8_t j = len; j; --j)
    {
      crc ^= *data++;

      for (uint8_t i = 8; i; --i)
      {
        crc = (crc & 0x80) ? (crc << 1) ^ pol : (crc << 1);
      }
    }
    return crc;
  }
}