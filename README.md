# FreezerTempMonitor

## Introduction

This is a esp8266-based wireless freezer temperature monitor.  Actually, it's a
general purpose temperature monitor but I use it to monitor my freezer, so there.

Written for a Wemos D1 esp8266 board, but will run with any esp8266-based board
with small modifications.

Uses the adafruit.io mqtt service for data collection and ifttt for alerting.

Power is supplied via the micro-usb connector on the D1 mini board, so you can
power this with any usb cable.

Every 10 seconds, the device sends the current temperature reading to the
adafruit server.  That in turn is connected to an ifttt action.  If the
temperature is above the set value, ifttt fires an alert that goes to my phone.

When the temperature goes higher than the set value, the red led turns on.   

## Pictures and Diagrams

![completed temperature monitor](https://i.imgur.com/QfJXgjM.jpg)

![board layout](https://i.imgur.com/V8oLVk6.png)

![schematic](https://i.imgur.com/6s5ECs7.png)

## Parts List

* [Wemos D1 Mini](https://www.banggood.com/D1-Mini-NodeMcu-Lua-WIFI-ESP8266-Development-Board-p-1044858.html?)
    * Any D1 Mini will probably work - haven't tried lite or pro versions
* 220 ohm resistor
    * Exact value isn't critical, this just acts as a current limiter for the led.
    * I chose this size because it made my led neither too bright nor too dim.
* 100 ohm resistor
    * Same as 220 ohm resistor
* 4.7k ohm resistor
    * This is the specific value needed for parasitic power on the temperature sensor
* [DS18B20 temperature sensor](https://www.banggood.com/Wholesale-DS18B20-Waterproof-Digital-Temperature-Temp-Sensor-Probe-p-53674.html?)
    * You could use [the regular version of this sensor](https://www.banggood.com/DS18B20-Temperature-Sensor-DALLAS-18B20-TO-92-Encapsulation-p-91798.html?rmmds=buy) too if you just want to monitor environmental temperature
* red led
* green led
* [case](https://www.banggood.com/2pcs-75-x-54-x-27mm-DIY-Plastic-Project-Housing-Electronic-Junction-Case-Power-Supply-Box-p-1063302.html?rmmds=myorder)
* [magnet](https://www.banggood.com/N52-50mmx5mm-Countersunk-Ring-Magnet-Disc-Hole-6mm-Rare-Earth-Neodymium-Magnet-p-995060.html?rmmds=search)
    * I used this magnet to hold the case on to the front of my freezer.

## To Do

* Read config settings from adafruit.io instead of hard coding them.
* Store readings locally on the d1 flash
* Use setting read from adafruit.io for the alert value in the code and the alert value in ifttt
    * that way I don't have to track it in two places.
* Make this a generic temperature monitor by allowing trigger when temp goes to high OR too low.
* Use pwm to make the red led flash instead of just steady on
* Some sort of battery backup?
* Implement sleep mode to reduce power requirements
* Implement this on an esp32 Wemos mini for even better power savings
    * maybe make this battery powered
    * or at least have a battery backup
* Throw in humidity monitoring, other things?`
* Rework which pins are used so that wiring and potential pcb layout are cleaner.
* Get a pcb made so assembly is cleaner.
* Alert if device starts sending bad data - this probably means the temp probe has failed or been unplugged.
* Turn off builtin led entirely and flash the power led to indicate thigs like successful network connection
    * builtin led is obscured by case anyway.
* Add a buzzer!
* Rewrite in micropython or nodemcu
    * definitely want to use an esp32 device for that
