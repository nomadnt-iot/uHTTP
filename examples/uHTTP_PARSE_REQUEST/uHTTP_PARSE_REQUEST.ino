/**
 * HTTP Parser
 * Parse an HTTP request and print to serial the following values:
 *
 * method: the requested method (GET HEAD POST PUT PATCH DELETE OPTIONS)
 * uri : the requested url
 * data: the requested data if GET query string ? data if POST/PUT body data
 * 
 * This example code is in the public domain.
 **/

#include <SPI.h>
#include <Ethernet.h>
#include <uHTTP.h>

const PROGMEM char INDEX[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nHello uHTTP client!";

byte macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x66};
byte ip4addr[4] = {192, 168, 0, 254};

uHTTP *server = new uHTTP(80);
EthernetClient response;

void setup(){
	Serial.begin(115200);

    Ethernet.begin(macaddr, ip4addr);

    Serial.print(F("Starting uHTTP at "));
    Serial.print(Ethernet.localIP());
    Serial.println(":80");

    server->begin();
}

void loop(){
	if((response = server->available())){
        header_t head = server->head();        

        // Or check if url is equals to the passed value
        if(server->uri(F("foo/bar"))){
            // Get complete uri:
            Serial.println(server->uri());
        }

        // Or check if only a segment of url is equals to the passed value
        if(server->uri(1, F("foo"))){
        	// Get only first segment from uri:
            Serial.print(F("Segment[1]: "));
            Serial.println(server->uri(1));
        }

        // Or you can get query string:
        Serial.print(F("QUERY: "));
        Serial.println(server->query());

        // Or you can get variable from GET data:
        Serial.print(F("QUERY[foo]: "));
        Serial.println(server->query(F("foo")));
        Serial.print(F("QUERY[bar]: "));
        Serial.println(server->query(F("bar")));
        Serial.print(F("QUERY[poo]: "));
        Serial.println(server->query(F("poo")));

        // Or you can get post data:
        if(server->method(uHTTP_METHOD_POST)){
          Serial.print(F("DATA[foo]: "));
          Serial.println(server->data(F("foo")));
          Serial.print(F("DATA[bar]: "));
          Serial.println(server->data(F("bar")));
          Serial.print(F("DATA[poo]: "));
          Serial.println(server->data(F("poo")));
        }
        
        // Or you can get the Content-Type:
        Serial.print(F("Content-Type: "));
        Serial.println(head.type);

        // Or you can get the Content-Length:
        Serial.print(F("Content-Length: "));
        Serial.println(head.length);

        // Or you can get Authorization token:
        Serial.print(F("Authorization: "));
        Serial.println(head.auth);

        // Or you can get Origin for CORS:
        Serial.print(F("Origin: "));
        Serial.println(head.orig);

        // Or you can get body:
        Serial.print(F("BODY: "));
        Serial.println(server->body());

        response.println((const __FlashStringHelper *)INDEX);
        
        response.stop();
    }
}