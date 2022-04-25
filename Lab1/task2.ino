#include <dht.h>
#include <items.h>
#define dht_apin A0
dht DHT;

// Here we have a counter using two ir sensors
// When a person enters, count increment by 1 
// When a person goes out, count decrement by 1 

// when count becomes zero => fan,lights will be turned off
// and a timer for 5 mins will start if tv is on
// after 5 mins tv will be turned off

// when first person comes in, light1 will be on and 
// fan will be on if temperature is  greater than >27

// ac will be on if temperature >35
// ac and fan will be turned off if temperature <24

// if one light is on then it will change its color to warm white
// if both lights are on then it will change its color to white(cold)
// all the things can be conttrolled remotely, code for this feature is not yet written in this file




int irpin1 = 7;
int irpin2 = 8;
int count = 0;
int temp=0;
int temperature=0;

// declaring and initializing 
TV tv(2);
Fan fan(3);
AC ac(6);
Light light1(4);
Light light2(5);

int interval=300000;
int prevtime=0;
int currenttime=0;
int flag=0;


int count=0;
void setup(){
    serial.begin(9600);
    TV tv()
    pinmode(irpin1, INPUT);
    pinmode(irpin2, INPUT);
    randomseed(A0);
}
void loop(){
    // counter system
    if(digitalRead(irpin1) && temp==0){
        temp+=1;TV
        Serial.print("Sensor 1 On");
    }
    else if(digitalRead(irpin1) && temp==-1){
        temp++;
        count--;
        Serial.println("Person Out");
        Serial.print("Sensor 1 On, ");
        Serial.print("Count: ");
        Serial.println(count);
    }
    if(digitalRead(irpin2) && temp==1){
        temp--;
        count++;
        Serial.println("Person In");
        Serial.print("Sensor 2 On, ");
        Serial.print("Count: ");
        Serial.println(count);
    }
    else if(digitalRead(irpin2) && temp==0){
        temp--;
        Serial.print("Sensor 2 On");
    }

    // reading dht temperature
    // DHT.read11(dht_apin);
    temperature=random(20,50);
    // checking current timer when no one in room
    if(counter==0)currenttime=millis();
    // timer started
    if(count==0 && tv.tvState && flag==0){
        Serial.println("Timer for 5min started ");
        prevtime=currenttime;
        flag=1;
    }
    // turn off tv if 5 min elased after count =0
    if(flag && (currenttime-prevtime)>=interval){
        Serial.println("TV is turn off due to empty room");
        tv.toggle();  
    }
    if(count==0){
        if(light1.lightState){
            light1.toggle();
            Serial.println("Turning off Light 1, count=0");
        }
        if(light2.lightState){
            light2.toggle();
            Serial.println("Turning off Light 2, count=0");
        }
        if(fan.fanState){
            fan.toggle();
            Serial.println("Turning off Fan, count=0");
        }
    }
    if(count==1){
        if(!light1.lightState){
            light1.toggle();
            Serial.println("Turning on Light 1");
        }
        if(!fan.fanState && temperature >=28 ){
            fan.toggle();
            Serial.println("Turning on Fan");
        }
    }
    if(light1.lightState && light2.lightState){
        light1.cold();
        light2.cold();
        Serial.println("Both light's temperature is Cold");
    }
    if(light1.lightState && !light2.lightState){
        light1.warm();
        Serial.println("light 1's temperature is Warm");
    }
    if(!light1.lightState && light2.lightState){
        light2.warm();
        Serial.println("light 2's temperature is Warm");
    }
    if(temperature<24){
        if(fan.fanState){
            fan.toggle();
            Serial.println("Fan off");
        }
        if(ac.acState){
            ac.toggle();
            Serial.println("AC off");
        }
    }
    if(temperature>=28){
        if(!fan.fanState){
            fan.toggle();
            Serial.println("Fan on");
        }
    }
    if(temperature>35){
        if(fan.fanState){
            fan.toggle();
            Serial.println("Fan off");
        }
        if(!ac.acState){
            ac.toggle();
            Serial.println("AC on");
        }
    }
}


// tv, 2light, fan, ac,count, timer,temp
// tv->on or off
// fan->on off
// ac-> on off
// count (1 -1 0)
// light 4 warm cool off
