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

byte macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
byte ip4addr[4] = {192, 168, 0, 254};

uHTTP *server = new uHTTP(80);

void setup(){
	Serial.begin(115200);
  Ethernet.begin(macaddr, ip4addr);

  Serial.print(F("Starting uHTTP at "));
  Serial.print(Ethernet.localIP());
  Serial.println(":80");

  server->begin();
}

void loop(){
	EthernetClient response = server->available();
	if(response){
    header_t head = server->head();

    // You can get request method:
    Serial.print(F("METHOD: "));
    Serial.println(server->method());
    // Or you can get complete uri:
    Serial.print(F("URI: "));
    Serial.println(server->uri());
    // Or you can get only first segment from uri:
    Serial.print(F("Segment[1]: "));
    Serial.println(server->uri(1));
    // Or you can get only second segment from uri:
    Serial.print(F("Segment[2]: "));
    Serial.println(server->uri(2));
    
    // Or check if url is equals to the passed value
    if(server->uri(F("foo/bar"))){
    	// do something here
    }

    // Or check if only a segment of url is equals to the passed value
    if(server->uri(1, F("foo"))){
    	// do something here
    }

    // Or you can get query string:
    Serial.print(F("QUERY: "));
    Serial.println(server->query());
    // Or you can get variable from GET data:
    Serial.print(F("QUERY[foo]: "));
    Serial.println(server->query(F("foo")));
    // Or you can get post data:
    if(server->method(uHTTP_METHOD_POST)){
      Serial.print(F("DATA[foo]: "));
      Serial.println(server->data(F("foo")));
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

    response.println("HTTP/1.1 200 OK");
    response.println("Content-Type: text/plain");
    response.println();
    response.println("Hello World!");
    response.stop();
  }
}