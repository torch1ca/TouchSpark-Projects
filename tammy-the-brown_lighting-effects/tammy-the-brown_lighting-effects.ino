#include <WiFi.h>
#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <esp_dmx.h>

const char* ssid = "VIRGIN313";
const char* password = "A25D7613CDFF";
const char* wsIP = "192.168.2.12";
const char* path = "/";
const int port = 81;

WebSocketsClient ws;

#define LED 2
int animID = -1;
bool dmxIsOn = false;
int lightNum = 1;
unsigned long animDurations[3] = {70, 900, 1200};
unsigned long animTimeSave = 0;
unsigned long strobeSpeeds[3] = {1000000, 80, 27};
unsigned long strobeTimeSave = 0;
int flashColours[3][3] = {
  {255, 255, 255},
  {255, 0, 215},
  {0, 255, 225}
};

//DMX Lights
#define DMX_COUNT 4
int transmitPin = 17;
int receivePin = 16;
int enablePin = 21;
dmx_port_t dmxPort = 1;
#define DMX_PCT_SIZE 29
byte dmxData[DMX_PCT_SIZE] = {};
bool queueDmxSend = false;

//===========================================================

void setDmxData() {
  for (int light = 0; light < DMX_COUNT; light++) {
    for (int i = 0; i < 3; i++) {
      int offCol[3] = {};
      int *onCol = flashColours[animID];
      int activeLight = animID == 0 ? lightNum : light;
      dmxData[activeLight * 7 + 2 + i] = dmxIsOn ? *(onCol + i) : offCol[i];
    }
  }

  queueDmxSend = true;
}

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

    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (type == WStype_DISCONNECTED){
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (type == WStype_TEXT){
    int data = *payload - 48;
    Serial.print("Message received! Message: ");
    Serial.println(data);

    if (animID == -1){
      animID = data;
      animTimeSave = millis();
      strobeTimeSave = millis();
      lightNum = random(0,4);
      dmxIsOn = true;
      setDmxData();
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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //dmx init
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_personality_t personalities[] = {};
  int personality_count = 0;
  dmx_driver_install(dmxPort, &config, personalities, personality_count);
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);

  dmxData[1] = 155;
  dmxData[8] = 155;
  dmxData[15] = 155;
  dmxData[22] = 155;
}

void loop()
{
  ws.loop();

  if (animID > -1){
    if (millis() - animTimeSave > animDurations[animID]){
      animID = -1;
      dmxIsOn = false;
      setDmxData();
    }
    else if (millis() - strobeTimeSave > strobeSpeeds[animID]){
      strobeTimeSave = millis();
      dmxIsOn = !dmxIsOn;
      setDmxData();
    }
  }

  // if(queueDmxSend){
    dmx_wait_sent(dmxPort, DMX_PCT_SIZE);
    dmx_write(dmxPort, dmxData, DMX_PCT_SIZE);
    dmx_send_num(dmxPort, DMX_PCT_SIZE);
    queueDmxSend = false;
  // }
}

