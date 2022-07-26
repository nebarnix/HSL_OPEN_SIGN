#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

const char* ssid     = "heatsynclabs";
const char* password = "hacktheplanet";

const char* host = "members.heatsynclabs.org";
    
// Init relay pins
const int relayPin = 3;
// name the output pin for 'Relay ' 
//int relayVar = 0; 

void setup() {
  Serial.begin(115200);
  delay(10);
  
  pinMode(relayPin, OUTPUT); // set as output
  digitalWrite(relayPin, 0); // set LOW

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  delay(30000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  //String url = "/";
  String url = "/space_api/simple.json";
  //String url = "/space_api.json";
  
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    //line to find {"unlocked":false,"door_1_locked":true,"door_2_locked":true}
    
    Serial.print(line);
    
    if(line.startsWith("{\"unlocked\":true",1))
       {        
       digitalWrite(relayPin, 1); // Turn sign on
       Serial.print("\nDoors are OPEN\n");
       }
       
    else if(line.startsWith("{\"unlocked\":false",1))
       {
       digitalWrite(relayPin, 0); // Turn sign off
       Serial.print("\nDoors are CLOSED\n");
       }
       
  }
  
  Serial.println();
  Serial.println("closing connection");
}

