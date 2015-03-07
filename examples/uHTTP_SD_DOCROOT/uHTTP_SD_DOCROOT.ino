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

#define TEXT_PLAIN  0
#define TEXT_HTML   1
#define TEXT_JS     2
#define TEXT_CSS    3
#define TEXT_XML    4
#define TEXT_JSON   5

#define SDC_PIN 4

byte macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x66};
byte ip4addr[4] = {192, 168, 0, 254};

uHTTP *server = new uHTTP(80);
EthernetClient response;

void send_headers(uint16_t code = 200, uint8_t ctype = TEXT_PLAIN);
void render(uint16_t code = 200, uint8_t ctype = TEXT_PLAIN);

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
        char url[32];

        if(server->uri(F("/"))) strcpy_P(url, PSTR("/index.htm"));
        else strcpy(url, server->uri());

        if(SD.exists(url)){
            File fd = SD.open(url, FILE_READ);

            if(fd){
                char *ext = strchr(url, '.') + 1;
                uint8_t ctype = TEXT_PLAIN;

                Serial.println(url);

                if(strcmp_P(ext, PSTR("js")) == 0) ctype = TEXT_JS;
                else if(strcmp_P(ext, PSTR("css")) == 0) ctype = TEXT_CSS;
                else if(strcmp_P(ext, PSTR("htm")) == 0) ctype = TEXT_HTML;
                else if(strcmp_P(ext, PSTR("xml")) == 0) ctype = TEXT_XML;
                else if(strcmp_P(ext, PSTR("jsn")) == 0) ctype = TEXT_JSON;
                
                send_headers(200, ctype);
            
                // Write file content
                while(fd.available()) response.write(fd.read());
                fd.close();
            }else{
                render(500, TEXT_HTML);
            }
        }else{
            render(404, TEXT_HTML);
        }
        
        response.stop();
    }
}

void send_headers(uint16_t code, uint8_t ctype){
    response.print("HTTP/1.1 ");

    switch(code){
        case 200:
            response.println("200 OK");
            break;
        case 404:
            response.println("404 Not Found");
            break;
        case 500:
            response.println("500 Internal Server Error");
            break;
    }

    response.print("Content-Type: ");
    switch(ctype){
        case TEXT_JS:
            response.println("text/javascript");
            break;
        case TEXT_CSS:
            response.println("text/css");
            break;
        case TEXT_HTML:
            response.println("text/html");
            break;
        case TEXT_XML:
            response.println("text/xml");
            break;
        case TEXT_JSON:
            response.println("text/json");
            break;
        default:
            response.println("text/plain");
            break;
    }
    response.println("Connection: close");
    response.println();
}

void render(uint16_t code, uint8_t ctype){
    send_headers(code, ctype);
    response.println(F("<html lang=\"en\">"));
    response.println(F("<head>"));
        response.println(F("<meta http-equiv=\"Cache-control\" content=\"public\">"));
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
                switch(code){
                    case 404:
                        response.println(F("<div class=\"title\">404 Not Found</div>"));
                        response.println(F("<div class=\"quote\">Ooops! The requested page was not found!</div>"));
                        break;
                    case 500:
                        response.println(F("<div class=\"title\">500 Internal Server Error</div>"));
                        response.println(F("<div class=\"quote\">Ooops! Something goes wrong!</div>"));
                        break;
                }
            response.println(F("</div>"));
        response.println(F("</div>"));
    response.println(F("</body>"));
    response.println(F("</html>"));
}