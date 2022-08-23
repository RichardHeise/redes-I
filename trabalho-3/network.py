#!/usr/bin/env python3

import json

INIT_MARKER = 126

def create_package(origin, punter, bet_value, bet, won, chips):
    return {
        "init" : INIT_MARKER,
        "origin" : origin,
        "punter" : punter,
        "bet_value" : bet_value,
        "bet" : bet,
        "won" : won,
        "chips" : chips,
        "baton" : False
    }

def send_msg(socket, data, receiver):
    # parity = 0

    # for i in range(0,len(json.dumps(data).encode("utf-8"))):
    #     parity = json.dumps(data).encode("utf-8")[i]^parity
        
    # data["parity"] = parity

    to_send = json.dumps(data).encode("utf-8")

    socket.sendto(to_send, receiver)

def receiv_msg(socket):
    while True:
        data, trash = socket.recvfrom(1024)

        data = json.loads(data)
        # parity = 0

        # for i in range(0,len(json.dumps(data).encode("utf-8"))):
        #     parity = json.dumps(data).encode("utf-8")[i]^parity

        # print(parity, data["parity"])
        # if parity != data["parity"]:
        #     print("Parity error")
        #     exit(0)
        
        if data["init"] == INIT_MARKER:
            return data