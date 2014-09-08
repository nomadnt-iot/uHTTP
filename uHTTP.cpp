/*
  uHTTP.cpp - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#include "uHTTP.h"

uHTTP::uHTTP(EthernetClient& client){
  //_client = client;
  typedef enum {METHOD, URI, DATA, PROTO, HEAD, BODY, STOP} parse_state;
  parse_state state = METHOD;

  uint8_t cursor = 0;
  uint8_t cr = 0;

  bool key = true;
  char header[128] = {0};

  while(client.available() > 0){
    char c = client.read();

    (c == '\r' || c == '\n') ? cr++ : cr = 0;

    switch(state){
      case METHOD:
        if(c == ' '){ state = URI; cursor = 0; }
        else if(cursor < METHOD_SIZE - 1){ _method[cursor++] = c; _method[cursor] = '\0'; }
        break;
      case URI:
        if(c == ' '){ state = PROTO; cursor = 0; }
        else if(c == '?'){ state = DATA; cursor = 0; }
        else if(cursor < URI_SIZE - 1){ _uri[cursor++] = c; _uri[cursor] = '\0'; }
        break;
      case DATA:
        if(c == ' '){ state = PROTO; cursor = 0; }
        else if(cursor < DATA_SIZE - 1){ _data[cursor++] = c; _data[cursor] = '\0'; }
      case PROTO:
        if(cr == 2){ state = HEAD; cursor = 0; }
        break;
      case HEAD:
        if(cr == 4){ state = BODY; cursor = 0; }
        else if(cr == 2){ cursor = 0; key = true; }
        else{
          if(key){
            if(c == ' '){ cursor = 0; key = false; }
            else{ header[cursor++] = c; header[cursor] = '\0'; }
          }else{
            if(c != '\r' && c != '\n'){
              if(strcmp_P(header, PSTR("Origin:")) == 0){
                if(cursor < HEAD_SIZE - 1){ _head.orig[cursor++] = c; _head.orig[cursor] = '\0'; }
              }else if(strcmp_P(header, PSTR("Authorization:")) == 0){
                if(cursor < HEAD_SIZE - 1){ _head.auth[cursor++] = c; _head.auth[cursor] = '\0'; }
              }
            }
          }
        }
        break;
      case BODY:
        if(cr == 2) state = STOP;
        else if(cursor < BODY_SIZE){ _body[cursor++] = c; _body[cursor] = '\0'; }
        break;
      case STOP:
        break;
    }
  }
}

uHTTP::~uHTTP(){
  delete [] _method;
  delete [] _uri;
  delete [] _data;
  delete [] _body;
}

char *uHTTP::method(){
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

Header uHTTP::head(){
  return _head;
}

char *uHTTP::head(uint8_t header){
  switch(header){
    case HEADER_AUTH:
      return _head.auth;
      break;
    case HEADER_ORIG:
      return _head.orig;
      break;
  }
  return NULL;
}

char *uHTTP::data(){
  return _data;
}

char *uHTTP::data(const char *key){
  char *act, *sub, *ptr;
  static char copy[DATA_SIZE];
  strcpy(copy, _data);
  for (act = copy; strncmp(sub, key, strlen(key)); act = NULL) {
    sub = strtok_rP(act, PSTR("&"), &ptr);
    if (sub == NULL) break;
  }
  if(sub != NULL) return strchr(sub, '=') + 1;
  return NULL;
}

char *uHTTP::body(){
  return _body;
}