from email import message
import random
import socket
import time

req="hello";
req_bytes=str.encode(req);

ip_add="192.168.43.84";
port=54321;
buffersize=1024;

num=random.randint(1,50);
prev_time=time.time();
curr_time=prev_time;
time_gap=5;

tcpsock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
tcpsock.bind((ip_add,port));
print("TCP Server up and listening for incoming connection");
client_add="";
tcpsock.listen(5);

conn,addr=tcpsock.accept();
print("Connection established");
print("\n");
print("IP Address of client: {}".format(addr));
print("\n");
while True and conn:
    curr_time=time.time();
    if(curr_time-prev_time>=time_gap):
        prev_time=curr_time;
        num=random.randint(1,50);
        print("New random number generated!");
    message=conn.recv(buffersize);
    # print(message);
    if(message==req_bytes):
        print("Sending Random number: {}".format(num));
        conn.send(str.encode(str(num)));


