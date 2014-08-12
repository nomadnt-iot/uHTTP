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

#include <uHTTP.h>
#include <UIPEthernet.h>

#define DEBUG 1

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
		uHTTP request = uHTTP(client);
		request.render();

		#if DEBUG > 0
		Serial.println(request.method());
  		Serial.println(request.uri());
  		Serial.println(request.proto());
  		Serial.println(request.body());
		#endif
	}
}