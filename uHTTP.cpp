/*
  uHTTP.h - Library that implement HTTP request interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#include "uHTTP.h"

uHTTP::uHTTP(EthernetClient& client){
  _client = client;
  this->_parse();
}

uHTTP::~uHTTP(){
  delete [] _method;
  delete [] _uri;
  delete [] _auth;
  delete [] _data;
}

void uHTTP::_parse(){
  uint8_t head = 0;
  uint8_t cursor = 0;
  uint8_t space = 0;
  uint8_t counter = 0;
  uint8_t cr = 0;
  bool data = false;
  bool auth = false;

  // Initializing buffer of RE_AUTH
  char buffer[24]; strcpy_P(buffer, RE_AUTH);
  
  while(_client.available() > 0){
    char c = _client.read();

    if((cr / 4) == 0){
      // Qui sono nell header
      if(head == 0){
        // First header line
        if(c != ' '){
          if(space == 0){
            if(cursor < METHOD_SIZE) _method[cursor++] = c; _method[cursor] = '\0';
          }else if(space == 1){
            if(c == '?' && strcmp_P(_method, PSTR("GET")) == 0){ data = true; cursor = 0; continue; }

            // GET request data
            if(data && cursor < DATA_SIZE){ _data[cursor++] = c; _data[cursor] = '\0'; }
            else{ _uri[cursor++] = c; _uri[cursor] = '\0'; }
          }
        }else{ space++; cursor = 0; }
      }else{
        // Rest of header lines

        if(auth && (c != '\r' && c != '\n')){
          // Qui devo popolare l'array per authorization
          if(cursor < AUTH_SIZE){ _auth[cursor++] = c; _auth[cursor] = '\0'; }
        }else{
          // Count compatible char in RE_AUTH
          if(c == buffer[cursor++]){ counter++; }
          // If the number of chars matched are the same number 
          if(counter / strlen(buffer)){ auth = true; cursor = 0; }
        }

      }

      if(c == '\r' || c == '\n'){
        cr++;
        if(cr / 2){ head++; cursor = 0; counter = 0; auth = false; }
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