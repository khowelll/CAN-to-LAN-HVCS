from socket import *
import time
import sys
from PyQt5.QtWidgets import (QWidget, QLabel, QLineEdit, QApplication)
#TODO Develop a GUI

address = ( '192.168.1.175', 5555)
#Must be the IP address of the Teensy
#Port 5555 is random

client_socket = socket(AF_INET, SOCK_DGRAM) #Set up the socket
client_socket.settimeout(1) #Only wait 1 second for a response


while(1): #main loop

    CanMsg = "18FE0100" #Message to be sent, using strings right now
    client_socket.sendto(bytes(CanMsg.encode('utf-8')), address) # send message to Teensy
    try:
        rec_data, addr = client_socket.recvfrom(2048) #read response from Teensy
        print(rec_data) #print response form Teensy
    except:
        pass

    time.sleep(1) #delay before looping again

