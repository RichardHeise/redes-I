#!/usr/bin/env python3

import json

INIT_MARKER = 126

def create_package(origin, punter, bet_value, bet, won, chips):
    # ++ Creating dict with relevant values ++
    return {
        "init" : INIT_MARKER, # initial message marker
        "origin" : origin,    # who holds the baton
        "punter" : punter,    # who will play
        "bet_value" : bet_value,  # how much chips were bet
        "bet" : bet, # which combination is the bet
        "won" : won, # did the punter won the bet?
        "chips" : chips, # chips of current player
        "baton" : False, # baton
        "parity" : 0  # Forced to zero
    }

def send_msg(socket, data, receiver):
    parity = 0

    for i in range(0,len(json.dumps(data).encode("utf-8"))):
        # Since data["parity"] is zero it has no effect on XOR
        parity = json.dumps(data).encode("utf-8")[i]^parity 
  
    data["parity"] = parity # Temporary measure

    to_send = json.dumps(data).encode("utf-8") # to bits, serialized

    socket.sendto(to_send, receiver)

def receiv_msg(socket, receiver):
    while True:
        # The 'trash' is the receiver of the tuple, useless here
        data, trash = socket.recvfrom(1024) 

        data = json.loads(data) # Tries to load
        if data["init"] == INIT_MARKER: # If valid we test parity

            rec_parity = data["parity"] # Original parity
            data["parity"] = 0 # Zero so it has no effect on the XOR
            parity = 0

            for i in range(0,len(json.dumps(data).encode("utf-8"))):
                # Since data["parity"] is zero it has no effect on XOR
                parity = json.dumps(data).encode("utf-8")[i]^parity

            if parity != rec_parity: 
                print("Parity error. Shuting down network.")
                send_msg(socket, data, receiver) # provoke parity error propagation
                exit(0)
            
            return data