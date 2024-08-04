#include <WiFi.h>
#include <WebSockets.h>
#include <WebSocketsClient.h>

const char* ssid = "VIRGIN313";
const char* password = "A25D7613CDFF";
const char* wsIP = "192.168.2.12";
const char* path = "/";
const int port = 81;

WebSocketsClient ws;

#define LED 2
int animID = -1;
bool ledIsOn = false;
unsigned long animDurations[3] = {150, 900, 1200};
unsigned long animTimeSave = 0;
unsigned long strobeSpeeds[3] = {1000000, 80, 27};
unsigned long strobeTimeSave = 0;

//===========================================================

//function process event: new data received from client
// typedef enum {
//     WStype_ERROR,
//     WStype_DISCONNECTED,
//     WStype_CONNECTED,
//     WStype_TEXT,
//     WStype_BIN,
//     WStype_FRAGMENT_TEXT_START,
//     WStype_FRAGMENT_BIN_START,
//     WStype_FRAGMENT,
//     WStype_FRAGMENT_FIN,
//     WStype_PING,
//     WStype_PONG,
// } WStype_t;
void webSocketEvent(WStype_t type, uint8_t *payload, size_t len)
{
  if (type == WStype_CONNECTED){
    String json = "{\"userInfo\":\"esp32\"}";
    ws.sendTXT(json);
    Serial.println("Connected! Sending userInfo to server");

    animID = 0;
    animTimeSave = millis();
    strobeTimeSave = millis();
    digitalWrite(LED, HIGH);
  }
  else if (type == WStype_TEXT){
    int data = *payload - 48;
    Serial.print("Message received! Message: ");
    Serial.println(data);

    if (animID == -1){
      animID = data;
      animTimeSave = millis();
      strobeTimeSave = millis();
      digitalWrite(LED, HIGH);
    }
  }
}

void setup()
{
  //Serial init
  Serial.begin(115200);
  delay(10);
  
  //connect to wifi router
  Serial.print("Connecting to Wifi: ");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" success!");
  WiFi.mode(WIFI_STA);
  Serial.print("Local IP: "); Serial.println(WiFi.localIP());

  //init web socket
  ws.begin(wsIP, port, "/");
  ws.onEvent(webSocketEvent);
  ws.setReconnectInterval(5000);

  //led setup
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

void loop()
{
  ws.loop();

  if (animID > -1){
    if (millis() - animTimeSave > animDurations[animID]){
      digitalWrite(LED, LOW);
      animID = -1;
      ledIsOn = false;
    }
    else if (millis() - strobeTimeSave > strobeSpeeds[animID]){
      strobeTimeSave = millis();
      ledIsOn = !ledIsOn;
      digitalWrite(LED, ledIsOn);
    }
  }
}

