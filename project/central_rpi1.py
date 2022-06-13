from paho.mqtt import client as mqtt_client
import random
import time
import json
import datetime;
  
# ct stores current time
#  for main rpi

broker = '192.168.43.84'
port = 1883

# f = open("/home/pi/Desktop/project/log.txt",'a',encoding = 'utf-8');
topic_wifi_to1="wifi/to/node1"
topic_wifi_to2="wifi/to/node2"
topic_wifi_to3="wifi/to/node3"
topic_wifi_to4="wifi/to/node4"

topic_wifi_from1="wifi/from/node1"
topic_wifi_from2="wifi/from/node2"
topic_wifi_from3="wifi/from/node3"
topic_wifi_from4="wifi/from/node4"

topic_ble_to1="ble/to/node1"
topic_ble_to2="ble/to/node2"

topic_ble_from1="ble/from/node1"
topic_ble_from2="ble/from/node2"

topic_xbee_to1="xbee/to/node1"
topic_xbee_to2="xbee/to/node2"

topic_xbee_from1="xbee/from/node1"
topic_xbee_from2="xbee/from/node2"

client_id = f'rpi1'


print ("Starting program")
def getSender(topic):
    if topic==topic_ble_from1:
        return "ble_1"
    elif topic ==topic_ble_from2:
        return "ble_2"
    elif topic ==topic_wifi_from1:
        return "wifi1"
    elif topic ==topic_wifi_from2:
        return "wifi2"
    elif topic ==topic_wifi_from3:
        return "wifi3"
    elif topic ==topic_wifi_from4:
        return "wifi4"
    elif topic ==topic_xbee_from1:
        return "xbee1"
    elif topic ==topic_xbee_from2:
        return "xbee2"
def getReciever(name):

    if name == "wifi1":
        return topic_wifi_to1
    elif name =="wifi2":
        return topic_wifi_to2
    elif name =="wifi3":
        return topic_wifi_to3
    elif name =="wifi4":
        return topic_wifi_to4

    elif name=="xbee1":
        return topic_xbee_to1
    elif name=="xbee2":
        return topic_xbee_to2
    elif name=="ble_1":
        return topic_ble_to1
    elif name=="ble_2":
        return topic_ble_to2


def on_message(client, userdata, msg):
    try:
        messageRecv=msg.payload.decode();
        ct = datetime.datetime.now();
        log=str(ct)+" : "+messageRecv+"\n";
        # print(log);
        f = open("/home/pi/Desktop/project/log.txt",'a',encoding = 'utf-8');
        f.write(log);
        f.close();
        jsonObj=json.loads(messageRecv)
        recvTopic=getReciever(jsonObj["to"]);
        if recvTopic!=None:
            result = client.publish(recvTopic, messageRecv)
            status = result[0]

            if status == 0:
                print(f"Sent msg to topic `{recvTopic}`")
            if(status!=0):
                print(f"Failed to send message to topic {recvTopic}")
    except:
        print("exception occured")

        


def fun(client):
    client.subscribe(topic_wifi_from1)
    client.subscribe(topic_wifi_from2)
    client.subscribe(topic_wifi_from3)
    client.subscribe(topic_wifi_from4)

    client.subscribe(topic_ble_from1)
    client.subscribe(topic_ble_from2)

    client.subscribe(topic_xbee_from1)
    client.subscribe(topic_xbee_from2)
    client.on_message = on_message
    while True:
        a=1;
        
    # print(msg.topic)
    # src=getSender(msg.topic);


    # recvMessage=""
    # while True:
        # recvMessage=recvMessage.strip();
        # senderNode=recvMessage[9:15]
        # if(senderNode=="xbee_1"):
        #     # do something
        #     result = client.publish(topic_11, recvMessage)
        #     # if status == 0:
        #         # print(f"Sent msg to topic `{topic_11}`")
        #     if(status!=0):
        #         print(f"Failed to send message to topic {topic_11}")
        # elif(senderNode=="xbee_2"):
        #     # do something
        #     result = client.publish(topic_21, recvMessage)
        #     status = result[0]
        #     # if status == 0:
        #         # print(f"Sent msg to topic `{topic_21}`")
        #     if(status!=0):
        #         print(f"Failed to send message to topic {topic_21}"
        # recvMessage="";



def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)
    # Set Connecting Client ID
    client = mqtt_client.Client(client_id)
    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def run():
    client = connect_mqtt()
    client.loop_start()
    fun(client)

if __name__ == '__main__':
    run()