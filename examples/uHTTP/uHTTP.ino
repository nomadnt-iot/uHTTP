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

#include <UIPEthernet.h>
#include <uHTTP.h>

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
		// Or you can get query string:
		Serial.print(F("QUERY STRING: "));
		Serial.println(request->data());
		// Or you can get variable from GET/POST/PUT/DELETE data:
		Serial.print(F("DATA[username]: "));
		Serial.println(request->data("username"));
		// Or you can get Authorization token:
		Serial.print(F("Authorization: "));
		Serial.println(request->head(HEADER_AUTH));
		// Or you can get Origin header:
		Serial.print(F("Origin: "));
		Serial.println(request->head(HEADER_ORIG));
		// Or you can get body:
		Serial.print(F("BODY: "));
		Serial.println(request->body());
  		#endif

  		client.println(F("HTTP/1.1 200 OK"));
  		client.println(F("content-type: text/plain"));
  		client.println();
  		client.stop();

  		delete request;
	}
}
