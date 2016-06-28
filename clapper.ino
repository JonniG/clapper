#include <PubSubClient.h>
#include <ESP8266WiFi.h>

int clapVal; //analog sound sensor value

//Interval between first clap and stop counting claps
long curTime;
long prevTime;
int interval = 1000;

int count = 0; //clap counter
int clapSens = 4; //Sound sensitivity. higher number is less sensitive

//Define pins
int clapLED1 = 12;
int clapLED2 = 13;
int clapLED3 = 14;
int micPin = A0;

// Wifi + Server Configuration
const char* ssid = "*****";
const char* password = "*****";
const char* mqtt_server = "*****";

//MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
  
}

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  //Connect to Wifi
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

//If connection lost
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Clapper")) {
      Serial.println("connected");
      // Once connected, publish an announcement
      client.publish("clapper/clap", "0", true);      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
  
void setup() {
   pinMode(micPin, INPUT);
   pinMode(clapLED1, OUTPUT);
   pinMode(clapLED2, OUTPUT);
   pinMode(clapLED3, OUTPUT);    

  //start the serial line for debugging
  Serial.begin(115200);
  delay(100);
  
  Serial.println("");
  Serial.println("Clapper Initiating");

  //Connect to wifi and MQTT server
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void clap() { 
  curTime = millis();
  if (curTime - prevTime >= interval) {
    if (count == 1) {
      client.publish("clapper/clap", "1", true);
      Serial.println("published 1 clap");
      delay(2000);
      digitalWrite(clapLED1, LOW);
      client.publish("clapper/clap", "0", true);
    } else if (count == 2) {
      client.publish("clapper/clap", "2", true);
      Serial.println("published 2 claps");
      delay(2000);
      digitalWrite(clapLED1, LOW);
      digitalWrite(clapLED2, LOW);
      client.publish("clapper/clap", "0", true);
    } else if (count == 3) {
      client.publish("clapper/clap", "3", true);
      Serial.println("published 3 claps");
      delay(2000);
      digitalWrite(clapLED1, LOW);
      digitalWrite(clapLED2, LOW);
      digitalWrite(clapLED3, LOW);
      client.publish("clapper/clap", "0", true);
    }
    count = 0;
  }
  if (count == 0) {
    clapVal = analogRead(micPin);
    if (clapVal > clapSens)
    {
      prevTime = curTime;
      count = 1;
      Serial.println("clap #1");
      digitalWrite(clapLED1, HIGH);
      delay(300);
      }
  } else if (count == 1) {
    clapVal = analogRead(micPin);
    if (clapVal > clapSens)
    {
      count = 2;
      Serial.println("clap #2");
      digitalWrite(clapLED2, HIGH);
      delay(300);
      }
    
  } else if (count == 2) {
    clapVal = analogRead(micPin);
    if (clapVal > clapSens)
    {
      count = 3;
      Serial.println("clap #3");
      digitalWrite(clapLED3, HIGH);
      delay(300);
      }
    
  }
}

void loop()
{
  clap();
      
  //maintain MQTT connection
  client.loop();
  
  if (!client.connected()) {
    reconnect();
  }

}

