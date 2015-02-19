/*
  uHTTP.cpp - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#include "uHTTP.h"

uHTTP::uHTTP(){
  _uri = new char[URI_SIZE];
  _query = new char[QUERY_SIZE];
  _body = new char[BODY_SIZE];

  server = new EthernetServer(80);
  server->begin();
}

uHTTP::uHTTP(uint16_t port){
  _uri = new char[URI_SIZE];
  _query = new char[QUERY_SIZE];
  _body = new char[BODY_SIZE];

  server = new EthernetServer(port);
  server->begin();
}

uHTTP::~uHTTP(){
  delete [] _uri;
  delete [] _query;
  delete [] _body;

  delete server;
}

EthernetClient *uHTTP::process(){
  memset(_uri, 0, sizeof _uri);
  memset(_query, 0, sizeof _query);
  memset(_body, 0, sizeof _body);
  memset(&_head, 0, sizeof _head);

  if(client = server->available()){
    typedef enum {METHOD, URI, QUERY, PROTO, KEY, VALUE, BODY, STOP} states;
    states state = METHOD;
    
    #ifdef uHTTP_CORS
    typedef enum {NONE, AUTHORIZATION, CONTENT_TYPE, CONTENT_LENGTH, ORIGIN} headers;
    #else
    typedef enum {NONE, AUTHORIZATION, CONTENT_TYPE, CONTENT_LENGTH} headers;
    #endif
    headers header = NONE;

    uint8_t cursor = 0;
    uint8_t cr = 0;

    bool sub = false;
    char key[HEAD_KEY_SIZE] = {0};
    char buffer[HEAD_VAL_SIZE] = {0};

    while(client.connected()){
      while(client.available() > 0){
        char c = client.read();

        (c == '\r' || c == '\n') ? cr++ : cr = 0;

        switch(state){
          case METHOD:
            if(c == ' '){
              if(strncmp_P(buffer, PSTR("OP"), 2) == 0) _method = OPTIONS;
              else if(strncmp_P(buffer, PSTR("HE"), 2) == 0) _method = HEAD;
              else if(strncmp_P(buffer, PSTR("PO"), 2) == 0) _method = POST;
              else if(strncmp_P(buffer, PSTR("PU"), 2) == 0) _method = PUT;
              else if(strncmp_P(buffer, PSTR("PA"), 2) == 0) _method = PATCH;
              else if(strncmp_P(buffer, PSTR("DE"), 2) == 0) _method = DELETE;
              else if(strncmp_P(buffer, PSTR("TR"), 2) == 0) _method = TRACE;
              else if(strncmp_P(buffer, PSTR("CO"), 2) == 0) _method = CONNECT;
              else _method = GET;
              state = URI; cursor = 0; 
              memset(buffer, 0, HEAD_VAL_SIZE * sizeof(*buffer));
            }
            else if(cursor < METHOD_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
            break;
          case URI:
            if(c == ' '){ state = PROTO; cursor = 0; }
            else if(c == '?'){ state = QUERY; cursor = 0; }
            else if(cursor < URI_SIZE - 1){ _uri[cursor++] = c; _uri[cursor] = '\0'; }
            break;
          case QUERY:
            if(c == ' '){ state = PROTO; cursor = 0; }
            else if(cursor < QUERY_SIZE - 1){ _query[cursor++] = c; _query[cursor] = '\0'; }
          case PROTO:
            if(cr == 2){ state = KEY; cursor = 0; }
            break;
          case KEY:
            if (cr == 4){ state = BODY; cursor = 0; }
            else if(c == ' '){ cursor = 0; state = VALUE; }
            else if(c != '\r' && c != '\n' && c != ':' && cursor < HEAD_KEY_SIZE - 1){ key[cursor++] = c; key[cursor] = '\0'; }
            break;
          case VALUE:
            if(cr == 2){
              switch(header){
                case AUTHORIZATION:
                  strncpy(_head.auth, buffer, AUTH_SIZE);
                  break;
                case CONTENT_TYPE:
                  if(strcmp_P(buffer, PSTR("text")) == 0){
                    _head.type = TEXT_PLAIN;
                  }else if(strcmp_P(buffer, PSTR("html")) == 0){
                    _head.type = TEXT_HTML;
                  }else if(strcmp_P(buffer, PSTR("form-data")) == 0){
                    _head.type = FORM_DATA;
                  }else if(strncmp_P(buffer, PSTR("x-www-form"), 10) == 0){
                    _head.type = X_WWW_FORM_URLENCODED;
                  }
                  break;
                case CONTENT_LENGTH:
                  _head.length = atoi(buffer);
                  break;
                #ifdef uHTTP_CORS
                case ORIGIN:
                  strncpy(_head.orig, buffer, ORIG_SIZE);
                  break;
                #endif
              }
              state = KEY; header = NONE; cursor = 0; sub = false;
              memset(buffer, 0, HEAD_VAL_SIZE * sizeof(*buffer));
            }else if(c != '\r' && c != '\n'){
              if(strcmp_P(key, PSTR("Authorization")) == 0){
                if(header != AUTHORIZATION) header = AUTHORIZATION;
                if(sub && cursor < AUTH_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
                else if(c == ' ') sub = true;
              }
              else if(strcmp_P(key, PSTR("Content-Type")) == 0){
                if(header != CONTENT_TYPE) header = CONTENT_TYPE;
                if(sub && cursor < HEAD_VAL_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
                else if(c == '/') sub = true;
              }
              else if(strcmp_P(key, PSTR("Content-Length")) == 0){
                if(header != CONTENT_LENGTH) header = CONTENT_LENGTH;
                if(cursor < HEAD_VAL_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
              }
              
              #ifdef uHTTP_CORS
              else if(strcmp_P(key, PSTR("Origin")) == 0){
                if(header != ORIGIN) header = ORIGIN;
                if(cursor < ORIG_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0';}
              }
              #endif
            }
            break;
          case BODY:
            if(cr == 2 || _head.length == 0) state = STOP;
            else if(cursor < BODY_SIZE - 1){ _body[cursor++] = c; _body[cursor] = '\0'; }
            break;
          case STOP:
            client.flush();
            break;
        }
      }
      return &client;
    }
  }
  return NULL;
}

uint8_t uHTTP::method(){
  return _method;
}

char *uHTTP::uri(){
  return _uri;
}

char* uHTTP::uri(uint8_t index){
  char *act, *segment, *ptr;
  static char copy[URI_SIZE];
  uint8_t i;
  strcpy(copy, _uri);
  for(i = 1, act = copy; i <= index; i++, act = NULL) {
    segment = strtok_rP(act, PSTR("/"), &ptr);
    if(segment == NULL) break;
  }
  return segment;
}

bool uHTTP::uri_equals(const char *uri){
  return (strcmp(this->uri(), uri) == 0) ? true : false;
}

bool uHTTP::uri_equals(const __FlashStringHelper *uri){
  return (strcmp_P(this->uri(), (const PROGMEM char *) uri) == 0) ? true : false;
}

bool uHTTP::uri_equals(uint8_t index, const char *uri){
  return (strcmp(this->uri(index), uri) == 0) ? true : false;
}

bool uHTTP::uri_equals(uint8_t index, const __FlashStringHelper *uri){
  return (strcmp_P(this->uri(index), (const PROGMEM char *) uri) == 0) ? true : false;
}

Header uHTTP::head(){
  return _head;
}

char *uHTTP::query(){
  return _query;
}

char *uHTTP::query(const char *key){
  char *act, *sub, *ptr;
  static char copy[QUERY_SIZE];
  strcpy(copy, _query);
  for (act = copy; strncmp(sub, key, strlen(key)); act = NULL) {
    sub = strtok_rP(act, PSTR("&"), &ptr);
    if (sub == NULL) break;
  }
  if(sub != NULL) return strchr(sub, '=') + 1;
  return NULL;
}

char *uHTTP::query(const __FlashStringHelper *key){
  char *act, *sub, *ptr;
  static char copy[QUERY_SIZE];
  strcpy(copy, _query);

  for(act = copy; strncmp_P(sub, (const PROGMEM char *) key, strlen_P((const PROGMEM char *) key)); act = NULL) {
    sub = strtok_rP(act, PSTR("&"), &ptr);
    if (sub == NULL) break;
  }
  if(sub != NULL) return strchr(sub, '=') + 1;
  return NULL;
}

char *uHTTP::body(){
  return _body;
}

char *uHTTP::data(){
  return _body;
}

char *uHTTP::data(const char *key){
  char *act, *sub, *ptr;
  static char copy[BODY_SIZE];
  strcpy(copy, _body);
  for (act = copy; strncmp(sub, key, strlen(key)); act = NULL) {
    sub = strtok_r(act, "&", &ptr);
    if (sub == NULL) break;
  }
  if(sub != NULL) return strchr(sub, '=') + 1;
  return NULL;
}

char *uHTTP::data(const __FlashStringHelper *key){
  char *act, *sub, *ptr;
  static char copy[BODY_SIZE];
  strcpy(copy, _body);
  for(act = copy; strncmp_P(sub, (const PROGMEM char *) key, strlen_P((const PROGMEM char *) key)); act = NULL) {
    sub = strtok_rP(act, PSTR("&"), &ptr);
    if (sub == NULL) break;
  }
  if(sub != NULL) return strchr(sub, '=') + 1;
  return NULL;
}
