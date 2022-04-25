from email import message
from ipaddress import ip_address
import socket
from time import sleep
import appliances
#######################################
#######################################
class Appliance:
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

class Fan:
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

def control(val,fan,ac):
    if(val<=24):
        if fan.getState()!=0:
            fan.toggle();
            print("Turned off fan");
        if ac.getState()!=0:
            ac.toggle();
            print("Turned off ac");
    elif(val>24 and val<=32):
        if fan.getState()==0:
            fan.toggle();
            print("Turned on fan");
        if ac.getState()!=0:
            ac.toggle();
            print("Turned off ac");
    else:
        if fan.getState()!=0:
            fan.toggle();
            print("Turned off fan");
        if ac.getState()==0:
            ac.toggle();
            print("Turned on ac");


#######################################
#######################################

req="hello";
req_bytes=str.encode(req);
ip_port=("192.168.43.84", 20001);
buffersize=1024;

light=Appliance(2);
ac=Appliance(3);
fan = Fan(5);



print("UDP Client");
udp_sock=socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM);


while 1:
    # messageAddPair2=udp_sock2.recvfrom(buffersize);
    # print("message from node red");
    # print(messageAddPair2[0]);
    udp_sock.sendto(req_bytes,ip_port);
    print("-> ",end=" ");
    message=udp_sock.recvfrom(buffersize);
    val=int(message[0]);
    print("Recieved number: {}".format(val));
    print("------------------------------");
    control(val,fan,ac);
    sleep(3);
    print("");
