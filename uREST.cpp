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
  char *val = request->uri(2);

  //Serial.println(*key);
  //Serial.println(*val);
  //Serial.println(key);
  //Serial.println(val);

  //if(key != NULL && has(key.toInt()) == true){
  if(*key != 0){
    // Setter
    if(*val != 0){
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
    }else{  // Getter
      char buffer[255] = {0};             // Buffer will contain json output

      if(key[0] == 'A'){
        uint8_t pin = key[1] - '0';
        uint8_t val = analogRead(pin);
        sprintf(buffer, "{\"id\": \"A%i\", \"value\": %i}", pin, val);
      }else if(key[0] != NULL){
        uint8_t pin = key[0] - '0';
        const char *val = (digitalRead(pin) == 0) ? "LOW" : "HIGH";
        sprintf(buffer, "{\"id\": %i, \"value\": \"%s\"}", pin, val);
      }
      
      request->render(F("200 OK"), buffer);
    }
  }else{
    request->render(F("404 Not Found"), NULL);
  }

  free(key);
  free(val);
  delete request;
  return true;
}

bool uREST::has(uint8_t pin){
  int i = 0;
  // non si sa perchè come ultimo carattere ho sempre "80"
  while(_pin[i] != '\0'){
    if(_pin[i++] == pin) return true;
    //Serial.println(_pin[i++]);
  }
  return false;
}