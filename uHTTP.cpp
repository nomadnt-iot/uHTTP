/*
  uHTTP.cpp - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#include "uHTTP.h"

uHTTP::uHTTP(uint16_t port){
  this->_uri = new char[URI_SIZE];
  this->_query = new char[QUERY_SIZE];
  this->_body = new char[BODY_SIZE];

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

    uint8_t cursor = 0;
    uint8_t cr = 0;

    char method[METHOD_SIZE] = {0};

    bool sub = false;
    char key[HEAD_SIZE] = {0};

    while(client.available() > 0){
      char c = client.read();

      (c == '\r' || c == '\n') ? cr++ : cr = 0;

      switch(state){
        case METHOD:
          if(c == ' '){
            if(strncmp_P(method, PSTR("OP"), 2) == 0) _method = METHOD_OPTIONS;
            else if(strncmp_P(method, PSTR("GE"), 2) == 0) _method = METHOD_GET;
            else if(strncmp_P(method, PSTR("HE"), 2) == 0) _method = METHOD_HEAD;
            else if(strncmp_P(method, PSTR("PO"), 2) == 0) _method = METHOD_POST;
            else if(strncmp_P(method, PSTR("PU"), 2) == 0) _method = METHOD_PUT;
            else if(strncmp_P(method, PSTR("PA"), 2) == 0) _method = METHOD_PATCH;
            else if(strncmp_P(method, PSTR("DE"), 2) == 0) _method = METHOD_DELETE;
            else if(strncmp_P(method, PSTR("TR"), 2) == 0) _method = METHOD_TRACE;
            else if(strncmp_P(method, PSTR("CO"), 2) == 0) _method = METHOD_CONNECT;
            else _method = METHOD_UNKNOWN;
            state = URI; 
            cursor = 0; 
          }
          else if(cursor < METHOD_SIZE - 1){ method[cursor++] = c; method[cursor] = '\0'; }
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
          if(cr == 4){ state = BODY; cursor = 0; }
          else if(c == ' '){ cursor = 0; state = VALUE; }
          else if(c != '\r' && c != '\n' && c != ':' && cursor < HEAD_SIZE - 1){ key[cursor++] = c; key[cursor] = '\0'; }
          break;
        case VALUE:
          if(cr == 2){ cursor = 0; sub = false; state = KEY;}
          else {
            if(c != '\r' && c != '\n'){
              if(strcmp_P(key, PSTR("Authorization")) == 0){
                if(sub && cursor < HEAD_SIZE - 1){ _head.auth[cursor++] = c; _head.auth[cursor] = '\0'; }
                else if(c == ' ') sub = true;
              }else if(strcmp_P(key, PSTR("Content-Type")) == 0){
                if(sub && cursor < HEAD_SIZE - 1){ _head.type[cursor++] = c; _head.type[cursor] = '\0'; }
                else if(c == '/') sub = true;
              }else if(strcmp_P(key, PSTR("Origin")) == 0){
                if(cursor < HEAD_SIZE - 1){ _head.orig[cursor++] = c; _head.orig[cursor] = '\0'; }
              }
            }
          }
          break;
        case BODY:
          if(cr == 2) state = STOP;
          else if(cursor < BODY_SIZE){ _body[cursor++] = c; _body[cursor] = '\0'; }
          break;
        case STOP:
          client.flush();
          break;
      }
    }

    return &client;
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
    //segment = strtok_rP(act, PSTR("/"), &ptr);
    segment = strtok_r(act, "/", &ptr);
    if(segment == NULL) break;
  }
  return segment;
}

Header uHTTP::head(){
  return _head;
}

char *uHTTP::head(uint8_t header){
  switch(header){
    case HEADER_TYPE:
      return _head.type;
      break;
    case HEADER_AUTH:
      return _head.auth;
      break;
    case HEADER_ORIG:
      return _head.orig;
      break;
    default:
      return NULL;
      break;
  }
}

char *uHTTP::query(){
  return _query;
}

char *uHTTP::query(const char *key){
  char *act, *sub, *ptr;
  static char copy[QUERY_SIZE];
  strcpy(copy, _query);
  for (act = copy; strncmp(sub, key, strlen(key)); act = NULL) {
    //sub = strtok_rP(act, PSTR("&"), &ptr);
    sub = strtok_r(act, "&", &ptr);
    if (sub == NULL) break;
  }
  if(sub != NULL) return strchr(sub, '=') + 1;
  return NULL;
}

char *uHTTP::body(){
  return _body;
}