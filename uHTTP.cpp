/*
  uHTTP.cpp - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#include "uHTTP.h"

uHTTP::uHTTP(EthernetClient& client){
  _client = client;

  uint8_t head = 0;
  uint8_t cursor = 0;
  uint8_t space = 0;
  uint8_t cr = 0;
  bool data = false;
  bool auth = false;

  // Initializing buffer of RE_AUTH
  char buffer[128] = {0};
  
  while(_client.available() > 0){
    char c = _client.read();

    if((cr / 4) == 0){
      // Qui sono nell header
      if(head == 0){
        // First header line
        if(c != ' '){
          if(space == 0){
            if(cursor < METHOD_SIZE){ _method[cursor++] = c; _method[cursor] = '\0'; }
          }else if(space == 1){
            if(c == '?' && strcmp_P(_method, PSTR("GET")) == 0){ data = true; cursor = 0; continue; }

            // GET request data
            if(data && cursor < DATA_SIZE){ _data[cursor++] = c; _data[cursor] = '\0'; }
            else if(cursor < URI_SIZE){ _uri[cursor++] = c; _uri[cursor] = '\0'; }
          }
        }else{ space++; cursor = 0; }
      }else{
        // Rest of header lines
        if(c != '\r' && c != '\n' && cursor < 128){
          buffer[cursor++] = c; buffer[cursor] = '\0';
        }else{
          if(cr == 1){            
            if(strncmp_P(buffer, PSTR("Origin: "), 8) == 0){
              strcpy(_orig, buffer + 8);
            }else if(strncmp_P(buffer, PSTR("Authorization: Basic "), 21) == 0){
              strcpy(_auth, buffer + 21);
            }
          }
        }
      }

      if(c == '\r' || c == '\n'){
        cr++; if(cr / 2){ head++; cursor = 0; }
      }else cr = 0;

    }else{
      // Qui sono nel body
      if(strcmp_P(_method, PSTR("POST")) == 0 || strcmp_P(_method, PSTR("PUT")) == 0){
        if(cursor < DATA_SIZE){ _data[cursor++] = c; _data[cursor] = '\0'; }
      }
    }
  }

  _client.flush();
}

uHTTP::~uHTTP(){
  delete [] _method;
  delete [] _uri;
  delete [] _auth;
  delete [] _orig;
  delete [] _data;
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

char *uHTTP::auth(){
  return _auth;
}

char *uHTTP::orig(){
  return _orig;
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