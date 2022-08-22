#!/usr/bin/env python3

import socket

UDP_IP = "localhost"
UDP_PORT_SEND = "6664"
UDP_PORT_REC = "6663"

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT_REC))

baton = False
chips = 5