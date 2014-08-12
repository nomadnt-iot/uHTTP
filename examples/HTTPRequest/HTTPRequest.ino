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
		request.render(F("200 OK"), NULL);

		#if DEBUG > 0
		Serial.print(F("METHOD: "));
		Serial.println(request.method());
		Serial.print(F("URI: "));
		Serial.println(request.uri());
		Serial.print(F("BODY: "));
		Serial.println(request.body());
		//Serial.print(F("Segment[1]: "));
		//Serial.println(request.uri(1));
		//Serial.print(F("Segment[2]: "));
		//Serial.println(request.uri(2));

		//char *method = request.method();
		//char *uri = request.uri();
		//char *body = request.body();
		char *key = request.uri(1);
		char *val = request.uri(2);

		//Serial.print(F("METHOD: "));
		//Serial.println(method);
		//Serial.print(F("URI: "));
		//Serial.println(uri);
		//Serial.print(F("BODY: "));
		//Serial.println(body);
		Serial.print(F("Segment[1]: "));
		Serial.println(key);
		Serial.print(F("Segment[2]: "));
		Serial.println(val);

		// E' necessario liberare la memoria altrimenti si fotte tutto.
		free(key);
		free(val);
  		#endif
	}
	delay(10);
}