//********* MARCIN PRZYBYSZ www.vsx.pl **************

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <Bounce2.h>

#define domoticz_in "domoticz/in"
#define LED_BUILTIN 2
#define BUTTON_PIN D2


// Connect to the WiFi
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";
 
WiFiClient espClient;
PubSubClient client(espClient);

IPAddress ip(192, 168, X, X); // where xx is the desired IP Address
IPAddress gateway(192, 168, X, X); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network

unsigned long previousMillis = 0;        // will store last time LED was updated



// constants won't change :
const long interval = 8000; 

const byte ledPin = 0; // Pin with LED on Adafruit Huzzah
Bounce debouncer = Bounce();

void callback(char* topic, byte* payload, unsigned int length) {
 StaticJsonBuffer<200> jsonBuffer;
 char receivedChar[length];
 //Serial.print("Message arrived [");
 //Serial.print(topic);
 //Serial.print("] ");
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

  if (idx == 85) {
    Serial.println("IDX 85 - sprawdzam nvalue: " + String(nvalue) + " / " + svalue);
    if (nvalue==1) { digitalWrite(D7,LOW); Serial.println("zalaczam d7"); }
    if (nvalue==0) { digitalWrite(D7,HIGH); Serial.println("wylaczam d7"); }
    
  }
}
 
 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP8266 Client")) {
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
 pinMode(D3, INPUT_PULLUP);
 pinMode(BUTTON_PIN, INPUT_PULLUP);
 pinMode(D7, OUTPUT);

  
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
  digitalWrite(D7,LOW);

  debouncer.attach(BUTTON_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  debouncer.interval(25); // Use a debounce interval of 25 milliseconds
}

void publikuj(int idx){
  Serial.println("Publikuje toggle przelacznika");

 String message;
 //{"command": "switchlight", "idx": 2450, "switchcmd": "On" }
 message+="{\"command\": \"switchlight\", \"idx\": ";
 message+=String(idx);
 message+=",  \"switchcmd\" : \"Toggle\"}";
if ( client.publish(domoticz_in, (char*) message.c_str())) {

  Serial.println("Opublikowano: " + message);
}
  
}



 
void loop()
{
 debouncer.update();

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
   delay(200);
   digitalWrite(LED_BUILTIN,HIGH);
  }
 if ( debouncer.fell() ) publikuj(85);
delay(2);
}
