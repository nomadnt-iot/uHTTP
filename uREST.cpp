/*
  uREST.h - Library that implement REST like interface.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#include "uREST.h"

uREST::uREST(uint8_t *pin){
  _pin = pin;
}

bool uREST::process(EthernetClient& client){
  uHTTP *request = new uHTTP(client);
  char *key = request->uri(1);

  if(key != NULL){
    char *val = request->uri(2);

    if(val != NULL){
      uint8_t pin = atoi(key);
      if(strcmp("HIGH", val) == 0){
        digitalWrite(pin, HIGH);
      }else if(strcmp("LOW", val) == 0){
        digitalWrite(pin, LOW);
      }else if(strcmp("INPUT", val) == 0){
        pinMode(pin, INPUT);
      }else if(strcmp("OUTPUT", val) == 0){
        pinMode(pin, OUTPUT);
      }else{
        analogWrite(pin, atoi(val));
      }
      request->render(F("200 OK"), NULL);
    }else{
      char buffer[JSON_SIZE] = {0};             // Buffer will contain json output
      char *cb = request->data("cb");

      if(key[0] == 'A'){
        uint8_t pin = key[1] - '0';
        uint8_t val = analogRead(pin);
        sprintf(buffer, "%s({\"id\": \"A%i\", \"value\": %i});", cb, pin, val);
      }else if(key[0] != NULL){
        uint8_t pin = key[0] - '0';
        const char *val = (digitalRead(pin) == 0) ? "LOW" : "HIGH";
        sprintf(buffer, "%s({\"id\": %i, \"value\": \"%s\"});", cb, pin, val);
      }
      
      request->render(F("200 OK"), buffer);
    }
  }else{
    request->render(F("404 Not Found"), NULL);
  }

  delete request;
}