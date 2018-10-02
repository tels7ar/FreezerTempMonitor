# FreezerTempMonitor

This is a esp8266-based wireless freezer temperature monitor.  Actually, it's a
general purpose temperature monitor but I use it to monitor my freezer, so there.

Written for a Wemos D1 esp8266 board, but will run with any esp8266-based board
with small modifications.

Uses the adafruit.io mqtt service for data collection and iftt for alerting.

Power is supplied via the micro-usb connector on the D1 mini board, so you can
power this with any usb cable.

Every 10 seconds, the device sends the current temperature reading to the
adafruit server.  That in turn is connected to an ifttt action.  If the
temperature is above the set value, ifttt fires an alert that goes to my phone.

When the temperature goes higher than the set value, the red led turns on.   

![completed temperature monitor](https://i.imgur.com/QfJXgjM.jpg)

![board layout](https://i.imgur.com/V8oLVk6.png)

![schematic](https://i.imgur.com/6s5ECs7.png)

## To Do

Read config settings from adafruit.io instead of hard coding them.

Store readings locally on the d1 flash

Use setting read from adafruit.io for the alert value in the code and the alert
value in ifttt, so I don't have to track it in two places.

Make this a generic temperature monitor by allowing trigger when temp goes to
high OR too low.

Use pwm to make the red led flash instead of just steady on

Some sort of battery backup?

Implement sleep mode to reduce power requirements

Implement this on an esp32 Wemos mini for even better power savings, and maybe
make this battery powered (or at least have a battery backup).

Throw in humidity monitoring, other things?`

Rework which pins are used so that wiring and potential pcb layout are cleaner.

Get a pcb made so assembly is cleaner.

Alert if device starts sending bad data - this probably means the temp probe
has failed or been unplugged.

Turn off builtin led entirely and flash the power led to indicate thigs like
successful network connection - since builtin led is obscured by case anyway.
