/**
 * uHTTP Web server implementation with SD DOCROOT
 * Check if file exists on SD and render it.
 *
 * This example code is in the public domain.
 **/
#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>
#include <uHTTP.h>

#define SDC_PIN 4

byte macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x66};
byte ip4addr[4] = {192, 168, 0, 254};

uHTTP *server = new uHTTP(80);
EthernetClient response;

void setup(){
	Serial.begin(115200);

    Serial.print(F("Initializing SD card...\t\t"));
    if(!SD.begin(SDC_PIN)){
        Serial.println(F("[FAIL]"));
        return;
    }
    Serial.println(F("[DONE]"));

    Ethernet.begin(macaddr, ip4addr);

    Serial.print(F("Starting uHTTP at "));
    Serial.print(Ethernet.localIP());
    Serial.println(":80");

    server->begin();
}

void loop(){
	if((response = server->available())){
        char url[32] = "/index.htm";
        if(strlen(server->uri())) strcpy(url, server->uri());

        if(SD.exists(url)){
            File fd = SD.open(url);
            if(fd){
                char *ext = strchr(url, '.') + 1;

                response.println("HTTP/1.1 200 OK");

                // Check mime from extension
                response.print("Content-Type: ");
                if(strcmp_P(ext, PSTR("js")) == 0) response.println("text/javascript");
                else if(strcmp_P(ext, PSTR("css")) == 0) response.println("text/css");
                else if(strcmp_P(ext, PSTR("htm")) == 0) response.println("text/html");
                else if(strcmp_P(ext, PSTR("xml")) == 0) response.println("text/xml");
                else if(strcmp_P(ext, PSTR("jsn")) == 0) response.println("text/json");
                else response.println("text/plain");
                response.println("Connection: close");
                response.println();
            
                // Write file content
                while(fd.available()) Serial.write(fd.read());
                fd.close();
            }
        }else{
            response.println("HTTP/1.1 404 Not Found");
            response.println("Content-Type: text/html");
            response.println("Connection: close");
            response.println();
            response.println(F("<html lang=\"en\">"));
            response.println(F("<head>"));
                response.println(F("<link href='//fonts.googleapis.com/css?family=Lato:100' rel='stylesheet' type='text/css'>"));
                response.println(F("<style>"));                
                    response.println(F("body{margin: 0; padding: 0; width: 100%; height: 100%; color: #00878F; display: table; font-weight: 100; font-family: 'Lato';}"));
                    response.println(F(".container{text-align: center; display: table-cell; vertical-align: middle;}"));
                    response.println(F(".content {text-align: center; display: inline-block;}"));
                    response.println(F(".title{font-size: 96px; margin-bottom: 40px;}"));
                    response.println(F(".quote{font-size: 24px; font-weight: bold;}"));
                response.println(F("</style>")) ;
                response.println(F("<title>uHTTP</title>"));
            response.println(F("</head>"));
            response.println(F("<body>"));
                response.println(F("<div class=\"container\">"));
                    response.println(F("<div class=\"content\">"));
                        response.println(F("<div class=\"title\">404 Not Found</div>"));
                        response.println(F("<div class=\"quote\">Ooops! The requested page was not found!</div>"));
                    response.println(F("</div>"));
                response.println(F("</div>"));
            response.println(F("</body>"));
            response.println(F("</html>"));
        }
        
        response.stop();
    }
}