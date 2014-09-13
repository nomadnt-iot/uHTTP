/*
  uHTTP.h - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#ifndef uHTTP_H
#define uHTTP_H

// #define uHTTP_uIP

#include <Arduino.h>
#ifdef uHTTP_uIP
#include <UIPEthernet.h>
#else
#include <SPI.h>
#include <Ethernet.h>
#endif

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
#define HEADER_TYPE     0
#define HEADER_AUTH     1
#define HEADER_ORIG     2

// Sizes
#define METHOD_SIZE     8
#define URI_SIZE        32
#define QUERY_SIZE      32
#define HEAD_SIZE       32
#define BODY_SIZE       64

typedef struct Header{
  char type[HEAD_SIZE];
  char auth[HEAD_SIZE];
  char orig[HEAD_SIZE];
};

class uHTTP{

  public:
    uHTTP(uint16_t port);
    ~uHTTP();

    EthernetClient *process();

    uint8_t method();
    char *uri();
    char *uri(uint8_t segment);
    char *query();
    char *query(const char *key);
    char *body();

    Header head();
    char *head(uint8_t key);

  private:
    EthernetServer *server;
    EthernetClient client;

    Header _head;

    uint8_t _method;
    char *_uri;
    char *_query;
    char *_body;
};

#endif