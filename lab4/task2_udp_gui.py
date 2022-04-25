from email import message
import random
import socket
import time
from ipaddress import ip_address
from threading import Thread

#######################################
#######################################
class Appliance:
    def __init__(self,pin):
        self.state=False;
        self.pin=pin;
    def getState(self):
        return str(self.state);
    def toggle(self):
        if self.state:
            self.state=False;
        else:
            self.state=True;
    def on(self):
        self.state=True;
    def off(self):
        self.state=False;
    

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
    isdebug=False;
    if(val<=24):
        if fan.getState()!=0:
            fan.toggle();
            if isdebug:
                print("Turned off fan");
        if ac.getState()==True:
            ac.toggle();
            if isdebug:
                print("Turned off ac");
    elif(val>24 and val<=32):
        if fan.getState()==0:
            fan.toggle();
            if isdebug:
                print("Turned on fan");
        if ac.getState()==True:
            ac.toggle();
            if isdebug:
                print("Turned off ac");
    else:
        if fan.getState()!=0:
            fan.toggle();
            if isdebug:
                print("Turned off fan");
        if ac.getState()==False:
            ac.toggle();
            if isdebug:
                print("Turned on ac");

#######################################
#######################################

req="hello";
req_bytes=str.encode(req);

ip_add="192.168.43.160";
port=54321;
buffersize=1024;

num=random.randint(1,50);
prev_time=time.time();
curr_time=prev_time;
time_gap=40;

light=Appliance(2);
ac=Appliance(3);
fan = Fan(5);

udpsock=socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
udpsock.bind((ip_add,port));
print("UDP Server up and running");
client_add="";

def updateAppliances(message):
    if message=='F 0':
        fan.setSpeed(0);
    elif message=='F 1':
        fan.setSpeed(1);
    elif message=='F 2':
        fan.setSpeed(2);
    elif message=='F 3':
        fan.setSpeed(3);
    elif message=='F 4':
        fan.setSpeed(4);
    elif message=='F 5':
        fan.setSpeed(5);

    elif message=='L 0':
        light.off();
    elif message=='L 1':
        light.on();
    
    elif message=='A 0':
        ac.off();
    elif message=='A 1':
        ac.on();

    print("Changed By Dashboard!")
    print("Fan :",fan.getState());
    print("AC :",ac.getState());
    print("Light :",light.getState());
    print()
    message="";
    

def ctrl():
    while True:
        global curr_time;
        global prev_time;
        global time_gap;
        global ac;
        global fan;
        global light;
        # generate the random number
        curr_time=time.time();
        if(curr_time-prev_time>=time_gap):
            prev_time=curr_time;
            num=random.randint(10,45);
            # print("New random number generated!");
            # control appliances using random number num
            control(num,fan,ac);
            print("Current Temperature :",num);
            print("Fan :",fan.getState());
            print("AC :",ac.getState());
            print("Light :",light.getState());
            print()
            global client_add;
            if(client_add!=""):
                # print()
                udpsock.sendto(str.encode(str(ac.state)),(client_add[0],54322))
                udpsock.sendto(str.encode(str(fan.state)),(client_add[0],54323))
                udpsock.sendto(str.encode(str(light.state)),(client_add[0],54324))

def serv():
    while True:
        global client_add;
        messageAddPair=udpsock.recvfrom(buffersize);
        message=messageAddPair[0];
        client_add=messageAddPair[1];
        message=str(message, 'UTF-8');
        updateAppliances(message);


t1=Thread(target=serv,args=());
t2=Thread(target=ctrl,args=());
t1.start();
t2.start();



