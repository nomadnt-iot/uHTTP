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
  String uri = String(request->uri());
  String key = uri.substring(uri.indexOf('/') + 1, uri.indexOf('/', uri.indexOf('/') + 1));
  String val = uri.substring(uri.indexOf(key) + key.length() + 1, uri.indexOf('\0', uri.indexOf('/')) + 1);
  
  //if(key != NULL && has(key.toInt()) == true){
  if(key != NULL){
    // Setter
    if(val != NULL){
      uint8_t pin = key.toInt();

      if(val.equals("HIGH") || val.equals("LOW") || val.equals("INPUT") || val.equals("OUTPUT")){
        if(val.equals("HIGH")) digitalWrite(pin, HIGH);
        if(val.equals("LOW")) digitalWrite(pin, LOW);
        if(val.equals("INPUT")) pinMode(pin, INPUT);
        if(val.equals("OUTPUT")) digitalWrite(pin, OUTPUT);
      }else{
        analogWrite(pin, val.toInt());
      }

      request->render(F("200 OK"), NULL);
    }else{  // Getter
      char buffer[255];               // Buffer will contain json output

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