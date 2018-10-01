#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define ONE_WIRE_BUS 5
// Warning led is on D0
#define LED_WARN D0
// Power led is on D2
#define LED_POWER D2

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

float Temperature=0;

// WiFi parameters
#define WLAN_SSID       "XXXXXXXX"
#define WLAN_PASS       "XXXXXXXX"
 
// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883
#define AIO_USERNAME    "XXXXXXXX"
#define AIO_KEY         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

// What is the temperature threshold?
#define WARNTEMP 20

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClientSecure client;

// io.adafruit.com SHA1 fingerprint
// This doesn't seem to work, though.
const char* fingerprint = "77 00 54 2D DA E7 D8 03 27 31 23 99 EB 27 DB CB A5 4C 57 18";
  
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Setup feeds for temperature & humidity
Adafruit_MQTT_Publish freezertemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/freezertemp");

int tempCleared = 0;
int overTemp = 0;

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();

  // initialize inputs and outputs
  // D1 is where I connected the temp probe
  pinMode(D1, INPUT);
  pinMode(LED_WARN, OUTPUT);
  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Turn on the power led
  digitalWrite(LED_POWER, HIGH);
  
  // Hi There
  Serial.println();
  Serial.println();
  Serial.println("Hi There! I'm a Temperature Monitor!");
  Serial.print("I alarm when the temp goes over ");
  Serial.print(WARNTEMP);
  Serial.println("F");
  Serial.print("I send mqtt data to ");
  Serial.print(AIO_SERVER);
  Serial.println();
  
  // Print the MAC in case you need to know
  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  // Connecting to WiFi network
  Serial.print("Connecting to wifi network ");
  Serial.println(WLAN_SSID);
 
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  // Print dots until connection succeeds
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Success!
  Serial.println();
  Serial.print("WiFi connected as ");
  Serial.println(WiFi.localIP());

  // make sure we are connecting to real io.adafruit
  if (client.verify(fingerprint, AIO_SERVER)) {
    Serial.println("Connection secure.");
  } else {
    Serial.println("Warning: inseucre connection!");
//    while(1);
  }
  
  // Make mqtt connection
  connect();

  // flash the onboard led to indicate we are setup and running
  // Note that you have to drive the onboard led LOW to turn it on.
  for (int i=0; i < 6; i++){
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }
}
 
void loop(void)
{
  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }
 
  sensors.requestTemperatures();
  // Convert temp to fahrenheit
  Temperature = sensors.getTempCByIndex(0) * (9.0/5.0) + 32.0;

  // Detect bogus temperatures and publish an invalid value
  // The One Wire code returns -196 degrees if the sensor is not
  // properly connected.
  if ( Temperature < -190 || Temperature > 150 ) {
    Serial.print("Invalid temperature reported: ");
    Serial.print(Temperature);
    Serial.println("F");
    // Flash the warning led a few times in a special pattern
    // to indicate failure.
    for (int i=0; i < 3; i++){
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
    }
    // Publish NaN to indicate bad data
    if (! freezertemp.publish("NaN"))
      Serial.println(F("Failed to publish invalid temperature (tried to publish NaN)"));
    else    
      Serial.println(F("Invalid Temperature published (as NaN), sleeping 10s"));
    
    delay(10000);
    // don't process rest of loop()
    return;
  }

  Serial.print("current temp: ");
  Serial.print(Temperature);
  Serial.println(" F");

  // if temp is too high, turn on the red internal led
  if ( Temperature > WARNTEMP ) {
    Serial.print("OVERTEMP: ");
    Serial.print(Temperature);
    Serial.print(" greater than ");
    Serial.print(WARNTEMP);
    Serial.println("F");
    // turn on the warning led, for visual feedback that temp is high
    digitalWrite(LED_WARN, HIGH);
    // set overtemp flag
    overTemp = 1;
  }
  
  if ( Temperature <= WARNTEMP && overTemp == 1 ){
    // just went safe
    overTemp = 0;
    tempCleared = 1;
  }
  
  if ( tempCleared == 1 ) {
    // yay we are cool enough now!
    // reset flag
    tempCleared = 0;
    // turn off the LED
    digitalWrite(LED_WARN, LOW);
    Serial.println("overtemp cleared!");
  }

  // Publish data
  if (! freezertemp.publish(Temperature))
    Serial.println(F("Failed to publish temperature"));
  else    
    Serial.println(F("Temperature published, sleeping 10s"));
  
  // flash the onboard led to show we are alive
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);

  // sleep 10 secs
  delay(10000);
}

// connect to adafruit io via MQTT
void connect() {
  
  Serial.print(F("Connecting to Adafruit IO... "));
  
  int8_t ret;
  
  while ((ret = mqtt.connect()) != 0) {
    
    switch (ret) {
    case 1: Serial.println(F("Wrong protocol")); break;
    case 2: Serial.println(F("ID rejected")); break;
    case 3: Serial.println(F("Server unavail")); break;
    case 4: Serial.println(F("Bad user/pass")); break;
    case 5: Serial.println(F("Not authed")); break;
    case 6: Serial.println(F("Failed to subscribe")); break;
    default: Serial.println(F("Connection failed")); break;
    }
    
    if(ret >= 0)
      mqtt.disconnect();
    
    Serial.println(F("Retrying connection..."));
    delay(5000);
  }
  Serial.println("Connected!");
  Serial.println();
}

