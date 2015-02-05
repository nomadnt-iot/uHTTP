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

uint8_t macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
IPAddress ipaddr(192, 168, 10, 254);

uHTTP *HTTP;
EthernetClient *response;

void setup(){
	Ethernet.begin(macaddr, ipaddr);
	
	Serial.begin(9600);
	Serial.print(F("Starting uHTTP at "));
	Serial.print(ipaddr);
	Serial.println(F(":80"));

	HTTP = new uHTTP(80);
}

void loop(){

	if((response = HTTP->process())){
		Header head = HTTP->head();

		// You can get request method:
		Serial.print(F("METHOD: "));
		Serial.println(HTTP->method());
		// Or you can get complete uri:
		Serial.print(F("URI: "));
		Serial.println(HTTP->uri());
		// Or you can get only first segment from uri:
		Serial.print(F("Segment[1]: "));
		Serial.println(HTTP->uri(1));
		// Or you can get only second segment from uri:
		Serial.print(F("Segment[2]: "));
		Serial.println(HTTP->uri(2));
		// Or you can get query string:
		Serial.print(F("QUERY: "));
		Serial.println(HTTP->query());
		// Or you can get variable from GET data:
		Serial.print(F("QUERY[foo]: "));
		Serial.println(HTTP->query("foo"));
		// Or you can get post data:
		Serial.print(F("DATA: "));
		Serial.println(HTTP->data());
		// Or you can get variable from POST/PUT/DELETE data:
		Serial.print(F("DATA[foo]: "));
		Serial.println(HTTP->data("foo"));
		// Or you can get the Content-Type:
		Serial.print(F("Content-Type: "));
		Serial.println(head.type);
		// Or you can get the Content-Type:
		Serial.print(F("Content-Length: "));
		Serial.println(head.length);
		// Or you can get Authorization token:
		Serial.print(F("Authorization: "));
		Serial.println(head.auth);
		// Or you can get Origin header:
		Serial.print(F("Origin: "));
		Serial.println(head.orig);
		// Or you can get body:
		Serial.print(F("BODY: "));
		Serial.println(HTTP->body());

		response->println(F("HTTP/1.1 200 OK"));
		response->println(F("Content-Type"));
		response->println();
		response->println(F("Hello World!"));
		response->stop();
	}
}
