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

  uint8_t head = 1;                 // Initialiting in head
  uint8_t i = 0;                    // Index pointer of char array
  uint8_t s = 0;                    // Number of space encountred
  uint8_t cr = 0;                   // Number of carrige return found

  while(_client.available() > 0){
    char c = _client.read();
    if(head){
      if(c != '\n' && c != '\r'){
        if(c != ' '){
          char *var;
          switch(s){
            case 0:                 // No space found mean that I'm on the method
              var = _method;
              break;
            case 1:                 // First space found mean that I'm on the uri
              var = _uri;
              break;
            case 2:                 // Second space found mean that I'm on the protocol
              var = _proto;
              break;
          }
          var[i++] = c;
          var[i] = '\0';
        }else{
          s++;                        // Incrementing space counter
          i = 0;                      // Resetting index for array
        }
      }else{
        i = 0;                        // Resetting index for array
        head = 0;                     // Stop to parse header
      }
    }else{
      if(cr / 4 && i < BUFSIZE){
        _body[i++] = c;
        _body[i] = '\0';  
      }else{
        (c != '\n' && c != '\r') ? cr = 0 : cr++;
      }
    }
  }
}

char *uHTTP::method(){
  return _method;
}

char *uHTTP::uri(){
  return _uri;
}

char *uHTTP::proto(){
  return _proto;
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