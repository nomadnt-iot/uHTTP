/**
 * EthernetRequest
 * Parse an HTTP request and print to serial the following values:
 *
 * method: the requested method (GET POST PUT PATCH DELETE)
 * uri : the requested url
 * proto: the requested protocol (HTTP/1.1)
 * body: the body request
 * 
 * This example code is in the public domain.
 **/

#include <UIPEthernet.h>
#include <uHTTP.h>
#include <Base64.h>

#define DEBUG 0

uint8_t macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t port = 80;

IPAddress ipaddr(192, 168, 0, 20);
EthernetServer server = EthernetServer(port);

void setup(){
	Ethernet.begin(macaddr, ipaddr);
	server.begin();

	#if DEBUG > 0
	Serial.begin(9600);
	Serial.print(F("Starting uHTTP at "));
	Serial.print(ipaddr);
	Serial.print(F(":"));
	Serial.println(port);
	#endif
}

void loop(){
	if(EthernetClient client = server.available()){
		uHTTP *request = new uHTTP(client);

		#if DEBUG > 0
		// You can get request method:
		Serial.print(F("METHOD: "));
		Serial.println(request->method());
		// Or you can get complete uri:
		Serial.print(F("URI: "));
		Serial.println(request->uri());
		// Or you can get only first segment from uri:
		Serial.print(F("Segment[1]: "));
		Serial.println(request->uri(1));
		// Or you can get only second segment from uri:
		Serial.print(F("Segment[2]: "));
		Serial.println(request->uri(2));
		// Or you can get variable from GET/POST/PUT/DELETE data:
		Serial.print(F("DATA[username]: "));
		Serial.println(request->data("username"));
		Serial.print(F("Authorization: "));
		Serial.println(request->auth());

		char message[32] = {0};
		base64_decode(message, request->auth(), strlen(request->auth()));
		Serial.println(message);
  		#endif

  		client.println(F("HTTP/1.1 200 OK"));
  		client.println(F("content-type: text/plain"));
  		client.println();
  		client.stop();

  		delete request;
	}
}