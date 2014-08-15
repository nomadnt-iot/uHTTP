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
  delete [] _data;
}

void uHTTP::_parse(){
  bool head = true;                 // Initialiting in head
  bool data = false;
  uint8_t i = 0;                    // Index pointer of char array
  uint8_t s = 0;                    // Number of space encountred
  uint8_t cr = 0;                   // Number of carrige return found

  while(_client.available() > 0){
    char c = _client.read();
    if(head){
      if(c != '\n' && c != '\r'){
        if(c != ' '){
          if(s == 0){
            _method[i++] = c;
            _method[i] = '\0';
          }else if(s == 1){
            if(c == '?' && strcmp(_method, "GET") == 0){
              data = true;
              i = 0;
              continue;
            }

            if(data){                 // GET request data
              _data[i++] = c;
              _data[i] = '\0';
            }else{
              _uri[i++] = c;
              _uri[i] = '\0';  
            }
          }
        }else{
          s++;                        // Incrementing space counter
          i = 0;                      // Resetting index for array
        }
      }else{
        i = 0;                        // Resetting index for array
        head = false;                 // Stop to parse header
      }
    }else{
      if(strcmp(_method, "POST") == 0 || strcmp(_method, "PUT") == 0){
        if(cr / 4){
          data = true;
        }else{
          (c != '\n' && c != '\r') ? cr = 0 : cr++;
        }

        if(data && i < DATA_SIZE){
          _data[i++] = c;
          _data[i] = '\0';
        }
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
    segment = strtok_r(act, "/", &ptr);
    if(segment == NULL) break;
  }
  return segment;
}

char *uHTTP::data(){
  return _data;
}

char *uHTTP::data(const char *name){
  char *act, *sub, *ptr;
  static char copy[DATA_SIZE];
  strcpy(copy, _data);
  for (act = copy; strncmp(sub, name, strlen(name)); act = NULL) {
    sub = strtok_r(act, "&", &ptr);
    if (sub == NULL) break;
  }
  return strchr(sub, '=') + 1;
}

void uHTTP::render(const __FlashStringHelper *status, const char *body){
  _client.print(F("HTTP/1.1 "));
  _client.print(status);
  _client.print(F("\r\n"));
  _client.print(F("content-type: application/json\r\n\r\n"));
  _client.print(body);
  _client.print(F("\r\n"));
  _client.stop();
}