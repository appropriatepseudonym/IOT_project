
/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

//pin assignment for leak sensor
//pin 26 buzzer
//pin 27 motion (touch7)
//pin 32 water (touch9)
//touchRead(pin)
//touch read on 32/33
https://github.com/swatish17/MQ7-Library 
https://github.com/adafruit/Adafruit_Sensor
https://www.instructables.com/id/I2C-between-Arduinos/

*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <Wire.h>
//#include "MQ7.h"


//#define DHTPIN 2     // what digital pin we're connected to , this is actuallt D4 on the nodeCMCU
#define DHTPIN 5     // what digital pin we're connected to , this is actuallt D1 on the nodeCMCU
#define DHTTYPE DHT11   // DHT 11
#define endLoopDelay 5000
#define LENG 31   //0x42 + 31 bytes equal to 32 bytes
unsigned char buf[LENG];


// Update these with values suitable for your network.

const char* ssid = "BUFFALO-G";
const char* password = "dontplayahate";
const char* mqtt_server = "192.168.0.20";

WiFiClient espClient;
PubSubClient client(espClient);
SoftwareSerial pmsSerial(D7, D8);
DHT dht(DHTPIN, DHTTYPE);
//MQ7 mq7(A0,5.0);

long lastMsg = 0;
char msg[50];
int value = 0;
int latestCOppm = -1;
int PM01Value=0;          //define PM1.0 value of the air detector module
int PM2_5Value=0;         //define PM2.5 value of the air detector module
int PM10Value=0;         //define PM10 value of the air detector module
int count=0;
int Tcount = 0;

char checkValue(unsigned char *thebuf, char leng)
{  
  char receiveflag=0;
  int receiveSum=0;

  for(int i=0; i<(leng-2); i++){
  receiveSum=receiveSum+thebuf[i];
  }
  receiveSum=receiveSum + 0x42;
 
  if(receiveSum == ((thebuf[leng-2]<<8)+thebuf[leng-1]))  //check the serial data 
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}


int transmitPM01(unsigned char *thebuf)
{
  int PM01Val;
  PM01Val=((thebuf[3]<<8) + thebuf[4]); //count PM1.0 value of the air detector module
  return PM01Val;
}

//transmit PM Value to PC
int transmitPM2_5(unsigned char *thebuf)
{
  int PM2_5Val;
  PM2_5Val=((thebuf[5]<<8) + thebuf[6]);//count PM2.5 value of the air detector module
  return PM2_5Val;
  }

//transmit PM Value to PC
int transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val=((thebuf[7]<<8) + thebuf[8]); //count PM10 value of the air detector module  
  return PM10Val;
}




void receiveEvent(int bytes) {
  latestCOppm= Wire.read();    // read one character from the I2C
}

// this callback function receives a message and acts on it
//need to remap this to the individual devices
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("myhome/bedroom/switch1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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

void setup() {
  Wire.begin(9); 
  Wire.onReceive(receiveEvent);
  //from previous code 
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(27,INPUT); //water
  pinMode(32,INPUT); //motion
  pinMode(26,OUTPUT); //buzzer
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  //for the sensrs
  Serial.println("DHTxx test!");
    dht.begin();
    Serial.println("PMS test!");
    // sensor baud rate is 9600
    pmsSerial.begin(9600);
    pmsSerial.setTimeout(1500);    

}







  
 



void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
char buffer[20]; 


 if(pmsSerial.find(0x42)){    
    pmsSerial.readBytes(buf,LENG);

    if(buf[0] == 0x4d){
      if(checkValue(buf,LENG)){
        PM01Value=transmitPM01(buf); //count PM1.0 value of the air detector module
        PM2_5Value=transmitPM2_5(buf);//count PM2.5 value of the air detector module
        PM10Value=transmitPM10(buf); //count PM10 value of the air detector module 
      }           
    } 
  }
  
   static unsigned long OledTimer=millis();  
    if (millis() - OledTimer >=2000) 
    {
      OledTimer=millis(); 
      
      Serial.print("PM1.0: ");  
      Serial.print(PM01Value);
      Serial.println("  ug/m3");            
    
      Serial.print("PM2.5: ");  
      Serial.print(PM2_5Value);
      Serial.println("  ug/m3");     
      
      Serial.print("PM1 0: ");  
      Serial.print(PM10Value);
      Serial.println("  ug/m3");   
      Serial.println();
    }

    
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    //return;
  }

  // Compute heat index in Fahrenheit (the default)
  //float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
              //client.publish("device3/humidity", itoa(h, buffer,10)); 
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
               // client.publish("device3/tempC", itoa(t, buffer,10)); 
  Serial.print(f);
  Serial.print(" *F\t");
                // client.publish("device3/tempF", itoa(f, buffer,10)); 
  Serial.print("\n ");

if (f <= 200 ){
  if (Tcount > 10){
  client.publish("device3/humidity", itoa(h, buffer,10));
  client.publish("device3/tempC", itoa(t, buffer,10)); 
  client.publish("device3/tempF", itoa(f, buffer,10)); 
  Tcount = 0;
  }
  
} 

  
//  Serial.print(hic);
//  Serial.print(" *C ");
//                client.publish("device3/heatIndC", itoa(hic, buffer,10)); 
//  Serial.print(hif);
//  Serial.println(" *F");
//              client.publish("device3/heatIndF", itoa(hif, buffer,10)); 
//  COPPM = mq7.getPPM();
//  Serial.print("Heat index: ");
//  Serial.print(latestCOppm);
//client.publish("device3/CO", itoa(latestCOppm, buffer,10)); 
  if (count > 10){
  client.publish("device3/PM01Value", itoa(PM01Value, buffer,10)); 
  client.publish("device3/PM2_5Value", itoa(PM2_5Value, buffer,10)); 
  client.publish("device3/PM10Value", itoa(PM10Value, buffer,10));
  count=0;
} 

//PM01Value=0;          //define PM1.0 value of the air detector module
//PM2_5Value=0;         //define PM2.5 value of the air detector module
//PM10Value=0;         //define PM10 value of the air detector module
  

delay(1000);
//Serial.print(count);
count++;
Tcount++;
}