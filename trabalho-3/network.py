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
        "chips" : chips
    }

def send_msg(socket, data, receiver):
    socket.sendto(json.dumps(data).encode("utf-8"), receiver)

def receiv_msg(socket):
    while True:
        data, trash = socket.recvfrom(1000)
        data = json.loads(data)
        if data["init"] == INIT_MARKER:
            return data