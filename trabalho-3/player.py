#!/usr/bin/env python3

import socket
import network
import game
import sys

id = int(sys.argv[1])
to_id = (id % 4 + 1)

UDP_IP = "localhost"
UDP_PORT_SEND = (666*10 + to_id)
UDP_PORT_REC = (666*10 + id)

receiver = (UDP_IP, UDP_PORT_SEND)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT_REC))

if id == 1:
    baton = True
else:
    baton = False
chips = 5

print("All set? Let's play.")
print("You have " + str(chips) + " chips")

while(True):
    if baton == True:

        # ++++++++++++++++++ Creating a bet ++++++++++++++++++ 
        print("Your turn, place a bet")
        print("""
                1-PAIR
                2-THREE OF A KIND
                3-TWO PAIRS
                4-FULL-HOUSE
                5-SMALL STRAIGHT
                6-LARGE STRAIGHT
                7-FOUR OF A KIND
                8-YAHTZEE
                """)
                
        bet = int(input("What say you? (bet 1-8)\n"))
        bet_value = int(input("How much will you bet? The minimum is 1\n"))

        data = network.create_package(id, id, bet_value, bet, False, chips) # creates bet
        network.send_msg(sock, data, receiver) # send it through the net

        print("Waiting other bets")

        data = network.receiv_msg(sock, receiver) 
        # ++++++++++++++++++ Waiting other bets ++++++++++++++++++ 

        print("Alright. Let's see...")
        # whose play is it?
        if data["punter"] == id:

            # ++++++++++++++++++ Our play ++++++++++++++++++ 

            print("Betting on a " + game.bet_name(data["bet"]) + " with " + str(data["bet_value"]) + " chips!")
            print("++++ It's your play, make it good, chap ++++\n")

            network.send_msg(sock, data, receiver)
            data = network.receiv_msg(sock, receiver)

            # Let's roll
            if game.rollDices() == data["bet"]: # win
                print("You won the bet!")
                chips = chips + game.points(data["bet"])
                print("You now have " +str(chips)+ " chips ;)")
                data = network.create_package(id, id, data["bet_value"], data["bet"], True, chips)
                # ++++++++++++++++++ Create our play ++++++++++++++++++ 


            else:
                print("You lost the bet, better luck next time...") 
                chips = chips - data["bet_value"]
                if chips <= 0:
                    print("You have no chips left, that means you lost the game :(")
                    data = network.create_package(id, id, data["bet_value"], data["bet"], False, chips)
                    network.send_msg(sock, data, receiver)
                    exit(0)
                print("You now have " +str(chips)+ " chips ;)")
                data = network.create_package(id, id, data["bet_value"], data["bet"], False, chips)
                # ++++++++++++++++++ Create our play ++++++++++++++++++ 

        else:

            # ++++++++++++++++++ Their play ++++++++++++++++++
            print("Here they come! Betting on " +game.bet_name(data["bet"])+ " with " +str(data["chips"])+ " chips! Punter " +str(data["punter"])+ " is about to roll!")
            network.send_msg(sock, data, receiver) # Tell the net it's not our play

            data = network.receiv_msg(sock, receiver) # Receives result of the play
            
            if data["won"] == True:
                print("He did it! The chad won, nice roll " +str(data["punter"]))
                print("Now " +str(data["punter"])+ " has " +str(data["chips"])+ " chips.")
            else:
                print("Uuh, " +str(data["punter"])+ " didn't got that one...")
                if data["chips"] <= 0:
                    print("Punter is broke!" +str(data["punter"])+ " just lost the game!")
                    print("That's all, folks!")
                    network.send_msg(sock, data, receiver)
                    exit(0)
                    
        # ++++++++++++++++++ Awaits for sync and sends baton to the next ++++++++++++++++++

        network.send_msg(sock, data, receiver) # Sends play
        data = network.receiv_msg(sock, receiver) # Receives confirmation of sent
        if data["origin"] == id:
            print("Alright, now it's player " +str(to_id)+ " turn")
            data["baton"] = True
            baton = False
            network.send_msg(sock, data, receiver) # pass the baton
        # ++++++++++++++++++ Baton sent. New round begins ++++++++++++++++++
    else:

        print("------------- New round beginning, get set! -------------")
        # ++++++++++++++++++ Awaits a bet from the net ++++++++++++++++++
        data = network.receiv_msg(sock, receiver)

        print("Waiting bets...")
        print("The bet is a " + game.bet_name(data["bet"])+ " by " +str(data["punter"])+ ". " +str(data["bet_value"])+ " chips are being bet!")
        cover = input("Will you cover the bet?[yes/no]\n") # bet found, will we cover it?

        if cover == "yes":

            # ++++++++++++++++++ Covering bet ++++++++++++++++++
            print("Covering bet! Now we have " +str(data["bet_value"] + 1)+ " chips on the table!")
            data = network.create_package(data["origin"], id, int(data["bet_value"])+1, data["bet"], data["won"], data["chips"])
            network.send_msg(sock, data, receiver)
            print("Waiting on the dices")
            # ++++++++++++++++++ Waits to roll the dices ++++++++++++++++++

        else:

            # ++++++++++++++++++ Not covering it ++++++++++++++++++
            network.send_msg(sock, data, receiver)
            # ++++++++++++++++++ Just sends message forward ++++++++++++++++++

        data = network.receiv_msg(sock, receiver)  # Everyone decided covering or not

        # ++++++++++++++++++ Covering it ++++++++++++++++++
        if data["punter"] == id:

            # ++++++++++++++++++ Our play ++++++++++++++++++ 
            print("Betting on a " + game.bet_name(data["bet"]) + " with " + str(data["bet_value"]) + " chips!")
            print("++++ It's your play, make it good, chap ++++\n")
            if game.rollDices() == data["bet"]:
                print(data["bet"], game.rollDices() )
                print("You won the bet.")
                chips = chips + game.points(data["bet"])
                data = network.create_package(data["origin"], id, data["bet_value"], data["bet"], True, chips)
                network.send_msg(sock, data, receiver)
                # ++++++++++++++++++ Sends our play ++++++++++++++++++


            else:
                print("You lost the bet.")
                chips = chips - data["bet_value"]
                data = network.create_package(data["origin"], id, data["bet_value"], data["bet"], False, chips)
                network.send_msg(sock, data, receiver)
                if data["chips"] <= 0:
                    print("That's all, folks!")
                    exit(0)
                # ++++++++++++++++++ Send our play  ++++++++++++++++++

            data = network.receiv_msg(sock, receiver) # Wait to sync since the net doesn't know the result yet

        else:
            
            # ++++++++++++++++++ Their play ++++++++++++++++++
            print(str(data["punter"])+ " is the punter! Waiting on the roll...")
            network.send_msg(sock, data, receiver)

            data = network.receiv_msg(sock, receiver) # waits for the result
            
            if data["won"] == True:
                print("The punter won the bet! They have " +str(data["chips"])+ " chips.")
            else:
                print("The punter lost the bet! They have " +str(data["chips"])+ " chips left.")
                if data["chips"] <= 0:
                    print("That's all, folks!")
                    network.send_msg(sock, data, receiver)
                    exit(0)

        # ++++++++++++++++++ Sends end of play ++++++++++++++++++
        network.send_msg(sock, data, receiver)

        if id == 1:
            prev = 4
        else:
            prev = id - 1


        # ++++++++++++++++++ If we can, we get the baton +++++++++++++++++
        if data["origin"] == prev:
            data = network.receiv_msg(sock, receiver)
            baton = data["baton"]
        # ++++++++++++++++++ New round ++++++++++++++++++
