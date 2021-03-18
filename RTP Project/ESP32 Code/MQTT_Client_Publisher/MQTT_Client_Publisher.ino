#include "PubSubClient.h" // Connect and publish to the MQTT broker

// Code for the ESP32
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)


const int xpin = 32;                  // x-axis of the accelerometer
const int ypin = 35;                  // y-axis
const int zpin = 34;                  // z-axis (only on 3-axis models)
const int redled = 17;
const int greenled = 16;

int device_id = 4545;

int data_index;
long hall_data;

float gx = 0;
float gy = 0;
float gz = 0;

float gx_prev = 0, gy_prev = 0, gz_prev = 0;

int xadc = 0; int yadc = 0; int zadc = 0;
int intensity = 0;
int action = 0;
int action_ts = 0;
int last_millis = 0;


// WiFi
const char* ssid = "NETGEAR86";                 // Your personal network SSID
const char* wifi_password = "happyvase635"; // Your personal network password

// MQTT
const char* mqtt_server = "192.168.1.25";  // IP of the MQTT broker
const char* Accelerometer_topic = "Accelerometer";
const char* Hall_Effect_Sensor_topic = "Hall_Effect";
const char* mqtt_username = "Tush"; // MQTT username
const char* mqtt_password = "Jeni"; // MQTT password
const char* clientID = "client_ESP32"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void setup() {
  Serial.begin(115200);
    connect_MQTT();
//  dht.begin();
}

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


//  // MQTT can only transmit strings
     String message_string_Accel = "{\"Device_ID\":" + String(device_id) + "," \
                            + "\"Intensity\":" +  String(intensity) + "," \
                            + "\"Status\":" +  String(intensity > 0) + "," \
                            + "\"xadc\":" + String(xadc) + "," + "\"yadc\":" + String(yadc) + "," + "\"zadc\":" + String(zadc) + "}" ;

      Serial.print("Accel string: " + message_string_Accel+ "\n");
    last_millis = millis();
    intensity = 0;
    //digitalWrite(greenled, HIGH);
    //digitalWrite(redled, HIGH);



//=====================================================================

hall_data=0;
for(data_index=0;data_index<1000;data_index++)
{
hall_data += hallRead();
delayMicroseconds(100);
}
Serial.print("Hall sensor value: ");
Serial.print((double)hall_data/1000);
Serial.println((double)hall_data/1000);
//  // MQTT can only transmit strings
String message_string_Hall =  "{\"Device_ID\":" + String(device_id) + "," \
                            + "\"Hall_sensor\":" +  String((double)hall_data/1000) + "}" ;

Serial.print("\n" + message_string_Hall+ "\n");
//======================================================================


    client.connect(clientID, mqtt_username, mqtt_password);
  // PUBLISH to the MQTT Broker (topic = Hall_Effect, defined at the beginning)
  if (client.publish(Hall_Effect_Sensor_topic, message_string_Hall.c_str())) {
    Serial.println("Hall Effect Data sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Hall Effect Data failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(Hall_Effect_Sensor_topic, message_string_Hall.c_str());
  }

  // PUBLISH to the MQTT Broker (topic = Accelerometer, defined at the beginning)
  if (client.publish(Accelerometer_topic, message_string_Accel.c_str())) {
    Serial.println("Accelerometer Data sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Accelerometer Data failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(Accelerometer_topic, message_string_Accel.c_str());
  }
  //client.disconnect();  // disconnect from the MQTT broker
  delay(2000);       // print new values every 2 second
}
