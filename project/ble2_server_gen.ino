
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ArduinoJson.h>
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define rxCHARACTERISTIC_UUID "cea8a91c-d81a-11ec-9d64-0242ac120002"
bool deviceConnected = false;
DynamicJsonDocument doc(512);

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
  pinMode(_ledpin, OUTPUT);
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
    delaytlow = (timePeriod * randNumber) / diff;
    delaythigh = timePeriod * (1.0 - (randNumber / diff));
  }
}
int Generator::val()
{
  return this->randNumber;
}

Generator temp(13, 15, 50);  // range ->(1,100)
Generator humid(3, 15, 100); // range ->(15,50)
BLECharacteristic *pCharacteristic;

String messageToSend()
{
  doc["from"] = "ble_2";
  doc["to"] = "wifi4";
  doc["temp"] = temp.val();
  doc["humid"] = humid.val();
  String msg = "";
  serializeJson(doc, msg);
  return msg;
}

void updateSensor()
{
  temp.run();
  humid.run();
}
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *prxCharacteristic)
  {
    std::string rxValue = prxCharacteristic->getValue();

    if (rxValue.length() > 0)
    {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++)
        Serial.print(rxValue[i]);

      Serial.println();
      Serial.println("*********");
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("My BLE Server");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);
  // BLECharacteristic::PROPERTY_NOTIFY);
  // BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  BLECharacteristic *prxCharacteristic = pService->createCharacteristic(
      rxCHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("{ Communication started! }");
  prxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it!");
}

void loop()
{

  static uint32_t prev_ms = millis();
  if (millis() > prev_ms + 1000)
  {
    prev_ms = millis();
    pCharacteristic->setValue(messageToSend().c_str());
  }
  updateSensor();
  // delay(2000);
}
