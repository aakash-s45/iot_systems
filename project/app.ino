class Fan
{
    int pin;
    int speed;
    int debug;

public:
    Fan(int pin)
    {
        this->pin = pin;
        debug = 0;
        speed = 0;
    }
    void setSpeed(int speed)
    {
        if (speed >= 0 && speed < 6)
            this->speed = speed;
    }
    int getSpeed()
    {
        return this->speed;
    }
    void off()
    {
        this->speed = 0;
    }
};

class AC
{
    int pin;
    int mode; // 0->cool,1->fan only,2->hot
    int debug;
    int state;
    int temp;
    AC(int pin)
    {
        this->pin = pin;
        debug = 0;
        mode = 0;
        state = 0;
        temp = 24;
    }
    void setMode(int mode)
    {
        if (mode < 3 && mode >= 0)
            this->mode = mode;
    }
    int getMode()
    {
        return this->mode;
    }
    void toggleMode()
    {
        this->mode = (this->mode + 1) % 3;
    }
    void off()
    {
        this->state = 0;
    }
    void on()
    {
        this->state = 1;
    }
    int state()
    {
        return this->state;
    }
    void setTemp(int temp)
    {
        this->temp = temp;
    }
    int getTemp()
    {
        return this->temp;
    }
};