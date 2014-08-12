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

void uHTTP::_parse(){
  bool head = true;                 // Initialiting in head
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
            _uri[i++] = c;
            _uri[i] = '\0';
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
      if(cr / 4 && i < BODY_SIZE){
        _body[i++] = c;
        _body[i] = '\0';  
      }else{
        (c != '\n' && c != '\r') ? cr = 0 : cr++;
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

char *uHTTP::uri(uint8_t segment){
  if(segment > 0){
    char *buffer = new char[URI_SIZE];
    uint8_t i = 0;
    uint8_t c = 0;
    uint8_t s = 0;

    //buffer = (char*)malloc(sizeof(char) * URI_SIZE);

    while(_uri[i] != '\0'){
      if(_uri[i] != '/'){
        if(segment == s && c < URI_SIZE){
          buffer[c++] = _uri[i];
          buffer[c] = '\0';
        }
      }else{
        s++;
      }
      i++;
    }
    return buffer;
  }
  return _uri;
}

char *uHTTP::body(){
  return _body;
}

void uHTTP::render(const char *status, const char *body){
  _client.print(F("HTTP/1.1 "));
  _client.print(status);
  _client.print(F("\r\n"));
  _client.print(F("content-type: application/json\r\n\r\n"));
  _client.print(body);
  _client.print(F("\r\n"));
  _client.stop();
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