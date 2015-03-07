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

#include <SPI.h>
#include <Ethernet.h>
#include <uHTTP.h>

byte macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x66};
byte ip4addr[4] = {192, 168, 0, 254};

uHTTP *server = new uHTTP(80);
EthernetClient response;

void setup(){
	Serial.begin(115200);

    Ethernet.begin(macaddr, ip4addr);

    Serial.print(F("Starting uHTTP at "));
    Serial.print(Ethernet.localIP());
    Serial.println(":80");

    server->begin();
}

void loop(){
	if((response = server->available())){
        header_t head = server->head();        

        // Or check if url is equals to the passed value
        if(server->uri(F("foo/bar"))){
            // Get complete uri:
            Serial.println(server->uri());
        }

        // Or check if only a segment of url is equals to the passed value
        if(server->uri(1, F("foo"))){
        	// Get only first segment from uri:
            Serial.print(F("Segment[1]: "));
            Serial.println(server->uri(1));
        }

        // Or you can get query string:
        Serial.print(F("QUERY: "));
        Serial.println(server->query());

        // Or you can get variable from GET data:
        Serial.print(F("QUERY[foo]: "));
        Serial.println(server->query(F("foo")));
        Serial.print(F("QUERY[bar]: "));
        Serial.println(server->query(F("bar")));
        Serial.print(F("QUERY[poo]: "));
        Serial.println(server->query(F("poo")));

        // Or you can get post data:
        if(server->method(uHTTP_METHOD_POST)){
          Serial.print(F("DATA[foo]: "));
          Serial.println(server->data(F("foo")));
          Serial.print(F("DATA[bar]: "));
          Serial.println(server->data(F("bar")));
          Serial.print(F("DATA[poo]: "));
          Serial.println(server->data(F("poo")));
        }
        
        // Or you can get the Content-Type:
        Serial.print(F("Content-Type: "));
        Serial.println(head.type);

        // Or you can get the Content-Length:
        Serial.print(F("Content-Length: "));
        Serial.println(head.length);

        // Or you can get Authorization token:
        Serial.print(F("Authorization: "));
        Serial.println(head.auth);

        // Or you can get Origin for CORS:
        Serial.print(F("Origin: "));
        Serial.println(head.orig);

        // Or you can get body:
        Serial.print(F("BODY: "));
        Serial.println(server->body());

        response.println("HTTP/1.1 200 OK");
        response.println("Content-Type: text/html");
        response.println("Connection: close");
        response.println();
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
                    response.println(F("<div class=\"title\">"));
                        response.println(F("<a href=\"http://nomadnt.github.io/uHTTP\">"));
                            response.println(F("<img src=\"https://cloud.githubusercontent.com/assets/8282385/6541264/1b556760-c4c7-11e4-87f7-8268cde78c2e.png\">"));
                        response.println(F("</a>"));
                    response.println(F("</div>"));
                    response.println(F("<div class=\"quote\">Simplicity is the ultimate sophistication. - Leonardo da Vinci</div>"));
                response.println(F("</div>"));
            response.println(F("</div>"));
        response.println(F("</body>"));
        response.println(F("</html>"));
        response.stop();
    }
}