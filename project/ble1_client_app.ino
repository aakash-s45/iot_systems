#include "BLEDevice.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "OnePlus 7";
const char *pass = "wifi1234@Password.com";

const char *topictoGateway = "ble/to/gateway";
const char *topicto1 = "ble/to/node1";     // (sub)
const char *topicto2 = "ble/to/node2";     // (sub)
const char *topicfrom1 = "ble/from/node1"; // (pub)
const char *topicfrom2 = "ble/from/node2"; // (pub)

const char *broker = "192.168.43.84";
const int port = 1883;
String bleMessage;

WiFiClient client;
PubSubClient mqtt(client);

DynamicJsonDocument sendDoc(1024);
DynamicJsonDocument recvDoc(1024);

// The remote service we wish to connect to.
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
// to recieve messages from peripheral
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");
// to send messages to peripheral
static BLEUUID prxcharUUID("cea8a91c-d81a-11ec-9d64-0242ac120002");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLERemoteCharacteristic *prxRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;

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

void control(int temp)
{
  Serial.println("Changed from sensor data");
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

static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
  Serial.print("Notify callback: ");
  // Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  // Serial.print(" of data length ");
  // Serial.println(length);
  Serial.print("data: ");
  Serial.println((char *)pData);
}

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient *pclient)
  {
  }

  void onDisconnect(BLEClient *pclient)
  {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer()
{
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice); // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  pClient->setMTU(517); // set client to request maximum MTU from server (default is 23 otherwise)

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  prxRemoteCharacteristic = pRemoteService->getCharacteristic(prxcharUUID);

  if (pRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  if (prxRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(prxcharUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead())
  {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("Characteristic value recieved: ");
    Serial.println(value.c_str());
  }
  if (prxRemoteCharacteristic->canRead())
  {
    std::string value = prxRemoteCharacteristic->readValue();
    Serial.print("Characteristic value sending: ");
    Serial.println(value.c_str());
  }

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  if (prxRemoteCharacteristic->canNotify())
    prxRemoteCharacteristic->registerForNotify(notifyCallback);

  connected = true;
  return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
    {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  }   // onResult
};    // MyAdvertisedDeviceCallbacks

void mqttcallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  String message;
  String targetNode = String(topic).substring(7);
  Serial.println(targetNode);
  // ble/to/node2
  for (int i = 0; i < length; i++)
  {
    message = message + (char)payload[i]; // convert *byte to string
  }
  if (targetNode == "node2" && connected)
  {
    Serial.println("Setting new characteristic value to \"" + message + "\"");
    // sedning the message recieved from mqtt via ble to peripheral
    prxRemoteCharacteristic->writeValue(message.c_str(), message.length());
  }
  else
  {

    // control appliances
    Serial.print("Message:");
    Serial.println(message);
    Serial.println("-----------------------");
    // read the message and control the appliances
    recvDoc.clear();
    deserializeJson(recvDoc, message);
    if (recvDoc["from"] != "xbee1")
    {
      int state = recvDoc["state"];
      if (state)
      {
        Serial.println("AC turned on");
        ac.on();
      }
      else if (!state)
      {
        Serial.println("AC turned off");
        ac.off();
      }
    }
    else
    {
      int temp1 = recvDoc["temp"];
      Serial.println(temp1);
      control(temp1);
      // recieved from xbee2
      // control according to temperature
    }
  }
}

void connectToWiFi()
{
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());
}

void connectToMQTT()
{
  // connecting to a MQTT Broker
  mqtt.setServer(broker, port);
  mqtt.setCallback(mqttcallback);
  if (mqtt.connect("ESP32_ble"))
  {
    Serial.println("connected");
  }
  else
  {
    Serial.print("failed with state ");
    Serial.println(mqtt.state());
    delay(2000);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  // scan the ble devices
  BLEScan *pBLEScan = BLEDevice::getScan();
  // check if the scanned device have the required service
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  // conenct to wifi and mqtt
  connectToWiFi();
  connectToMQTT();
  // subscribe to mqtt
  mqtt.subscribe(topicto1);
  mqtt.subscribe(topicto2);
}

void loop()
{
  // publishing data to mqtt
  mqtt.loop();

  static uint32_t prev_ms = millis();
  if (millis() > prev_ms + 1000)
  {

    prev_ms = millis();
    sendDoc.clear();
    sendDoc["from"] = "ble_1";
    sendDoc["to"] = "dashb";
    sendDoc["temp"] = ac.getTemp();
    sendDoc["mode"] = ac.getMode();
    sendDoc["state"] = ac.state();
    String msg1 = "";
    serializeJson(sendDoc, msg1);
    mqtt.publish(topicfrom1, msg1.c_str());

    mqtt.publish(topicfrom2, bleMessage.c_str());
    // read the message revieved over ble and update the message
    if (pRemoteCharacteristic->canRead())
    {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("Char Val recieved: ");
      bleMessage = value.c_str();
      Serial.println(bleMessage);
    }
  }
  // doConnect=> we have found the desired device and now we have to conenct to it
  if (doConnect == true)
  {
    if (connectToServer())
    {
      Serial.println("We are now connected to the BLE Server.");
    }
    else
    {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // to reconnect if disconnected
  if (doScan && !connected)

    if (!connected && doScan)
    {
      BLEDevice::getScan()->start(0);
    }

  delay(1000); // Delay a second between loops.
}
