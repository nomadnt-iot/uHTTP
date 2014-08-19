#include <uHTTP.h>
#include <EEPROM.h>
#include <UIPEthernet.h>
#include "Config.h"

#define DEBUG 1
#define JSON_SIZE 64

EthernetServer *server;
EthernetClient client;

uint8_t macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

uint8_t digital[] = {2, 3, 4, 5, 6, 7, 8, 9};					// Allowed digital pins to be used from REST
uint8_t analog[] = {A0, A1, A2, A3, A4, A5};					// Allowed analog pins to be used from REST

void setup(){
	#if DEBUG > 0
	Serial.begin(9600);
	#endif

	config_read(0, cfg);

	if(!cfg.configured){
		#if DEBUG > 0
    Serial.println(F("First Boot"));
    Serial.println(F("Writing default config"));
    #endif
    cfg = {true, 80, true, {192, 168, 0, 20}, {255, 255, 255, 0}, {0}, {0}};
  	config_write(0, cfg);
  }

	if(!Ethernet.begin(macaddr)){
		IPAddress ip4addr(cfg.ip4addr[0], cfg.ip4addr[1], cfg.ip4addr[2], cfg.ip4addr[3]);
		Ethernet.begin(macaddr, ip4addr);
	}

	server = new EthernetServer(cfg.port);
	server->begin();

	#if DEBUG > 0
	Serial.print(F("Starting uHTTP at "));
	Serial.print(Ethernet.localIP());
	Serial.print(F(":"));
	Serial.println(cfg.port);
	#endif

	// Setting up pin mode for digital and analog
	for(uint8_t i = 0; i < (sizeof(digital)/sizeof(digital[0])); i++) pinMode(digital[i], OUTPUT);
	for(uint8_t i = 0; i < (sizeof(analog)/sizeof(analog[0])); i++) pinMode(analog[i], INPUT);
}

void loop(){
	if(client = server->available()){
		uHTTP request = uHTTP(client);

		if(strcmp_P(request.uri(1), PSTR("digital")) == 0){
			digitalCommand(request);
		}else if(strcmp_P(request.uri(1), PSTR("analog")) == 0){
			analogCommand(request);
		}else if(strcmp_P(request.uri(1), PSTR("config")) == 0){
			configCommand(request);
		}
		client.stop();
	}
	delay(10);
}

void digitalCommand(uHTTP &request){
	char buffer[JSON_SIZE];

	uint8_t pin = atoi(request.uri(2));
	if(has_digital(pin)){
		char *value, *mode;		
		if(strcmp_P(request.method(), PSTR("GET")) == 0 || strcmp_P(request.method(), PSTR("POST")) == 0){
			if(strcmp_P(request.method(), PSTR("POST")) == 0){
				mode = request.data("mode");
				if(strcmp_P(mode, PSTR("INPUT")) == 0 || strcmp_P(mode, PSTR("OUTPUT")) == 0){
					(strcmp_P(mode, PSTR("INPUT")) == 0) ? pinMode(pin, INPUT) : pinMode(pin, OUTPUT);
				}

				value = request.data("value");
				if(strcmp_P(value, PSTR("HIGH")) == 0 || strcmp_P(value, PSTR("LOW")) == 0){
					(strcmp_P(value, PSTR("HIGH")) == 0) ? digitalWrite(pin, HIGH) : digitalWrite(pin, LOW);
				}
			}
			sprintf_P(buffer, PSTR("{\"id\": %i, \"value\": \"%s\"}"), pin, (digitalRead(pin)) ? "HIGH" : "LOW");
			send_headers(F("200 OK"));
			client.print(buffer);
		}else{
			send_headers(F("404 Not Found"));
		}
	}else{
		if(strcmp_P(request.method(), PSTR("GET")) == 0){
			// Stampo tutti i pin digitali
			send_headers(F("200 OK"));
			client.print(F("["));
			for(uint8_t i = 0; i < (sizeof(digital)/sizeof(digital[0])); i++){
				sprintf_P(buffer, PSTR("{\"id\": %i, \"value\": \"%s\"}"), digital[i], (digitalRead(digital[i])) ? "HIGH" : "LOW");
				client.print(buffer);
				if(i < (sizeof(digital)/sizeof(digital[0])) - 1) client.print(F(","));
			}
			client.print(F("]"));
		}else{
			send_headers(F("404 Not Found"));
		}
	}
}

