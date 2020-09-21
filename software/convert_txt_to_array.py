import numpy as np
import socket

with open('test_acc_value.txt','r') as filestream:

	for line in filestream:
		currentline = line.split(',')
		print(currentline)



acc_value = np.array(currentline)

zero_array = np.zeros(124)

final_vector = np.append(acc_value,zero_array)

print(len(final_vector))



 

localIP     = "127.0.0.1"

localPort   = 20001

bufferSize  = 1024

# From manual, motion computer has IP 192.168.90.240. Can be changed. 
# Listening port 16385
# Send Port 16385
# Host computer can send from any port number





msgFromServer       = final_vector

#bytesToSend         = str.encode(msgFromServer)

 

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

 

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

 

print("UDP server up and listening")

 

# Listen for incoming datagrams

while(True):

    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)

    message = bytesAddressPair[0]

    address = bytesAddressPair[1]

    clientMsg = "Message from Client:{}".format(message)
    clientIP  = "Client IP Address:{}".format(address)
    
    print(clientMsg)
    print(clientIP)

   

    # Sending a reply to client

    UDPServerSocket.sendto(bytesToSend, address)