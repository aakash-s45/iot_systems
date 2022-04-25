from email import message
import random
import socket
import time

req="hello";
req_bytes=str.encode(req);

ip_add="192.168.43.84";
port=20001;
buffersize=1024;

num=random.randint(1,50);
prev_time=time.time();
curr_time=prev_time;
time_gap=5;

udpsock=socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
udpsock.bind((ip_add,port));
print("UDP Server up and running");
client_add="";

while True:
    curr_time=time.time();
    if(curr_time-prev_time>=time_gap):
        prev_time=curr_time;
        num=random.randint(1,50);
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


