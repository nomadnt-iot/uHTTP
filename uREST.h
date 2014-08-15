/*
  uREST.h - Library that implement REST like interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#ifndef uREST_H
#define uREST_H

#include "Arduino.h"
#include "uHTTP.h"

// #define DEBUG 0
#define JSON_SIZE 64

class uREST{

  public:
    uREST(uint8_t *pin);
    bool process(EthernetClient& client);
    bool has(uint8_t pin);

  private:
    uint8_t *_pin;
};

#endif