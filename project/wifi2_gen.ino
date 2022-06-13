#include "painlessMesh.h"
#include <ArduinoJson.h>
// for node 2 wifi
#define MESH_PREFIX "whateverYouLike57"
#define MESH_PASSWORD "somethingSneaky57"
#define nodeName "wifi2"
#define MESH_PORT 5555
#define channel 6

const uint32_t nodeId1 = 2489133322;
const uint32_t nodeId2 = 758443432;
const uint32_t nodeId3 = 3257174137;
const uint32_t nodeId4 = 3265139431;

DynamicJsonDocument sendDoc(1024);

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

Generator temp(2, 10, 40);
Generator humid(3, 30, 90);

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

// User stub
void sendMessageTask(); // Prototype so PlatformIO doesn't complain
void updateSensor();

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessageTask);
Task taskUpdateSensor(TASK_SECOND * 1, TASK_FOREVER, &updateSensor);

void sendMessageTask()
{
  sendDoc["from"] = "wifi2";
  sendDoc["to"] = "wifi3";
  sendDoc["type"] = "gen";
  sendDoc["temp"] = temp.val();
  sendDoc["humid"] = humid.val();
  String msg = "";
  serializeJson(sendDoc, msg);
  if (mesh.isConnected(nodeId3)) // sending to wifi3
  {
    Serial.println("sent to wifi3");
    mesh.sendSingle(nodeId3, msg);
  }
  if (mesh.isConnected(nodeId1)) // sending to bridge
    mesh.sendSingle(nodeId1, msg);

  taskSendMessage.setInterval(TASK_SECOND * 20);
}

void updateSensor()
{
  temp.run();
  humid.run();
  taskUpdateSensor.setInterval(random(TASK_SECOND * 5, TASK_SECOND * 9));
}

// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
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
  userScheduler.addTask(taskUpdateSensor);

  taskSendMessage.enable();
  taskUpdateSensor.enable();
}

void loop()
{
  // it will run the user scheduler as well
  mesh.update();
}
