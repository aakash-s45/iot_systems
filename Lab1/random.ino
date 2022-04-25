

class Generator{

    double randNumber;
    int timePeriod;
    int n = 10;
    double delaytlow;
    double delaythigh;
    int debug;
    int _ledpin;

public:
    Generator(int pin);
    void run();
    int val();
};

Generator::Generator(int ledpin){
    _ledpin = ledpin;
    pinMode(_ledpin, OUTPUT);
    randomSeed(analogRead(2));
    timePeriod = 2000;
    n=10;
    debug=1;
}
void Generator::run(){
    if (n != 0){
      if(debug){
         Serial.print("current n: ");
         Serial.print(n);
         Serial.print(" current x: ");
         Serial.println(randNumber);
      }
      n -= 1;
      digitalWrite(_ledpin, HIGH);
      delay(delaytlow);
      digitalWrite(_ledpin, LOW);
      delay(delaytlow);
   }
   else
   {
      randNumber = random(10);
      if(debug){
        Serial.print("Generated number: ");
        Serial.println(randNumber);
      }
      n = 10;
      delaytlow = (timePeriod * randNumber) / 9;
      delaythigh = timePeriod * (1.0 - (randNumber / 9.0));
   }
}
int Generator::val(){
    return this->randNumber;
}

Generator light(3);

// Generator fan(1);

void setup() {
    Serial.begin(9600);
}

void loop() {
    light.run();
}

