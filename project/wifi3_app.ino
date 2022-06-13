//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
// for node 3 wifi
//************************************************************
#include "painlessMesh.h"
#include <ArduinoJson.h>

#define MESH_PREFIX "whateverYouLike57"
#define MESH_PASSWORD "somethingSneaky57"
#define MESH_PORT 5555
#define nodeName "wifi3"
#define channel 6

const uint32_t nodeId1 = 2489133322;
const uint32_t nodeId2 = 758443432;
const uint32_t nodeId3 = 3257174137;
const uint32_t nodeId4 = 3265139431;

DynamicJsonDocument sendDoc(1024);
DynamicJsonDocument recvDoc(1024);

class AC
{
  int pin;
  int _mode; // 0->cool,1->fan only,2->hot
  int debug;
  int _state;
  int _temp;

public:
  AC(int _pin)
  {
    this->pin = _pin;
    debug = 0;
    _mode = 0;
    _state = 0;
    _temp = 24;
  }
  void setMode(int mode)
  {
    this->_mode = mode;
  }
  int getMode()
  {
    return this->_mode;
  }
  void toggleMode()
  {
    this->_mode = (this->_mode + 1) % 3;
  }
  void off()
  {
    this->_state = 0;
  }
  void on()
  {
    this->_state = 1;
    this->_mode = 0;
  }
  int state()
  {
    return this->_state;
  }
  void setTemp(int temp)
  {
    this->_temp = temp;
  }
  int getTemp()
  {
    return this->_temp;
  }
};

AC ac(2);

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

// User stub
void sendMessageTask(); // Prototype so PlatformIO doesn't complain
// void updateSensor();

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessageTask);

void sendHelper(int change = 0)
{
  sendDoc["from"] = "wifi3";
  sendDoc["to"] = "dashb";
  sendDoc["type"] = "app";
  sendDoc["state"] = ac.state();
  sendDoc["temp"] = ac.getTemp();
  sendDoc["mode"] = ac.getMode();
  if (change == 1)
  {
    sendDoc["by"] = "Sensor";
  }
  else if (change == 2)
  {
    sendDoc["by"] = "dashboard";
  }
  else
  {
    sendDoc["by"] = "no";
  }
  String msg = "";
  serializeJson(sendDoc, msg);
  if (mesh.isConnected(nodeId1)) // sending to bridge
    mesh.sendSingle(nodeId1, msg);
}
void sendMessageTask()
{
  sendHelper(0);
  taskSendMessage.setInterval(TASK_SECOND * 5);
}
// void updateSensor()
// {
//   temp.run();
//   humid.run();
//   taskUpdateSensor.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
// }
void control(int temp)
{
  if (temp < 14)
  {
    ac.setMode(2);
    ac.setTemp(23);
  }
  else if (temp > 14 && temp < 20)
  {
    ac.on();
    ac.setMode(1);
    ac.setTemp(23);
  }
  else if (temp > 20 && temp < 24)
  {
    ac.off();
  }
  else if (temp > 24)
  {
    ac.on();
    ac.setTemp(23);
  }
}
// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
  recvDoc.clear();
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  deserializeJson(recvDoc, msg);
  // String msg1 = "";
  // serializeJson(recvDoc, msg1);
  // serializeJson(recvDoc, msg1);
  // 39 only state
  // 48 when state
  // 50
  // {"state":1,"from":"dashb","to":"wifi3","temperature":23}
  // {"state":1,"from":"dashb","to":"wifi3","mode":2}

  if (recvDoc["from"] == "dashb")
  {
    Serial.println("Recieved from Dashboard");
    // change state
    if (msg.length() > 40)
    {
      if (msg.length() > 50 && recvDoc["temp"] != NULL)
      {
        Serial.println("Temp changed");
        int x = recvDoc["temp"];
        ac.setTemp(x);
        sendDoc["temp"] = ac.getTemp();
      }
      if (msg.length() > 45 && recvDoc["mode"] != NULL)
      {
        Serial.println("Mode changed");
        int x = recvDoc["mode"];
        ac.setMode(x);
        Serial.println(ac.getMode());
        sendDoc["mode"] = ac.getMode();
      }
    }
    if (recvDoc["state"] != NULL)
    {
      if (recvDoc["state"] == 0)
      {
        Serial.println("AC turned OFF");
        ac.off();
        sendDoc["state"] = ac.state();
      }
      else if (recvDoc["state"] == 1)
      {
        Serial.println("AC turned ON");
        ac.on();
        sendDoc["state"] = ac.state();
      }
    }
    sendHelper(2);
    // send update to my topic
  }
  else
  {
    // control
    Serial.println("Recieved from Sensor");
    int temp = recvDoc["temp"];
    int humid = recvDoc["humid"];
    static uint32_t prev_ms = millis();
    if (millis() - prev_ms > 20000)
    {
      prev_ms = millis();
      control(temp);
      sendHelper(1);
    }
    // send updates to my topic
  }
  // perform the action using temp;
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup()
{
  Serial.begin(115200);

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, channel);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  // userScheduler.addTask(taskUpdateSensor);

  taskSendMessage.enable();
  // taskUpdateSensor.enable();
}

void loop()
{
  // it will run the user scheduler as well
  mesh.update();
}
