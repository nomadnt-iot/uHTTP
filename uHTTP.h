/*
  uHTTP.h - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#ifndef uHTTP_H
#define uHTTP_H

// #define uHTTP_uIP
// #define uHTTP_CORS

#include <Arduino.h>
#ifdef uHTTP_uIP
#include <UIPEthernet.h>
#else
#include <SPI.h>
#include <Ethernet.h>
#endif

// Sizes
#define METHOD_SIZE     8
#define URI_SIZE        32
#define QUERY_SIZE      32
#define AUTH_SIZE       16

#ifdef uHTTP_CORS
#define ORIG_SIZE       16
#endif

#define BODY_SIZE       64

#define HEAD_KEY_SIZE   16
#define HEAD_VAL_SIZE   32

typedef enum content_t { TEXT_PLAIN, TEXT_HTML, X_WWW_FORM_URLENCODED, FORM_DATA };
typedef enum method_t  { OPTIONS, GET, HEAD, POST, PUT, PATCH, DELETE, TRACE, CONNECT };

typedef struct Header{
  content_t type;
  char auth[AUTH_SIZE];
  #ifdef uHTTP_CORS
  char orig[ORIG_SIZE];
  #endif
  unsigned int length;
};

class uHTTP{

  public:
    uHTTP(uint16_t port);
    uHTTP();
    ~uHTTP();

    EthernetClient *process();

    uint8_t method();
    char *uri();
    char *uri(uint8_t segment);
    
    char *query();
    char *query(const char *key);
    char *query(const __FlashStringHelper *key);

    char *body();
    char *data();
    char *data(const char *key);
    char *data(const __FlashStringHelper *key);

    Header head();

    bool uri_equals(const char *uri);
    bool uri_equals(const __FlashStringHelper *uri);
    bool uri_equals(uint8_t index, const char *uri);
    bool uri_equals(uint8_t index, const __FlashStringHelper *uri);

  private:
    EthernetServer *server;
    EthernetClient client;

    Header _head;

    method_t _method;
    char *_uri;
    char *_query;
    char *_body;
};

#endif