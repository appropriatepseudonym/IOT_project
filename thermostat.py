#ProgramFlow  
#subscribe to thermostat subject 
#if wantedTemp > or < than sensed temp, do something 


# install libs 
# sudo apt-get update
# sudo apt-get install build-essential python-dev python-openssl git
# git clone https://github.com/adafruit/Adafruit_Python_DHT.git && cd Adafruit_Python_DHT
# sudo python setup.py install
# cd examples
# sudo ./AdafruitDHT.py 11 4



import Adafruit_DHT
import paho.mqtt.client as mqtt #import the client1
import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(18,GPIO.OUT)

broker_address="192.168.1.184" 



def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    desiredTemp = str(message.payload.decode("utf-8")
    print("message topic=",message.topic)
    print("message qos=",message.qos)
    print("message retain flag=",message.retain)
    

def on_log(client, userdata, level, buf):
    print("log: ",buf)


broker_address="192.168.1.184"
#broker_address="iot.eclipse.org"

client1.on_log=on_log #for logging 

print("creating new instance")
client = mqtt.Client("P1") #create new instance
client.on_message=on_message #attach function to callback
print("connecting to broker")
client.connect(broker_address) #connect to broker
client.loop_start() #start the loop
print("Subscribing to topic","house/bulbs/bulb1")
client.subscribe("house/bulbs/bulb1")

#for temp sensor 
sensor = Adafruit_DHT.DHT11
pin = 4
humidity, temperature = Adafruit_DHT.read_retry(sensor, pin)

if (desiredTemp > temperature):
	GPIO.output(18,GPIO.HIGH)
else:  
	GPIO.output(18,GPIO.LOW)





# 
# print "LED on"
# GPIO.output(18,GPIO.HIGH)
# time.sleep(1)
# print "LED off"
# GPIO.output(18,GPIO.LOW)