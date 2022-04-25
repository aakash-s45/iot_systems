class appliance:
    def __init__(self,pin):
        self.state=False;
        self.pin=pin;
    def getState(self):
        return self.state;
    def toggle(self):
        if self.state:
            self.state=False;
        else:
            self.state=True;

class fan:
    def __init__(self,pin):
        self.state=0;
        self.pin=pin;
    def getState(self):
        return self.state;
    def toggle(self):
        if not self.state == 0:
            self.state=0;
        else:
            self.state=2;
    def setSpeed(self,speed):
        self.state=speed;
