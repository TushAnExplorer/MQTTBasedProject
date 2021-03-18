/*
 * Acceleromter based intensity (slow vs fast pickup) of Item picked up 
 * 
*/


//#include "EspMQTTClient.h"

const int xpin = 32;                  // x-axis of the accelerometer
const int ypin = 35;                  // y-axis
const int zpin = 34;                  // z-axis (only on 3-axis models)
const int redled = 17;
const int greenled = 16;

int device_id = 4545;

//EspMQTTClient client(
//  "DM_hotspot",
//  "abcd1234",
//  "broker.hivemq.com",  // MQTT Broker server ip
//  "TestClientTest",     // Client name that uniquely identify your device
//  1883              // The MQTT port, default to 1883. this line can be omitted
//);


void setup() {
  // initialize the serial communications:
  Serial.begin(115200);

  //pinMode(greenled, OUTPUT);
 // pinMode(redled, OUTPUT);
  //digitalWrite(greenled, LOW);
  //digitalWrite(redled, LOW);

  //client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
}

//void onConnectionEstablished()
//{
//  //client.subscribe("mytopic/kitchenMotion", [](const String & payload) {
//    //Serial.println(payload);
//  //});
//
//  client.publish("mytopic/kitchenMotion", "Device booted"); // You can activate the retain flag by setting the third parameter to true
//}

float gx = 0;
float gy = 0;
float gz = 0;

float gx_prev = 0, gy_prev = 0, gz_prev = 0;

int xadc = 0; int yadc = 0; int zadc = 0;
int intensity = 0;
int action = 0;
int action_ts = 0;
int last_millis = 0;

void loop() {
  xadc = 0;
  yadc = 0;
  zadc = 0;
  for (int i = 0; i < 100; i++)
  {
    xadc += analogRead(xpin);
    yadc += analogRead(ypin);
    zadc += analogRead(zpin);
    delayMicroseconds(100);
  }
  xadc /= 100;
  yadc /= 100;
  zadc /= 100;

  gx_prev = gx;
  gy_prev = gy;
  gz_prev = gz;
  gx = (xadc - 2048) / 330.0;
  gy = (yadc - 2048) / 330.0;
  gz = (zadc - 2048) / 330.0;

  float g = sqrt(gx * gx + gy * gy + gz * gz);
  float g_prev = sqrt(gx_prev * gx_prev + gy_prev * gy_prev + gz_prev * gz_prev);
  //  Serial.printf("g: %.3f,\t gx: %.3f,\t gy: %.3f,\t gz: %.3f \n", g, gx, gy, gz);

  int intensity_prev = intensity;
  Serial.printf("delta: %.1f \t", abs(g - g_prev));
  if (abs(g - g_prev) <= 0.1) {
    //No motion
    Serial.println("");
  }
  else if (abs(g - g_prev) <= 0.75) {
    //Slow motion
    if (intensity_prev <= 1) {
      action = 1;
      intensity = 1;
      action_ts = millis();
      //digitalWrite(greenled, LOW);
     // digitalWrite(redled, HIGH);
      Serial.println("LOW");
    }
    else Serial.println("LOW skipped");
  }
  else if (abs(g - g_prev) <= 2.5) {
    //Med motion
    if (intensity_prev <= 2) {
      intensity = 2;
      action = 1;
      action_ts = millis();
      //digitalWrite(greenled, LOW);
      //digitalWrite(redled, LOW);
      Serial.println("MED");
    }
    else Serial.println("MID skipped");
  }
  else if (abs(g - g_prev) > 2.5 ) {
    //Fast motion
    intensity = 3;
    action = 1;
    action_ts = millis();
    //digitalWrite(greenled, HIGH);
    //digitalWrite(redled, LOW);
    Serial.println("HIGH");
  }

  if (millis() - last_millis >= 5000) {
    String message_string = "{\"TS\":" + String(millis()) + "," \
                            + "\"Device ID\":" + String(device_id) + "," \
                            + "\"Intensity\":" +  String(intensity) + "," \
                            + "\"Status\":" +  String(intensity > 0) + "," \
                            + "\"xadc\":" + String(xadc) + "," + "\"yadc\":" + String(yadc) + "," + "\"zadc\":" + String(zadc) + "}" ;
    //client.publish("mytopic/kitchenMotion", message_string);
      Serial.print(message_string);
    last_millis = millis();
    intensity = 0;
    //digitalWrite(greenled, HIGH);
    //digitalWrite(redled, HIGH);
  }
  delay(100);
  //client.loop();

}
