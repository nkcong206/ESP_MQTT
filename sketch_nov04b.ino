#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define analogPin A0

#define led D5

#define button D1

#define ssid "Huong Doan"
#define password "0123456789"

#define mqtt_server "192.168.1.24"
const uint16_t mqtt_port = 1883;

#define mqtt_topic_pub "device_2" 
#define deviceId "cam_bien"


WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

unsigned long waitTime = 150; 
boolean ledStatus = 0; 
boolean lastButtonStatus = 0;
boolean buttonLongPress = 0;
unsigned long lastChangedTime;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  pinMode(button, INPUT);
  pinMode(led,OUTPUT);
  client.setServer(mqtt_server, mqtt_port); 
}
void setup_wifi() 
{
  delay(10);
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

void reconnect() 
{
  while (!client.connected()) // Chờ tới khi kết nối
  {
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect(deviceId))  //kết nối vào broker
    {
      Serial.println("Đã kết nối:");
    }
    else 
    {
      Serial.print("Lỗi:, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}


void loop() {
   
  // put your main code here, to run repeatedly:
   if (!client.connected())// Kiểm tra kết nối
    reconnect();
  client.loop();
  long now = millis();
if (now - lastMsg > 2000) {
    lastMsg = now;
    value = analogRead(analogPin);
    snprintf (msg, 75, "{\"type\":\"sensor\",\"value\":%ld}", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(mqtt_topic_pub, msg);
   }
   
  boolean reading = digitalRead(button);
  if (reading != lastButtonStatus) { 
    lastButtonStatus = reading; 
    lastChangedTime = millis(); 
    
  } 
  if (millis() - lastChangedTime > waitTime) { 
    buttonLongPress = reading;
    lastChangedTime = millis();
  }
  
  if (buttonLongPress == true) {
    ledStatus = !ledStatus;
    snprintf (msg, 75, "{\"type\":\"button\",\"value\":%ld}", ledStatus);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(mqtt_topic_pub, msg);
    buttonLongPress = false; 
  }
  digitalWrite(led,ledStatus);
}
