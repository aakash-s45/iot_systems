from email import message
from ipaddress import ip_address
import socket
from time import sleep

req="hello";
req_bytes=str.encode(req);
ip_port=("192.168.43.133", 54321);
buffersize=1024;

udp_sock=socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM);
while 1:
    udp_sock.sendto(req_bytes,ip_port);
    print("-> ",end=" ");
    message=udp_sock.recvfrom(buffersize);
    print("Recieved number: {}".format(int(message[0])));
    sleep(3);
