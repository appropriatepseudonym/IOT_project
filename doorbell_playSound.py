#!/usr/bin/env python

import os
from time import sleep
import context
import paho.mqtt.subscribe as subscribe
from time import sleep


MQTT_SERVER = "192.168.1.25"
topics = ['doorBell']


m = subscribe.simple(topics, hostname="MQTT_SERVER", retained=False, msg_count=2)
for a in m:
    print(a.topic)
    print(a.payload)
    if a.payload == 1
        os.system('mpg123 -q /Users/chad/Downloads/doorbell-1.mp3 &')
        a.payload = 0
    sleep(0.1);