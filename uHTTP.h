/*
  uHTTP.h - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#ifndef uHTTP_H
#define uHTTP_H

#include "Arduino.h"
#include "../UIPEthernet/UIPEthernet.h"

#define DEBUG 0
#define BUFSIZE 255

class uHTTP{

  public:
    uHTTP(EthernetClient& client);

    char *method();
    char *uri();
    char *proto();
    char *body();

    void render(const char *status, const char *body);
    void render(const __FlashStringHelper *status, const char *body);

  private:
    EthernetClient _client;

    char _method[8] = {'\0'};
    char _uri[16] = {'\0'};
    char _proto[12] = {'\0'};
    char _body[BUFSIZE] = {'\0'};

    void _parse();
};

#endif