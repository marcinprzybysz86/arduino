/*********
https://www.baldengineer.com/mqtt-tutorial.html 
*********/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>


#define domoticz_in "domoticz/in"
#define LED_BUILTIN 2



// Connect to the WiFi
const char* ssid = "xxxxxxxxx";   //ssid wifi
const char* password = "xxxxxxxxx";   // haslo wifi
const char* mqtt_server = "192.168.1.X";  //mqtt server address
 
WiFiClient espClient;
PubSubClient client(espClient);

IPAddress ip(192, 168, 1, X); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, X); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network

unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long StartPodlewania = 0;
unsigned long CzasPodlewania = 60000*40;

 

// constants won't change :
const long interval = 3000; 

const byte ledPin = 0; // Pin with LED on Adafruit Huzzah


void callback(char* topic, byte* payload, unsigned int length) {
 StaticJsonBuffer<300> jsonBuffer;
 char receivedChar[length];
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
 receivedChar[i] = (char)payload[i];
  //Serial.print(receivedChar);
 }
 JsonObject& root = jsonBuffer.parseObject(receivedChar);
  int idx = root["idx"];
  int nvalue = root["nvalue"];
  String svalue = root["svalue1"];
 
  obsluga(idx,nvalue,svalue);
}

void obsluga(int idx, int nvalue, String svalue)
{
 Serial.println("IDX: " + String(idx) + " nval.: ");
 //Serial.print(nvalue + " svalue: ");
// Serial.println(svalue);

  if (idx == 48) {                // IDX dummy switch with level choice
    Serial.println("IDX 104 - sprawdzam nvalue: " + String(nvalue) + " / " + svalue);
    if (svalue=="0") { stop(); }
    if (svalue=="10") { zalacz(D2);}
    if (svalue=="20") { zalacz(D3);}
    if (svalue=="30") { zalacz(D0);}
    if (svalue=="40") { zalacz(D5);}
    if (svalue=="50") { zalacz(D6);}
    if (svalue=="60") { all();}
    
    
  }
 
}

void stop(){
digitalWrite(D0,HIGH);  delay(200); // relay 1
 digitalWrite(D2,HIGH);  delay(200);
 digitalWrite(D3,HIGH);  delay(200);
 digitalWrite(D5,HIGH);  delay(200);
 digitalWrite(D6,HIGH);  delay(200);  //relay 5
 digitalWrite(D7,HIGH);     // transformator
  
}

void zalacz(int numer){
  Serial.print("Zalaczam: ");
  Serial.println(numer);
digitalWrite(D0,HIGH);  delay(1000); // relay 1
 digitalWrite(D2,HIGH);  delay(1000);
 digitalWrite(D3,HIGH);  delay(1000);
 digitalWrite(D5,HIGH);  delay(1000);
 digitalWrite(D6,HIGH);  delay(1000);  //relay 5
 digitalWrite(D7,LOW);     // transformator

 digitalWrite(numer,LOW);
  
}

void all(){
digitalWrite(D0,LOW);  delay(200); // relay 1
 digitalWrite(D2,LOW);  delay(200);
 digitalWrite(D3,LOW);  delay(200);
 digitalWrite(D5,LOW);  delay(200);
 digitalWrite(D6,LOW);  delay(200);  //relay 5
 digitalWrite(D7,LOW);     // transformator
  
}
 
 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP8266_podlewanie")) {
  Serial.println("connected");
  // ... and subscribe to topic
  client.subscribe("domoticz/out");
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}

// displays at startup the Sketch running in the Arduino
void display_Running_Sketch (void){
  String the_path = __FILE__;
  int slash_loc = the_path.lastIndexOf('/');
  String the_cpp_name = the_path.substring(slash_loc+1);
  int dot_loc = the_cpp_name.lastIndexOf('.');
  String the_sketchname = the_cpp_name.substring(0, dot_loc);

  Serial.print("\nArduino is running Sketch: ");
  Serial.println(the_sketchname);
  Serial.print("Compiled on: ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.print(__TIME__);
  Serial.print("\n");
}
void podlaczWifi(){

    WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet); 
  Serial.println("Uruchomiono. / podlaczam sie do wifi /");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
Serial.println("WIFI OK!");

  
  Serial.println("");
  Serial.print("Podlaczono do: ");
  Serial.println(ssid);
  Serial.print("Adres IP: ");
  Serial.println(WiFi.localIP());
}
void setup()
{
 Serial.begin(9600);
display_Running_Sketch();
podlaczWifi();
 
 // pin setup
 pinMode(LED_BUILTIN,OUTPUT);  //////////////////////////////////////////////////////// pin mode
   // preparing GPIOs
  pinMode(D0, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);


   delay(100);digitalWrite(D0, HIGH);
  delay(100);digitalWrite(D2, HIGH);
  delay(100);digitalWrite(D3, HIGH);
  delay(100);digitalWrite(D5, HIGH);
  delay(100);digitalWrite(D6, HIGH);
  delay(100);digitalWrite(D7, HIGH);

  
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);

 ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN,HIGH);

 Serial.println("koniec setupu.");

}

void publikuj(int idx){
  Serial.println("Publikuje toggle przelacznika");

 String message;
 //{"command": "switchlight", "idx": 2450, "switchcmd": "On" }
 message+="{\"command\": \"switchlight\", \"idx\": ";
 message+=String(idx);
 message+=",  \"switchcmd\" : \"On\"}";
if ( client.publish(domoticz_in, (char*) message.c_str())) {

  Serial.println("Opublikowano: " + message);
}
  
}



 
void loop()
{
 

  ArduinoOTA.handle();
 if (!client.connected()) {
  reconnect();
 }
 client.loop();

 unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
   //publikuj(0,0);
   digitalWrite(LED_BUILTIN,LOW);
   delay(100);
   digitalWrite(LED_BUILTIN,HIGH);
  }

delay(2);
}