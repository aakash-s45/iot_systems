#include "Arduino.h"


#ifndef items_h
#define items_h

class TV{
    int tvPin;

public:
    int tvState;
    TV(int);
    void toggle();
};



class Fan{
    int fanPin;

public:
    int fanState;
    Fan(int);
    void toggle();
};

class AC{
    int acPin;

public:
    int acState;
    AC(int);
    void toggle();
};
class Light{
    int redPin;//red
    int greenPin;//green
    int bluePin;//blue
    int lightTemp;//0 for warm, 1 for cool
    // warm (253, 244, 220)
    // cool (255, 255, 255)

public:
    int lightState; 
    Light(int ,int ,int );
    void warm();
    void cold();
    void toggle();
};

#endif
 