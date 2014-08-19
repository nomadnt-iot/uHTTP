/**
 *
 *
 * METHOD	URI 					Commend
 *
 * GET		/<PIN>/					Get pin mode and their value
 * GET		/<PIN>/<HIGH|LOW>		Set pin value to HIGH or LOW
 * GET		/<PIN>/<0-255>			Set pin value from 0 to 255
 * GET		/<PIN>/<OUTPUT|INPUT>	Set pin mode to OUTPUT or INPUT
 *
 **/

#include <uREST.h>
#include <EEPROM.h>
#include <UIPEthernet.h>

#define DEBUG 1

uint8_t macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t port = 80;

IPAddress ipaddr(192, 168, 0, 20);
EthernetServer server = EthernetServer(port);

uint8_t pin[] = {3, 4, 5, 6, 7, 8, 9};					// Allowed Pins to be used from uREST
uREST resource = uREST(pin);

void setup(){
  Ethernet.begin(macaddr, ipaddr);
  server.begin();

  // Setting pin mode to OUTPUT
  for(uint8_t i = 0; i < (sizeof(pin)/sizeof(pin[0])); i++){
    pinMode(pin[i], OUTPUT);
  }

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
		resource.process(client);
	}
}