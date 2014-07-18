

/*
  Pachube sensor client
 
 This sketch connects an analog sensor to Pachube (http://www.pachube.com)
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.
 
 This example has been updated to use version 2.0 of the Pachube.com API. 
 To make it work, create a feed with a datastream, and give it the ID
 sensor1. Or change the code below to match your feed.
 
 
 Circuit:
 * Analog sensor attached to analog in 0
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 15 March 2010
 modified 9 Apr 2012
 by Tom Igoe with input from Usman Haque and Joe Saavedra
 
http://arduino.cc/en/Tutorial/PachubeClient
 This code is in the public domain.
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <b64.h>
#include <HttpClient.h>
#include <Xively.h>
#include <Wire.h>


#define sensorPin A2
#define ledPin 9

char xivelyKey[] = "<MY_API_KEY>"; // replace your pachube api key here
#define xivelyFeed 123456789 // replace your feed ID

char tempID[] = "Temperature";
char humidID[] = "Humidity";

#define tmp102Address 0x48

XivelyDatastream datastreams[] = {
  XivelyDatastream(tempID, strlen(tempID), DATASTREAM_FLOAT),
  XivelyDatastream(humidID, strlen(humidID), DATASTREAM_FLOAT),
};

XivelyFeed feed(xivelyFeed, datastreams, 2);

// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(10,0,1,177);
// initialize the library instance:
EthernetClient client;

XivelyClient xivelyclient(client);

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(216,52,233,122);      // numeric IP for api.pachube.com
//char server[] = "api.pachube.com";   // name address for pachube API

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000; //delay between updates to Pachube.com

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Wire.begin();
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);

 // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // DHCP failed, so use a fixed IP address:
    Ethernet.begin(mac, ip);
  }
}

void loop() {
  
  /* Not reading the stream at the moment
  int getReturn = xivelyclient.get(feed, xivelyKey);
  if(getReturn > 0) {
    Serial.println("LED Datastream");
    Serial.println(feed[1]);
  } else {
    Serial.println("HTTP Error");
  }
  
  int level = feed[1].getFloat();
  if (level < 0) {
    level = 0;
  } else if(level > 255){
    level = 255;
  }
  
  digitalWrite(ledPin, level);
  */
  
  // read the analog sensor:
  float temp = (1.8 * getTemperature()) + 32;
  //int sensorValue = analogRead(sensorPin);
  datastreams[0].setFloat(temp);
  Serial.print("Read temp value ");
  Serial.println(datastreams[0].getFloat());
 
  Serial.println("Uplaoding it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  Serial.println(""); 

  delay(15000);
}

float getTemperature(){
  Wire.requestFrom(tmp102Address,2);
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  // 12bit int... using two's complement
  int TemperatureSum = ((MSB << 8) | LSB) >> 4;
  
  float celsius = TemperatureSum * 0.0625;
  return celsius;
}
