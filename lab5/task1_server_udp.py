import network
import urandom
import socket
import time

wlan = network.WLAN(network.STA_IF) # create station interface
wlan.active(True)       # activate the interface
wlan.scan()             # scan for access points
wlan.isconnected()      # check if the station is connected to an AP
wlan.connect('OnePlus 7', 'wifi1234@Password.com') # connect to an AP
wlan.config('mac')      # get the interface's MAC address
wlan.ifconfig()


req="hello";
req_bytes=str.encode(req);

ip_add="192.168.43.133";
port=54321;
buffersize=1024;

num=urandom.getrandbits(5);
prev_time=time.time();
curr_time=prev_time;
time_gap=5;

udpsock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
udpsock.bind((ip_add,port));
print("UDP Server up and running");
client_add="";

while True:
    curr_time=time.time();
    if(curr_time-prev_time>=time_gap):
        prev_time=curr_time;
        num=urandom.getrandbits(5);
        print("New random number generated!");
    messageAddPair=udpsock.recvfrom(buffersize);
    message=messageAddPair[0];
    client_add=messageAddPair[1];
    # print("IP Address: {}".format(messageAddPair[1]));
    # print(message);
    # udpsock.sendto(str.encode(str(num)),client_add);
    if(message==req_bytes):
        print("Sending Random number: {}".format(num));
        udpsock.sendto(str.encode(str(num)),messageAddPair[1]);



