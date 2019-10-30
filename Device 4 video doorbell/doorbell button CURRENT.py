#sudo apt-get install python-rpi.gpio python3-rpi.gpio
#pip3 install paho-mqtt


#  https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/robot/buttons_and_switches/
# 


# We need to edit /etc/rc.local (as root since this is the owner).
# 
# sudo nano /etc/rc.local
# At the bottom, just above exit 0 we’ll add a call to our script.
# 
# python /home/pi/run.py



from signal import pause
import paho.mqtt.publish as publish
from time import sleep
from gpiozero import Button
import RPi.GPIO as GPIO


MQTT_SERVER = "192.168.1.25"
GPIO.setmode(GPIO.BCM)
button1=4

GPIO.setup(button1 ,GPIO.IN,pull_up_down=GPIO.PUD_DOWN)
print('press a button')

try:
    while True:
         if GPIO.input(4)==1:
           print('button1 pressed')
           print("Sending message...")
           publish.single("doorBell", "1", hostname=MQTT_SERVER)
           sleep(2)
except:
    GPIO.cleanup()