void analogCommand(uHTTP &request){
	char buffer[JSON_SIZE];

	uint8_t pin = atoi(request.uri(2)) + A0;
	if(has_analog(pin)){		
		uint8_t value;
		char *mode;
		if(strcmp_P(request.method(), PSTR("GET")) == 0 || strcmp_P(request.method(), PSTR("POST")) == 0){
			if(strcmp_P(request.method(), PSTR("POST")) == 0){
				mode = request.data("mode");
				if(strcmp_P(mode, PSTR("INPUT")) == 0 || strcmp_P(mode, PSTR("OUTPUT")) == 0){
					(strcmp_P(mode, PSTR("INPUT")) == 0) ? pinMode(pin, INPUT) : pinMode(pin, OUTPUT);
				}

				value = atoi(request.data("value"));
				if(value >= 0 && value <= 255) analogWrite(pin, value);
			}
			sprintf_P(buffer, PSTR("{\"id\": %i, \"value\": %i}"), pin - A0, analogRead(pin));
			send_headers(F("200 OK"));
			client.print(buffer);
		}else{
			send_headers(F("404 Not Found"));
		}
	}else{
		if(strcmp_P(request.method(), PSTR("GET")) == 0){
			// Stampo tutti i pin digitali
			send_headers(F("200 OK"));
			client.print(F("["));
			for(uint8_t i = 0; i < (sizeof(analog)/sizeof(analog[0])); i++){
				sprintf_P(buffer, PSTR("{\"id\": %i, \"value\": %i}"), analog[i], analogRead(analog[i]));
				client.print(buffer);
				if(i < (sizeof(digital)/sizeof(digital[0])) - 1) client.print(F(","));
			}
			client.print(F("]"));
		}else{
			send_headers(F("404 Not Found"));
		}
	}
}

void configCommand(uHTTP &request){
	char buffer[JSON_SIZE];
	char *key = request.uri(2);

	if(key){
		//Serial.print("Stampo");
		//Serial.println(key);
	}else{
		if(strcmp_P(request.method(), PSTR("GET")) == 0){
			sprintf_P(buffer, PSTR("{\"port\": %i, \"dhcp\": %i, \"macaddr\": \"\" \"ip4addr\": \"%i.%i.%i.%i\"}"), cfg.port, cfg.dhcp, cfg.ip4addr[0], cfg.ip4addr[1], cfg.ip4addr[2], cfg.ip4addr[3]);
			send_headers(F("200 OK"));
			client.print(buffer);
		}else{
			send_headers(F("404 Not Found"));
		}
	}
}

bool has_digital(uint8_t value){
  for(uint8_t i = 0; i < (sizeof(digital)/sizeof(digital[0])); i++){
  	if(digital[i] == value) return true;
	}
  return false;
}

bool has_analog(uint8_t value){
  for(uint8_t i = 0; i < (sizeof(analog)/sizeof(analog[0])); i++){
  	if(analog[i] == value) return true;
	}
  return false;
}

void send_headers(const __FlashStringHelper *status){
	client.print(F("HTTP/1.1 "));
	client.print(status);
	client.print(F("\r\n"));
	client.print(F("content-type: application/json\r\n"));
	client.print(F("access-control-allow-origin: *\r\n"));
	client.print(F("access-control-allow-methods: PUT\r\n"));
	client.print(F("access-control-max-age: 1000\r\n"));
	client.println();
}

void erase_eeprom(){
	for(int addr = 0; addr < E2END; addr++) EEPROM.write(addr, 0);
}