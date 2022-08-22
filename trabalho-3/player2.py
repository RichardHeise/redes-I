#!/usr/bin/env python3

import socket
import network

UDP_IP = "localhost"
UDP_PORT_SEND = 6661
UDP_PORT_REC = 6662

receiver = (UDP_IP, UDP_PORT_SEND)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT_REC))

baton = False
chips = 5

data = network.receiv_msg(sock)
network.send_msg(sock, data, receiver)