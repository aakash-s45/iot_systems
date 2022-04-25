#define ledpin LED_BUILTIN
double randNumber;
int timePeriod = 2000; 
int n = 10;
double delaytlow;
double delaythigh;
int debug=1;

void setup()
{
   Serial.begin(9600);
   pinMode(ledpin, OUTPUT);

   // if analog input pin 0 is unconnected, random analog
   // noise will cause the call to randomSeed() to generate
   // different seed numbers each time the sketch runs.
   // randomSeed() will then shuffle the random function.
   Serial.println("setup started");
   randomSeed(analogRead(0));
}

void loop()
{
   // print a random number from 0 to 9
   if (n != 0)
   {
      if(debug){
         Serial.print("current n: ");
         Serial.print(n);
         Serial.print(" current x: ");
         Serial.println(randNumber);
      }

      n -= 1;
      digitalWrite(ledpin, HIGH);
      delay(delaytlow);
      digitalWrite(ledpin, LOW);
      delay(delaytlow);
   }
   else
   {
      randNumber = random(10);
      Serial.print("Generated number: ");
      Serial.println(randNumber);
      n = 10;
      delaytlow = (timePeriod * randNumber) / 9;
      delaythigh = timePeriod * (1.0 - (randNumber / 9.0));
   }
}
