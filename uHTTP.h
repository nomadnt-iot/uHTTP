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

// Method types
#define METHOD_UNKNOWN  0
#define METHOD_OPTIONS  1
#define METHOD_GET      2
#define METHOD_HEAD     3
#define METHOD_POST     4
#define METHOD_PUT      5
#define METHOD_PATCH    6
#define METHOD_DELETE   7
#define METHOD_TRACE    8
#define METHOD_CONNECT  9

// Header types
#define HEADER_AUTH     0
#define HEADER_ORIG     1

// Sizes
#define METHOD_SIZE     8
#define URI_SIZE        16
#define DATA_SIZE       32
#define HEAD_SIZE       32
#define BODY_SIZE       32

typedef struct Header{
  char auth[HEAD_SIZE] = {0};
  char orig[HEAD_SIZE] = {0};
};

class uHTTP{

  public:
    uHTTP(EthernetClient& client);
    ~uHTTP();

    uint8_t method();
    char *uri();
    char *uri(uint8_t segment);
    char *data();
    char *data(const char *key);
    char *body();

    Header head();
    char *head(uint8_t header);

  private:
    uint8_t _method;
    Header _head;
    char *_uri = new char[URI_SIZE]();
    char *_data = new char[DATA_SIZE]();
    char *_body = new char[BODY_SIZE]();
};

#endif