from paho.mqtt import client as mqtt_client
import random
import time
import serial
import json


ser = serial.Serial(
        port='/dev/ttyACM0', 
        baudrate = 9600
)

broker = '192.168.43.84'
port = 1883
topicfrom1="xbee/from/node1"
topicfrom2="xbee/from/node2"
topicto1="xbee/to/node1"
topicto2="xbee/to/node2"

# topic_11 = "/xbee/node_1/from"
# topic_12 = "/xbee/node_1/to"
# topic_21 = "/xbee/node_2/from"
# topic_22 = "/xbee/node_2/to"
client_id = f'xbee_subnet'


print ("Starting program")

def on_message(client, userdata, msg):
        mqttRecv=msg.payload.decode();
        node=msg.topic[8:]
        ser.write(str.encode(mqttRecv));





def serialData(client):
    client.subscribe(topicto1)
    client.subscribe(topicto2)
    client.on_message = on_message

    recvMessage=""  
    while True:
        # read serial data and send it to mqtt
        try:
            data =ser.readline();
            data=data.decode();
            recvMessage=data.strip();

            jsonObj=json.loads(recvMessage)
            src=jsonObj["from"]
            # print(src)
            if src=="xbee1":
                result = client.publish(topicfrom1, recvMessage)
                status = result[0]
                if status==0:
                    print("sent to node1")
                elif(status!=0):
                    print(f"Failed to send message to topic {topicfrom1}")
            else:
                result = client.publish(topicfrom2, recvMessage)
                status = result[0]
                if status==0:
                    print("sent to node2")
                elif(status!=0):
                    print(f"Failed to send message to topic {topicfrom2}")
            recvMessage="";
        except:
            continue



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
    serialData(client)

if __name__ == '__main__':
    run()