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
        num=urandom.getrandbits(5);
        print("New random number generated!");
    message=conn.recv(buffersize);
    # print(message);
    if(message==req_bytes):
        print("Sending Random number: {}".format(num));
        conn.send(str.encode(str(num)));


