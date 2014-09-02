/**
 * Rest Interface
 * Parse an HTTP request and use data to buil REST-like interface
 * 
 * This example code is in the public domain.
 **/

#include <uHTTP.h>
#include <EEPROM.h>
#include <UIPEthernet.h>
#include <avr/wdt.h>
#include "Config.h"

#define DEBUG 1
#define JSON_SIZE 64

EthernetServer *server;
EthernetClient client;

struct Config{
  bool configured;
  unsigned int port;
  bool dhcp;
  uint8_t ipaddr[4];
} cfg;

const uint8_t macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
const uint8_t digital[] = {2, 3, 4, 5, 6, 7, 8, 9};				// Allowed digital pins to be used from REST
const uint8_t analog[] = {A0, A1, A2, A3, A4, A5};				// Allowed analog pins to be used from REST
bool _reset = false;

void(* resetFunc) (void) = 0;

void setup(){
	#if DEBUG > 0
	Serial.begin(9600);
	#endif

	configRead(0, cfg);										// Load configuration
	//firstboot();

	if(!cfg.configured){
		#if DEBUG > 0
    	Serial.println(F("First Boot! Writing default config."));
    	#endif
    	cfg = {true, 80, true, {192, 168, 0, 20}};
  		configWrite(0, cfg);
  	}

  	//if(!cfg.dhcp || !Ethernet.begin(macaddr)){
  		IPAddress ipaddr(cfg.ipaddr[0], cfg.ipaddr[1], cfg.ipaddr[2], cfg.ipaddr[3]);
		Ethernet.begin(macaddr, ipaddr);
  	//}

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
		if(strcmp_P(request.method(), PSTR("OPTIONS")) == 0){
			render(F("200 OK"), NULL);
		}

		if(strcmp_P(request.uri(1), PSTR("digital")) == 0){
			digitalCommand(request);
		}else if(strcmp_P(request.uri(1), PSTR("analog")) == 0){
			analogCommand(request);
		}else if(strcmp_P(request.uri(1), PSTR("config")) == 0){
			configCommand(request);
		}
	}else{
		if(_reset){ reboot(); }
	}
	
	delay(10);
}

void digitalCommand(uHTTP &request){
	char buffer[JSON_SIZE];

	uint8_t pin = atoi(request.uri(2));
	if(has_digital(pin)){
		char *value, *mode;		
		if(strcmp_P(request.method(), PSTR("GET")) == 0 || strcmp_P(request.method(), PSTR("PUT")) == 0){
			if(strcmp_P(request.method(), PSTR("PUT")) == 0){
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
			render(F("200 OK"), buffer);
		}else{
			render(F("404 Not Found"), NULL);
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
			client.stop();
		}else{
			render(F("404 Not Found"), NULL);
		}
	}
}

void analogCommand(uHTTP &request){
	char buffer[JSON_SIZE];

	uint8_t pin = atoi(request.uri(2)) + A0;
	if(has_analog(pin)){		
		uint8_t value;
		char *mode;
		if(strcmp_P(request.method(), PSTR("GET")) == 0 || strcmp_P(request.method(), PSTR("PUT")) == 0){
			if(strcmp_P(request.method(), PSTR("PUT")) == 0){
				mode = request.data("mode");
				if(strcmp_P(mode, PSTR("INPUT")) == 0 || strcmp_P(mode, PSTR("OUTPUT")) == 0){
					(strcmp_P(mode, PSTR("INPUT")) == 0) ? pinMode(pin, INPUT) : pinMode(pin, OUTPUT);
				}

				value = atoi(request.data("value"));
				if(value >= 0 && value <= 255) analogWrite(pin, value);
			}
			sprintf_P(buffer, PSTR("{\"id\": %i, \"value\": %i}"), pin - A0, analogRead(pin));
			render(F("200 OK"), buffer);
		}else{
			render(F("404 Not Found"), NULL);
		}
	}else{
		if(strcmp_P(request.method(), PSTR("GET")) == 0){
			// Stampo tutti i pin analogici
			send_headers(F("200 OK"));
			client.print(F("["));
			for(uint8_t i = 0; i < (sizeof(analog)/sizeof(analog[0])); i++){
				sprintf_P(buffer, PSTR("{\"id\": %i, \"value\": %i}"), analog[i], analogRead(analog[i]));
				client.print(buffer);
				if(i < (sizeof(digital)/sizeof(digital[0])) - 1) client.print(F(","));
			}
			client.print(F("]"));
			client.stop();
		}else{
			render(F("404 Not Found"), NULL);
		}
	}
}

void configCommand(uHTTP &request){
	char buffer[JSON_SIZE];

	if(strcmp_P(request.method(), PSTR("GET")) == 0 || strcmp_P(request.method(), PSTR("PUT")) == 0){
		bool changed = false;
		if(strcmp_P(request.method(), PSTR("PUT")) == 0){
			if(request.data("port") && atoi(request.data("port")) != cfg.port){
				cfg.port = atoi(request.data("port"));
				changed = true;
			}
			if(request.data("dhcp") && atoi(request.data("dhcp")) != cfg.dhcp){
				cfg.dhcp = atoi(request.data("dhcp"));
				changed = true;
			}

			if(request.data("ipaddr")){
				uint8_t ip;
				char *data = request.data("ipaddr");
				
				for(uint8_t i = 0; i < 4; i++, data = NULL){
					ip = atoi(strtok_P(data, PSTR(".")));
					if(ip != cfg.ipaddr[i]){
						cfg.ipaddr[i] = ip;
						changed = true;
					}
				}
			}

			if(changed){
				configWrite(0, cfg); 
				_reset = true;
			}
		}
		sprintf_P(buffer, PSTR("{\"port\": %i, \"dhcp\": %i, \"ipaddr\": \"%i.%i.%i.%i\"}"), cfg.port, cfg.dhcp, cfg.ipaddr[0], cfg.ipaddr[1], cfg.ipaddr[2], cfg.ipaddr[3]);
		render(F("200 OK"), buffer);
	}else{
		render(F("404 Not Found"), NULL);
	}
}

bool has_digital(uint8_t value){
	for(uint8_t i = 0; i < (sizeof(digital)/sizeof(digital[0])); i++) if(digital[i] == value) return true;
	return false;
}

bool has_analog(uint8_t value){
	for(uint8_t i = 0; i < (sizeof(analog)/sizeof(analog[0])); i++) if(analog[i] == value) return true;
  	return false;
}

void send_headers(const __FlashStringHelper *status){
	client.print(F("HTTP/1.1 "));
	client.print(status);
	client.print(F("\r\n"));
	client.print(F("content-type: application/json\r\n"));
	client.print(F("access-control-allow-origin: *\r\n"));
	client.print(F("access-control-allow-methods: GET,PUT\r\n"));
	client.print(F("access-control-allow-headers: Content-Type, X-Requested-With\r\n"));
	client.print(F("access-control-max-age: 1000\r\n"));
	client.println();
}

void render(const __FlashStringHelper *status, const char *body){
	send_headers(status);
	client.println(body);
	client.stop();
}

void reboot(){_reset = false; wdt_enable(WDTO_15MS); while(1) {}}
void firstboot(){for(int addr = 0; addr < E2END; addr++) EEPROM.write(addr, 0); }