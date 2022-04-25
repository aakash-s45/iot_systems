from email import message
from ipaddress import ip_address
import socket
from time import sleep

req="hello";
req_bytes=str.encode(req);
ip_port=("192.168.43.84", 54321);
buffersize=1024;

tcp_sock=socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM);
tcp_sock.connect(ip_port);
while 1:
    tcp_sock.send(req_bytes);
    # tcp_sock.sendto(req_bytes,ip_port);
    print("-> ",end=" ");
    message=tcp_sock.recv(buffersize);
    print("Recieved number: {}".format(message));
    sleep(3);
