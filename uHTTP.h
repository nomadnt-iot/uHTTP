/*
  uHTTP.h - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#ifndef uHTTP_H
#define uHTTP_H

#include "Arduino.h"
#include "../UIPEthernet/UIPEthernet.h"

// #define DEBUG 0
#define METHOD_SIZE 8
#define QUERY_SIZE 128
#define BODY_SIZE 255
#define URI_SIZE 32

class uHTTP{

  public:
    uHTTP(EthernetClient& client);

    char *method();
    char *uri();
    char *uri(uint8_t segment);
    char *body();

    void render(const char *status, const char *body);
    void render(const __FlashStringHelper *status, const char *body);

  private:
    EthernetClient _client;

    char _method[METHOD_SIZE] = {0};
    char _uri[URI_SIZE] = {0};
    char _body[BODY_SIZE] = {0};

    void _parse();
};

#endif