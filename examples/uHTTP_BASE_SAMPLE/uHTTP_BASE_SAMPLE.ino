/**
 * HTTP Base example
 * Serve index.html page and provide a single session authentication for /admin/
 * 
 * / or /index.html will show home page with no authentication required
 * /admin/ will show sample admin area
 * /logout/ will logoff user
 *
 * This example code is in the public domain.
 **/

#include <SPI.h>
#include <Ethernet.h>
#include <uHTTP.h>

#define SESSION_TIMEOUT     3600            // Session timeout in seconds 

// Base64 encoded token. username: admin, password: admin
const PROGMEM char TOKEN[] = "YWRtaW46YWRtaW4=";
// Index page compressed that will be send in a single packet
const PROGMEM char INDEX[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html lang=\"en\">\r\n<head>\r\n<meta http-equiv=\"Cache-control\" content=\"public\">\r\n<link href='//fonts.googleapis.com/css?family=Lato:100' rel='stylesheet' type='text/css'>\r\n<style>\r\nbody{margin: 0; padding: 0; width: 100%; height: 100%; color: #00878F; display: table; font-weight: 100; font-family: 'Lato';}\r\n.container{text-align: center; display: table-cell; vertical-align: middle;}\r\n.content {text-align: center; display: inline-block;}\r\n.title{font-size: 96px; margin-bottom: 40px;}\r\n.quote{font-size: 24px; font-weight: bold;}\r\n</style>\r\n<title>uHTTP</title>\r\n</head>\r\n<body>\r\n<div class=\"container\">\r\n<div class=\"content\">\r\n<div class=\"title\">\r\n<a href=\"http://nomadnt.github.io/uHTTP\">\r\n<img src=\"https://cloud.githubusercontent.com/assets/8282385/6541264/1b556760-c4c7-11e4-87f7-8268cde78c2e.png\">\r\n</a>\r\n</div>\r\n<div class=\"quote\">Simplicity is the ultimate sophistication. - Leonardo da Vinci</div>\r\n</div>\r\n</div>\r\n</body>\r\n</html>";
// Admin page compressed that will be send in a single packet
const PROGMEM char ADMIN[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html lang=\"en\">\r\n<head>\r\n<meta http-equiv=\"Cache-control\" content=\"public\">\r\n<link href='//fonts.googleapis.com/css?family=Lato:100' rel='stylesheet' type='text/css'>\r\n<style>\r\nbody{margin: 0; padding: 0; width: 100%; height: 100%; color: #00878F; display: table; font-weight: 100; font-family: 'Lato';}\r\na:link{text-decoration: none;}\r\na:visited{color: #00878F;}\r\na:hover{text-decoration: underline}\r\n.container{text-align: center; display: table-cell; vertical-align: middle;}\r\n.content {text-align: center; display: inline-block;}\r\n.title{font-size: 96px; margin-bottom: 40px;}\r\n.quote{font-size: 24px; font-weight: bold;}\r\n</style>\r\n<title>uHTTP</title>\r\n</head>\r\n<body>\r\n<div class=\"container\">\r\n<div class=\"content\">\r\n<div class=\"title\">Admin area\r\n</div>\r\n<div class=\"quote\">Click <a href=\"/logout\">here</a> to logout</div>\r\n</div>\r\n</div>\r\n</body>\r\n</html>";
// Login page compressed that will be send in a single packet
const PROGMEM char LOGIN[] = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/html\r\nWWW-Authenticate: Basic realm=\"uHTTP\"\r\n\r\n<html lang=\"en\">\r\n<head>\r\n<meta http-equiv=\"Cache-control\" content=\"public\">\r\n<link href='//fonts.googleapis.com/css?family=Lato:100' rel='stylesheet' type='text/css'>\r\n<style>\r\nbody{margin: 0; padding: 0; width: 100%; height: 100%; color: #00878F; display: table; font-weight: 100; font-family: 'Lato';}\r\n.container{text-align: center; display: table-cell; vertical-align: middle;}\r\n.content {text-align: center; display: inline-block;}\r\n.title{font-size: 96px; margin-bottom: 40px;}\r\n.quote{font-size: 24px; font-weight: bold;}\r\n</style>\r\n<title>uHTTP</title>\r\n</head>\r\n<body>\r\n<div class=\"container\">\r\n<div class=\"content\">\r\n<div class=\"title\">401 Unauthorized</div>\r\n<div class=\"quote\">Access denied due to invalid credentials</div>\r\n</div>\r\n</div>\r\n</body>\r\n</html>";
// Logout page compressed that will be send in a single packet
const PROGMEM char LOGOUT[] = "HTTP/1.1 302 Found\r\nLocation: http://logout:@192.168.0.254/admin\r\n\r\n";
// Error 404 page compressed that will be send in a single packet
const PROGMEM char ERROR404[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html lang=\"en\">\r\n<head>\r\n<meta http-equiv=\"Cache-control\" content=\"public\">\r\n<link href='//fonts.googleapis.com/css?family=Lato:100' rel='stylesheet' type='text/css'>\r\n<style>\r\nbody{margin: 0; padding: 0; width: 100%; height: 100%; color: #00878F; display: table; font-weight: 100; font-family: 'Lato';}\r\n.container{text-align: center; display: table-cell; vertical-align: middle;}\r\n.content {text-align: center; display: inline-block;}\r\n.title{font-size: 96px; margin-bottom: 40px;}\r\n.quote{font-size: 24px; font-weight: bold;}\r\n</style>\r\n<title>uHTTP</title>\r\n</head>\r\n<body>\r\n<div class=\"container\">\r\n<div class=\"content\">\r\n<div class=\"title\">404 Page Not Found</div>\r\n<div class=\"quote\">Ooops! The requested page was not found!</div>\r\n</div>\r\n</div>\r\n</body>\r\n</html>";
// Error 500 page compressed that will be send in a single packet
const PROGMEM char ERROR500[] = "HTTP/1.1 500 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html lang=\"en\">\r\n<head>\r\n<meta http-equiv=\"Cache-control\" content=\"public\">\r\n<link href='//fonts.googleapis.com/css?family=Lato:100' rel='stylesheet' type='text/css'>\r\n<style>\r\nbody{margin: 0; padding: 0; width: 100%; height: 100%; color: #00878F; display: table; font-weight: 100; font-family: 'Lato';}\r\n.container{text-align: center; display: table-cell; vertical-align: middle;}\r\n.content {text-align: center; display: inline-block;}\r\n.title{font-size: 96px; margin-bottom: 40px;}\r\n.quote{font-size: 24px; font-weight: bold;}\r\n</style>\r\n<title>uHTTP</title>\r\n</head>\r\n<body>\r\n<div class=\"container\">\r\n<div class=\"content\">\r\n<div class=\"title\">500 Internal Server Error</div>\r\n<div class=\"quote\">Ooops! Something goes wrong!</div>\r\n</div>\r\n</div>\r\n</body>\r\n</html>";

byte macaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x66};
byte ip4addr[4] = {192, 168, 0, 254};

uHTTP *server = new uHTTP(80);
EthernetClient response;

unsigned long session_time = 0;

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

        if(server->uri(F("/")) || server->uri(F("/index.html"))){
            response.println((const __FlashStringHelper *)INDEX);
        }else if(server->uri(1, F("admin"))){
            if(session_time > 0 && (millis() - session_time) > (SESSION_TIMEOUT * 1000L)){
                session_time = 0;
                response.println((const __FlashStringHelper *) LOGOUT);
            }else{
                // Check authentication
                if(strcmp_P(head.auth, TOKEN) == 0){
                    session_time = millis();
                }else{
                    session_time = 0;
                }

                // Check session time
                if(!session_time){
                    response.println((const __FlashStringHelper *) LOGIN);
                }else{
                    response.println((const __FlashStringHelper *) ADMIN);
                }
            }
        }else if(server->uri(1, F("logout"))){
            response.println((const __FlashStringHelper *) LOGOUT);
        }else{
            response.println((const __FlashStringHelper *) ERROR404);
        }

        response.stop();
    }
}

/**
 *  This is another way to send response but in this way each response.print/ln
 *  is sended as a single tcp packet so you will send a number of packets equals
 *  to the number of response.print/ln calls.
 **/
void actionIndex(){
    response.println(F("HTTP/1.1 200 OK"));
    response.println(F("Content-Type: text/html"));
    response.println(F("Connection: close"));
    response.println(F(""));
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
}