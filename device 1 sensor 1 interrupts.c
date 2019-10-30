
#include <WiFi.h>
#include <PubSubClient.h>


#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */
#define BUTTON_PIN_BITMASK 0x100000000 // 2^32 in hex


//#define Threshold 30 /* Greater the value, more the sensitivity */

RTC_DATA_ATTR int bootCount = 0;

// Update these with values suitable for your network.

const char* ssid = "EHDN";
const char* password = "buffalo-tp";
const char* mqtt_server = "192.168.1.121";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int interruptPin = 32;


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
     //client.subscribe("myhome/bedroom/switch1");
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

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}


void running(){

   if (!client.connected()) {
   reconnect();
  }
 client.loop();
  
// this handles sending the values back to the controller
// we would have this perodically check a temp and then send it back 
// or we would have this check once a second if the pin is high (water) and send alert
  
  
  
    if (digitalRead(32))  //or 32/9
    {
      digitalWrite(5, HIGH);
      delay(50);
      digitalWrite(5, LOW);
//     digitalWrite(26, HIGH);
//     delay(50);
//     digitalWrite(26, LOW);
      snprintf (msg, 75, "water detected");
      Serial.print("Publish message: water /n");
      Serial.println(msg);
      //Serial.print(digitalRead(32));
     client.publish("sensor1/water", msg);  //change this to the individual device topic
    }
}


void setup(){

  pinMode(5, OUTPUT);  // Initialize the BUILTIN_LED pin as an output
  pinMode(26,OUTPUT); //buzzer
  pinMode(interruptPin,INPUT); 

  
  
  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  running();
  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");


/*
  First we configure the wake up source
  We set our ESP32 to wake up for an external trigger.
  There are two types for ESP32, ext0 and ext1 .
  ext0 uses RTC_IO to wakeup thus requires RTC peripherals
  to be on while ext1 uses RTC Controller so doesnt need
  peripherals to be powered on.
  Note that using internal pullups/pulldowns also requires
  RTC peripherals to be turned on.
  */
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_32,1); //1 = High, 0 = Low




  /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop(){
  //This is not going to be called
}