#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


MDNSResponder mdns;

// Replace with your network credentials
const char* ssid = "arduino";
const char* password = "yourpassword";


ESP8266WebServer server(84);
  WiFiClient client;

String webPage = "";

int gpio0_pin = 0;
IPAddress ip(192, 168, 1, 205); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network

unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
const long interval = 1800000;           // interval at which to blink (milliseconds)


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



void setup(void){
  
  webPage += "<pre>Sketch name<br/> ";
  webPage += __FILE__;
  webPage += "<br/> compiled on ";
  webPage += __DATE__;
  webPage += __TIME__;
  webPage += "</pre>";
  webPage += "<h1>ESP8266 NAWADNIANIE</h1><p>D#1 <a href=\"socket1On\"><button>ON</button></a></p>";
  webPage += "<p>D#2 <a href=\"socket2On\"><button>ON</button></a></p>";
  webPage += "<p>D#3 <a href=\"socket3On\"><button>ON</button></a></p>";
  webPage += "<p>D#5 <a href=\"socket4On\"><button>ON</button></a></p>";
  webPage += "<p>D#6 <a href=\"socket5On\"><button>ON</button></a></p>";
  webPage += "<p>Wyslij LOG <a href=\"sendlog\"><button>WYSLIJ</button></a>&nbsp;<a href=\"stop\"><button>STOP</button></a></p>";
  
  // preparing GPIOs
  pinMode(D4, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);


   delay(100);digitalWrite(D4, HIGH);
  delay(100);digitalWrite(D2, HIGH);
  delay(100);digitalWrite(D3, HIGH);
  delay(100);digitalWrite(D5, HIGH);
  delay(100);digitalWrite(D6, HIGH);
  delay(100);digitalWrite(D7, HIGH);

 
  
  delay(500);
  Serial.begin(115200);
  display_Running_Sketch();
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
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/socket1On", [](){
    server.send(200, "text/html", webPage);
 digitalWrite(D4,LOW);   // relay 1
 digitalWrite(D2,HIGH);
 digitalWrite(D3,HIGH);
 digitalWrite(D5,HIGH);
 digitalWrite(D6,HIGH);    //relay 5
 digitalWrite(D7,LOW);     // transformator
    delay(300);
     Log2("Sekcja1ON");
  });

    server.on("/socket2On", [](){
    server.send(200, "text/html", webPage);
 digitalWrite(D4,HIGH);   // relay 1
 digitalWrite(D2,LOW);
 digitalWrite(D3,HIGH);
 digitalWrite(D5,HIGH);
 digitalWrite(D6,HIGH);    //relay 5
 digitalWrite(D7,LOW);     // transformator
    delay(300);
    Log2("Sekcja2ON");
  });

    server.on("/socket3On", [](){
    server.send(200, "text/html", webPage);
 digitalWrite(D4,HIGH);   // relay 1
 digitalWrite(D2,HIGH);
 digitalWrite(D3,LOW);
 digitalWrite(D5,HIGH);
 digitalWrite(D6,HIGH);    //relay 5
 digitalWrite(D7,LOW);     // transformator
    delay(300);
    Log2("Sekcja3ON");
  });

      server.on("/socket4On", [](){
    server.send(200, "text/html", webPage);
 digitalWrite(D4,HIGH);   // relay 1
 digitalWrite(D2,HIGH);
 digitalWrite(D3,HIGH);
 digitalWrite(D5,LOW);
 digitalWrite(D6,HIGH);    //relay 5
 digitalWrite(D7,LOW);     // transformator
    delay(300);
    Log2("Sekcja4ON");
  });

      server.on("/socket5On", [](){
    server.send(200, "text/html", webPage);
 digitalWrite(D4,HIGH);   // relay 1
 digitalWrite(D2,HIGH);
 digitalWrite(D3,HIGH);
 digitalWrite(D5,HIGH);
 digitalWrite(D6,LOW);    //relay 5
 digitalWrite(D7,LOW);     // transformator
    delay(300);
    Log2("Sekcja5ON");
  });

  
    server.on("/stop", [](){
    server.send(200, "text/html", webPage);
    Log2("stopping");
 digitalWrite(D4,HIGH);  delay(1000); // relay 1
 digitalWrite(D2,HIGH);  delay(1000);
 digitalWrite(D3,HIGH);  delay(1000);
 digitalWrite(D5,HIGH);  delay(1000);
 digitalWrite(D6,HIGH);  delay(1000);  //relay 5
 digitalWrite(D7,HIGH);     // transformator
    delay(300);
    Log2("ALL_OFF");
  });

  server.on("/all", [](){
    server.send(200, "text/html", webPage);
    Log2("stopping");
    digitalWrite(D7,LOW);
 digitalWrite(D4,LOW);  delay(1000); // relay 1
 digitalWrite(D2,LOW);  delay(1000);
 digitalWrite(D3,LOW);  delay(1000);
 digitalWrite(D5,LOW);  delay(1000);
 digitalWrite(D6,LOW);  delay(1000);  //relay 5
      // transformator
    delay(300);
    Log2("ALL_ON");
  });

  server.on("/sendlog", [](){
    server.send(200, "text/html", webPage);
    Serial.println("wysylam log...");
Log2("testowy");
    delay(300);
    Serial.println("Wyslano.");
  });




  
  server.begin();
  Serial.println("HTTP server started");
  Log2("started");
}
 
void loop(void){
  server.handleClient();
unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    Log2("pingOK");
  }
  
} 

void Log2(String text) {
 HTTPClient http;  //Declare an object of class HTTPClient
    String url;
    url+="http://192.168.1.200:8080/json.htm?type=command&param=addlogmessage&message=esp_";
    url+=text;
    Serial.println(url);
    http.begin(url);  //Specify request destination
    int httpCode = http.GET(); 
    Serial.println(httpCode);//Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
 
    }
 
    http.end();   //Close connection
}



