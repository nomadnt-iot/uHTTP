uHTTP
=====

uHTTP is a tiny library for Arduino that provide a very manner way to access to HTTP request data

This library is written by Filippo Sallemi <tonyputi@gmail.com>

Installation
--

To install the libraries, you need to place them into your "libraries" folder. You can find it within your Arduino IDE distribution within the "hardware" folder.

```
  # cd <path/to/your/arduino/distribution>/libraries
  # git clone https://github.com/nomadnt/uHTTP.git
```
Or you download the zipped version of the library from https://github.com/nomadnt/uREST/archive/master.zip, create a directory <path/to/your/arduino/distribution>/libraries/uHTTP and put the contents of the zip-file there.

Additional information can be found on the Arduino website: http://www.arduino.cc/en/Hacking/Libraries

Be sure to restart the IDE if it was running.

Configuration
--

#### uREST ####
uREST provide an elegant way to parse and process a REST call from any browser and any device

METHOD | URI            | DESCRIPTION
------ | -------------- | --------------------------------------------
GET    | /digital/      | Return all digital pin configuration/values
GET    | /digital/{PIN} | Return specified digital pin configuration/value
POST   | /digital/{PIN} | Set configuration/value for specified digital pin
GET    | /analog/       | Return all analog pin configuration/values
GET    | /analog/{PIN}  | Return specified analog pin configuration/values
POST   | /analog/{PIN}  | Set configuration/value for specified analog pin
GET    | /config/       | Return internal configuration
POST   | /config/       | Set internal configuration values


You can get pin status via curl

`curl http://192.168.0.20/digital/13`

You can set pin mode and status via curl:

`curl -X POST -d "mode=OUTPUT,value=HIGH" http://192.168.0.20/digital/13`
