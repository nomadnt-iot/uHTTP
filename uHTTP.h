/*
  uHTTP.h - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#ifndef uHTTP_H
#define uHTTP_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "../UIPEthernet/UIPEthernet.h"

#define METHOD_SIZE 8
#define URI_SIZE 16
#define AUTH_SIZE 32
#define DATA_SIZE 32

const char RE_AUTH[] PROGMEM = "Authorization: Basic ";

class uHTTP{

  public:
    uHTTP(EthernetClient& client);
    ~uHTTP();

    char *method();
    char *uri();
    char *uri(uint8_t segment);
    char *auth();
    char *data(const char *key);

  private:
    EthernetClient _client;

    char *_method = new char[METHOD_SIZE];
    char *_uri = new char[URI_SIZE];
    char *_auth = new char[AUTH_SIZE];
    char *_data = new char[DATA_SIZE];

    void _parse();
};

#endif