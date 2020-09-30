import socket
import numpy as np
UDP_IP = '169.254.170.171'
UDP_PORT = 8888
#MESSAGE = "Hello, World!"


print("UDP target IP: %s" % UDP_IP)
print("UDP target port: %s" % UDP_PORT)


sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP


for i in range(10):
    MESSAGE = str(np.random.randint(-1000,1000))
    sock.sendto(MESSAGE.encode(), (UDP_IP, UDP_PORT))
    print("message: %s" % MESSAGE)