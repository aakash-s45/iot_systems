/*
Code for random number generator for nodemcu
*/

class Generator{

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
    Generator(int pin,int st,int ed);
    void run();
    int val();
};

Generator::Generator(int ledpin,int st,int ed){
    _ledpin = ledpin;
    _start=st;
    _end=ed;
    diff=ed-st;
    pinMode(_ledpin, OUTPUT);
    randomSeed(analogRead(A0));
    timePeriod = 1000;
    n=10;
    debug=0;
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
      randNumber = random(_start,_end);
      if(debug){
        Serial.print("Generated number: ");
        Serial.println(randNumber);
      }
      n = 10;
      delaytlow = (timePeriod * randNumber) / diff;
      delaythigh = timePeriod * (1.0 - (randNumber / diff));
   }
}
int Generator::val(){
    return this->randNumber;
}

Generator light(13,1,100);//range ->(1,100)
Generator temp(3,15,50);  //range ->(15,50)
Generator humid(2,20,100);//range ->(20,100)

void setup() {
    Serial.begin(9600);
}

void loop() {
    light.run();
    temp.run();
    humid.run();
    Serial.print("T:");
    Serial.print(temp.val());
    
    Serial.print(", H:");
    Serial.print(humid.val());
    
    Serial.print(", L:");
    Serial.println(light.val());

}