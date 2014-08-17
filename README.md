uREST
=====

uREST is a library that implements REST like interface for ARDUINO.

This library is written by Filippo Sallemi <tonyputi@gmail.com>

Installation
--

To install the libraries, you need to place them into your "libraries" folder. You can find it within your Arduino IDE distribution within the "hardware" folder.

```
  # cd <path/to/your/arduino/distribution>/libraries
  # git clone https://github.com/nomadnt/uREST.git
```
Or you download the zipped version of the library from https://github.com/nomadnt/uREST/archive/master.zip, create a directory <path/to/your/arduino/distribution>/libraries/uREST and put the contents of the zip-file there.

Additional information can be found on the Arduino website: http://www.arduino.cc/en/Hacking/Libraries

Be sure to restart the IDE if it was running.

Configuration
--

uREST belongs to another library "uHTTP".

#### uHTTP ####
uHTTP provide methods to navigate HTTP request to Arduino Ethernet.

#### uREST ####
uREST provide an elegant way to parse and process a REST call from any browser and any device

METHOD        | URI           | DESCRIPTION
------------- | ------------- | -------------
GET           | /{PIN}/HIGH   | Call digitalWrite(pin, HIGH);
GET           | /{PIN}/LOW    | Call digitalWrite(pin, LOW);
GET           | /{PIN}/{VALUE}| Call digitalWrite(pin, VALUE);
GET           | /{PIN}/INPUT  | Call pinMode(pin, INPUT);
GET           | /{PIN}/OUTPUT | Call pinMode(pin, OUTPUT);
