//************************************************************
// this is a simple example that uses the painlessMesh library to
// connect to a another network and relay messages from a MQTT broker to the nodes of the mesh network.
// To send a message to a mesh node, you can publish it to "painlessMesh/to/12345678" where 12345678 equals the nodeId.
// To broadcast a message to all nodes in the mesh you can publish it to "painlessMesh/to/broadcast".
// When you publish "getNodes" to "painlessMesh/to/gateway" you receive the mesh topology as JSON
// Every message from the mesh which is send to the gateway node will be published to "painlessMesh/from/12345678" where 12345678
// is the nodeId from which the packet was send.

// 2489133322-> node1
// 758443432 -> node 2
// 3257174137 -> node 3
// 3265139431 -> node 4
//************************************************************
//  for node 1 wifi
#include <Arduino.h>
#include <painlessMesh.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#define MESH_PREFIX "whateverYouLike57"
#define MESH_PASSWORD "somethingSneaky57"
#define MESH_PORT 5555
#define channel 6

#define STATION_SSID "OnePlus 7"
#define STATION_PASSWORD "wifi1234@Password.com"

#define HOSTNAME "MQTT_WIFI_Bridge"

#define wifi1_to "wifi/to/node1"
#define wifi2_to "wifi/to/node2"
#define wifi3_to "wifi/to/node3"
#define wifi4_to "wifi/to/node4"
#define wifi4_toi "wifi/to/nodi4"

#define wifi1_from "wifi/from/node1"
#define wifi2_from "wifi/from/node2"
#define wifi3_from "wifi/from/node3"
#define wifi4_from "wifi/from/node4"

const uint32_t nodeId1 = 2489133322;
const uint32_t nodeId2 = 758443432;
const uint32_t nodeId3 = 3257174137;
const uint32_t nodeId4 = 3265139431;

int state = 0;
DynamicJsonDocument sendDoc(1024);
DynamicJsonDocument docNode3(1024);

class Generator
{

  double randNumber;
  int timePeriod;
  int n = 5;
  double delaytlow;
  double delaythigh;
  int debug;
  int _ledpin;
  int _start;
  int _end;
  double diff;

public:
  Generator(int pin, int st, int ed);
  void run();
  int val();
};

Generator::Generator(int ledpin, int st, int ed)
{
  _ledpin = ledpin;
  _start = st;
  _end = ed;
  diff = ed - st;
  //    pinMode(_ledpin, OUTPUT);
  randomSeed(analogRead(A0));
  timePeriod = 1000;
  n = 10;
  debug = 0;
}
void Generator::run()
{
  if (n != 0)
  {
    if (debug)
    {
      Serial.print("current n: ");
      Serial.print(n);
      Serial.print(" current x: ");
      Serial.println(randNumber);
    }
    n -= 1;
  }
  else
  {
    randNumber = random(_start, _end);
    if (debug)
    {
      Serial.print("Generated number: ");
      Serial.println(randNumber);
    }
    n = 10;
  }
}
int Generator::val()
{
  return this->randNumber;
}

// Prototypes
void receivedCallback(const uint32_t &from, const String &msg);
void mqttCallback(char *topic, byte *payload, unsigned int length);
void sendMessageTask();
void updateSensor();

IPAddress getlocalIP();

IPAddress myIP(0, 0, 0, 0);
IPAddress mqttBroker(192, 168, 43, 84);
Generator temp(2, 10, 40);
Generator humid(3, 30, 90);

painlessMesh mesh;
Scheduler userScheduler;
WiFiClient wifiClient;
PubSubClient mqttClient(mqttBroker, 1883, mqttCallback, wifiClient);

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessageTask);
Task taskUpdateSensor(TASK_SECOND * 1, TASK_FOREVER, &updateSensor);

void sendMessageTask()
{
  sendDoc["from"] = "wifi1";
  sendDoc["to"] = "xbee1";
  sendDoc["type"] = "gen";
  sendDoc["temp"] = temp.val();
  sendDoc["humid"] = humid.val();

  String msg = "";
  serializeJson(sendDoc, msg);
  // Serial.println(msg);
  mqttClient.publish("wifi/from/node1", msg.c_str());
  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 8));
}
void updateSensor()
{
  temp.run();
  humid.run();
  taskUpdateSensor.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}

