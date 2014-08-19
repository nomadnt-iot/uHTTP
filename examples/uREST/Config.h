#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

struct Config{
  bool configured;
  uint8_t port;
  bool dhcp;
  uint8_t ip4addr[4];
  uint8_t netmask[4];
  uint8_t gateway[4];
  uint8_t dns[4];
} cfg;

template <class T> int config_write(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
	  EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int config_read(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
	  *p++ = EEPROM.read(ee++);
    return i;
}