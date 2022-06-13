#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

DynamicJsonDocument doc(512);
DynamicJsonDocument doc2(512);
SoftwareSerial xbee(10, 11);

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

String recv = "";
String recv2 = "";

// Generator temp(13, 15, 50);  // range ->(1,100)
// Generator humid(3, 15, 100); // range ->(15,50)

String messageToSend()
{
  doc.clear();
  doc["from"] = "xbee1";
  doc["to"] = "dashb";
  doc["temp"] = ac.getTemp();
  doc["mode"] = ac.getMode();
  doc["state"] = ac.state();

  String msg = "";
  serializeJson(doc, msg);
  return msg;
}

void sendData(String message)
{
  //    String msg = messageToSend();
  xbee.write(message.c_str());
}
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
void setup()
{
  Serial.begin(9600);
  // Serial.println("Starting Node 1!");

  xbee.begin(9600);
  xbee.println("Hello, world?");
}

void loop()
{
  static uint32_t prev_ms = millis();
  if (millis() - prev_ms > 1000)
  {
    prev_ms = millis();
  }

  if (xbee.available())
  {
    // read from zigbee2 and send to Serial
    char c = xbee.read();
    recv += c;
    if (c == '}')
    {
      Serial.println(recv);
      recv = "";
    }
  }

  if (Serial.available())
  {
    char c = Serial.read();
    recv2 += c;
    if (c == '}')
    {
      doc2.clear();
      deserializeJson(doc2, recv2);

      String toNode = doc2["to"];
      if (toNode == "xbee2")
      {
        xbee.write(recv2.c_str());
      }
      else
      {
        // data from wifi4
        int temp = doc2["temp"];
        int humid = doc2["humid"];
        control(temp);
      }
      recv2 = "";
    }
  }
  // recv = "";
}
