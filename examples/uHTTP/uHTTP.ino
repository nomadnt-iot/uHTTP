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
		Serial.print(F("METHOD: "));
		// You can get request method:
		Serial.println(request->method());
		Serial.print(F("URI: "));
		// Or you can get complete uri:
		Serial.println(request->uri());
		Serial.print(F("Segment[1]: "));
		// Or you can get only first segment from uri:
		Serial.println(request->uri(1));
		Serial.print(F("Segment[2]: "));
		// Or you can get only second segment from uri:
		Serial.println(request->uri(2));
		Serial.print(F("DATA: "));
		// Or you can get all data from GET or POST in x-www-form-urlencoded format:
		Serial.println(request->data());
		// Or you can get only one variable from data:
		Serial.println(request->data("username"));
  		#endif

  		client.println(F("HTTP/1.1 200 OK"));
  		client.println(F("content-type: text/plain"));
  		client.println();
  		client.print(F("Method: "));
  		client.println(request->method());
  		client.print(F("URI: "));
  		client.println(request->uri());
  		client.print(F("DATA: "));
  		client.println(request->data());
  		client.stop();

  		delete request;
	}
}