#ifndef uHTTP_H
#define uHTTP_H

#include "Arduino.h"
#include "EthernetClient.h"
#include "EthernetServer.h"

// #define uHTTP_DEBUG

// Sizes
#define uHTTP_BUFFER_SIZE    255
#define uHTTP_METHOD_SIZE    8
#define uHTTP_URI_SIZE       32
#define uHTTP_QUERY_SIZE     32
#define uHTTP_AUTH_SIZE      32
#define uHTTP_TYPE_SIZE      34
#define uHTTP_ORIG_SIZE      16
#define uHTTP_BODY_SIZE      64

#define uHTTP_METHOD_OPTIONS 0
#define uHTTP_METHOD_GET     1
#define uHTTP_METHOD_HEAD    2
#define uHTTP_METHOD_POST    3
#define uHTTP_METHOD_PUT     4
#define uHTTP_METHOD_PATCH   5
#define uHTTP_METHOD_DELETE  6
#define uHTTP_METHOD_TRACE   7
#define uHTTP_METHOD_CONNECT 8

namespace uhttp{

    typedef struct header_t{
        char type[uHTTP_TYPE_SIZE];
        char auth[uHTTP_AUTH_SIZE];
        char orig[uHTTP_ORIG_SIZE];
        uint16_t length;
    };

    class uHTTP : public EthernetServer {
        private:
            const __FlashStringHelper *__name;

            header_t __head;

            uint8_t __method;

            char *__uri;
            char *__query;
            char *__body;

            char *parse(const char *needle, char *haystack, const __FlashStringHelper *sep);
            char *parse(const __FlashStringHelper *needle, char *haystack, const __FlashStringHelper *sep);

        public:
            uHTTP();
            uHTTP(uint16_t port);
            ~uHTTP();

            EthernetClient available();

            header_t head();

            uint8_t method();
            bool method(uint8_t type);

            char *uri();
            char *uri(uint8_t segment);
            bool uri(const char *uri);
            bool uri(const __FlashStringHelper *uri);
            bool uri(uint8_t index, const char *uri);
            bool uri(uint8_t index, const __FlashStringHelper *uri);
            
            char *query();
            char *query(const char *key);
            char *query(const __FlashStringHelper *key);

            char *body();
            char *data(const char *key);
            char *data(const __FlashStringHelper *key);
    };

}

#endif