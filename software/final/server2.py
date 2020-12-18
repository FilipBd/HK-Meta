#!/usr/bin/env python
import socket
import serial
import re
import struct



localIP     = "127.0.0.1"

localPort   = 8888
bufferSize  = 1024
error = "nothing recieved"

msgFromServer       = "Hello UDP Client"

bytesToSend         = str.encode(msgFromServer)

RemoteIP = "169.254.170.171"

pi=3.14159
kp=289
ki=4436
fc = 1 #cutoff frequency
#upper=45*pi
upper=40*pi
lower=-upper
error_prev=0
ts_prev=0
u_prev=0
y_prev=0

# Create a datagram socket

#UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
#UDPServerSocket = socket.setsockopt(socket.SOL_SOCKET)
UDPServerSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
UDPServerSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

UDPclient =  socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
UDPclient.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

#arduino = serial.Serial('COM4',115200,timeout=.01)
print("UDP server up and listening")


# Listen for incoming datagrams

while(True):

    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize) #reading from LUA

    message = bytesAddressPair[0]

    address = bytesAddressPair[1]

    clientMsg = "Message from Client:{}".format(message)
    clientIP  = "Client IP Address:{}".format(address)

    print(clientMsg)
    #print(clientIP)
    message_str=message.decode("utf-8") #convert bytes to a string
    p=re.compile((r"[+-]?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?")) #compile a pattern to capture float values
    floats=[float(i) for i in p.findall(message_str)] #convert strings to floats
    ts=floats.pop(1)
    accelx=floats.pop(0) #r (reference)
    print("ts is :",ts)
    print("reference is: ",accelx)
    # recieve sensor data from client (odrive Arduino)

    #print("Arduino data: ",data)

    #data_str=data.decode("utf-8")
    #data_f=float(data_str)


    #error=accelx-data_f
    Ts = ts-ts_prev
    u = accelx * 1.9671 * 25 * 2 * pi
    a = (2*pi*Ts*fc)/(2*pi*Ts*fc+1)
    #y = u
    y = y_prev *(1-a) + a * u  #lowpass filter
    if(y > upper):
        y = upper
    if(y < lower):
        y = lower
    output = y/(2*pi)

    print("control signal is:", output)
    UDPclient.sendto(str.encode(str(output)), ('169.254.170.171',8888))
    #error_prev=error
    ts_prev=ts
    u_prev=u
    y_prev=y

"""
    if data:
        print("Arduino data: ",data)
        UDPclient.sendto(data, ('169.254.170.171',8888))
        UDPclient.sendto(message, ('169.254.170.171',8888))
    else:
        print(error)
        UDPclient.sendto(error.encode(), ('169.254.170.171',8888))
        UDPclient.sendto(message, ('169.254.170.171',8888))
"""