void setup()
{
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, channel);
  mesh.onReceive(&receivedCallback);
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);

  userScheduler.addTask(taskSendMessage);
  userScheduler.addTask(taskUpdateSensor);

  taskSendMessage.enable();
  taskUpdateSensor.enable();
}

void loop()
{
  mesh.update();
  mqttClient.loop();

  if (myIP != getlocalIP())
  {
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());

    if (mqttClient.connect("ESP_wifi_Client"))
    {
      mqttClient.publish("wifi/from/gateway", "Ready!");
      mqttClient.subscribe("wifi/to/gateway");
      mqttClient.subscribe(wifi1_to);
      mqttClient.subscribe(wifi2_to);
      mqttClient.subscribe(wifi3_to);
      mqttClient.subscribe(wifi4_to);
      mqttClient.subscribe(wifi4_toi);
    }
  }
}

void receivedCallback(const uint32_t &from, const String &msg)
{
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
  // recieve message from netwrork and send them to mqtt
  // mess-> {"from":"name","to":"name","paylaod":"msg"}
  String topic = "wifi/from/gateway";
  if (from == 758443432)
    topic = wifi2_from;
  else if (from == 3257174137)
    topic = wifi3_from;
  else if (from == 3265139431)
    topic = wifi4_from;
  mqttClient.publish(topic.c_str(), msg.c_str());
}

void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
  char *cleanPayload = (char *)malloc(length + 1);
  memcpy(cleanPayload, payload, length);
  cleanPayload[length] = '\0';
  String msg = String(cleanPayload);
  free(cleanPayload);
  // msg is the message

  String targetNode = String(topic).substring(8);

  // Serial.println(targetNode);
  if (targetNode == "node1")
  {
    // perform the action required
  }
  else if (targetNode == "node2")
  {
    if (mesh.isConnected(758443432))
      mesh.sendSingle(758443432, msg);
    else
      mqttClient.publish("wifi/from/gateway", "Client not connected!");
  }
  else if (targetNode == "node3")
  {
    // change the format of message
    deserializeJson(docNode3, msg);
    docNode3["from"] = "dashb";
    docNode3["to"] = "wifi3";
    String newMsg = "";
    serializeJson(docNode3, newMsg);
    if (mesh.isConnected(3257174137))
      mesh.sendSingle(3257174137, newMsg);
    else
      mqttClient.publish("wifi/from/gateway", "Client not connected!");
  }
  else if (targetNode == "nodi4")
  {

    deserializeJson(docNode3, msg);
    state = docNode3["state"];
    docNode3.clear();
    docNode3["from"] = "dashb";
    docNode3["to"] = "wifi3";
    docNode3["state"] = state;
    String newMsg1 = "";
    serializeJson(docNode3, newMsg1);

    if (mesh.isConnected(3265139431))
      mesh.sendSingle(3265139431, newMsg1);
    else
      mqttClient.publish("wifi/from/gateway", "Client not connected!");
  }
  else if (targetNode == "node4")
  {
    if (mesh.isConnected(3265139431))
      mesh.sendSingle(3265139431, msg);
    else
      mqttClient.publish("wifi/from/gateway", "Client not connected!");
  }
  else if (targetNode == "gateway")
  {
    if (msg == "getNodes")
    {
      auto nodes = mesh.getNodeList(true);
      String str;
      for (auto &&id : nodes)
        str += String(id) + String(" ");
      mqttClient.publish("wifi/from/gateway", str.c_str());
    }
  }
  else if (targetNode == "broadcast")
  {
    mesh.sendBroadcast(msg);
  }
  else
  {
    mqttClient.publish("wifi/from/gateway", "This node is not present in this nwtwork!");
  }
}

IPAddress getlocalIP()
{
  return IPAddress(mesh.getStationIP());
}
