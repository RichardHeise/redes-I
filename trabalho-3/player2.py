#!/usr/bin/env python3

import socket
import network
import game

UDP_IP = "localhost"
UDP_PORT_SEND = 6663
UDP_PORT_REC = 6662

receiver = (UDP_IP, UDP_PORT_SEND)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT_REC))

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

        data = network.create_package("player2", "player2", bet_value, bet, False, chips)
        network.send_msg(sock, data, receiver)

        print("Waiting other bets")

        data = network.receiv_msg(sock, receiver)
        # ++++++++++++++++++ Waiting other bets ++++++++++++++++++ 

        print("Alright. Let's see...")
        # whose play is it?
        if data["punter"] == "player2":

            # ++++++++++++++++++ Our play ++++++++++++++++++ 
            data = network.create_package("player2", "player2", data["bet_value"], data["bet"], False, chips)
            network.send_msg(sock, data, receiver)

            data = network.receiv_msg(sock, receiver)

            print("Betting on a " + game.bet_name(data["bet"]) + " with " + str(data["bet_value"]) + " chips!")
            print("++++ It's your play, make it good, chap ++++\n")

            # Let's roll
            if game.rollDices() == data["bet"]: # win
                print("You won the bet!")
                chips = chips + game.points(data["bet"])
                print("You now have " +str(chips)+ " chips ;)")
                data = network.create_package("player2", "player2", data["bet_value"], data["bet"], True, chips)
                network.send_msg(sock, data, receiver)

            else:
                print("You lost the bet, better luck next time...") # lose
                chips = chips - data["bet_value"]
                if chips <= 0:
                    print("You have no chips left, that means you lost the game :(")
                    data = network.create_package("player2", "player2", data["bet_value"], data["bet"], False, chips)
                    network.send_msg(sock, data, receiver)
                    exit(0)
                print("You now have " +str(chips)+ " chips ;)")
                data = network.create_package("player2", "player2", data["bet_value"], data["bet"], False, chips)
                network.send_msg(sock, data, receiver)
            # ++++++++++++++++++ Send our play through the net ++++++++++++++++++ 

        else:

            # ++++++++++++++++++ Their play ++++++++++++++++++
            print("Here they come! Betting on " +game.bet_name(data["bet"])+ " with " +str(data["chips"])+ " chips! Punter " +str(data["punter"])+ " is about to roll!")
            data = network.create_package("player2", data["punter"], data["bet_value"], data["bet"], False, chips)
            network.send_msg(sock, data, receiver)

            if data["punter"] != "player2":
                data = network.receiv_msg(sock, receiver) # Receives result of the play

                if data["won"] == True:
                    print("He did it! The chad won, nice roll " +str(data["punter"]))
                    print("Now " +str(data["punter"])+ " has " +str(data["chips"])+ " chips.")
                    network.send_msg(sock, data, receiver)
                else:
                    print("Uuh, " +str(data["punter"])+ " didn't got that one...")
                    network.send_msg(sock, data, receiver)
                    if data["chips"] <= 0:
                        print("Punter is broke!" +str(data["punter"])+ " just lost the game!")
                        print("That's all, folks!")
                        exit(0)
                # ++++++++++++++++++ Sends their play so everybody is in sync ++++++++++++++++++
                    
        # ++++++++++++++++++ Awaits for sync and sends baton to the next ++++++++++++++++++
        data = network.receiv_msg(sock, receiver)
        if data["origin"] == "player2":
            print("Alright, now it's player3's turn")
            data["baton"] = True
            baton = False
            network.send_msg(sock, data, receiver)
        print("------------- New round beginning, get set! -------------")
        # ++++++++++++++++++ Baton sent. New round begins ++++++++++++++++++
    else:

        # ++++++++++++++++++ Awaits a bet from the net ++++++++++++++++++
        data = network.receiv_msg(sock, receiver)

        print("Waiting bets...")
        print("The bet is a " + game.bet_name(data["bet"])+ " by " +str(data["punter"])+ ". " +str(data["bet_value"])+ " chips are being bet!")
        cover = input("Will you cover the bet?[yes/no]\n") # bet found, will we cover it?

        if cover == "yes":

            # ++++++++++++++++++ Covering bet ++++++++++++++++++
            print("Covering bet! Now we have " +str(data["bet_value"] + 1)+ " chips on the table!")
            data = network.create_package(data["origin"], "player2", int(data["bet_value"])+1, data["bet"], data["won"], data["chips"])
            network.send_msg(sock, data, receiver)
            print("Waiting on the dices")
            # ++++++++++++++++++ Waits to roll the dices ++++++++++++++++++

        else:

            # ++++++++++++++++++ Not covering it ++++++++++++++++++
            network.send_msg(sock, data, receiver)
            # ++++++++++++++++++ Just sends message forward ++++++++++++++++++

        data = network.receiv_msg(sock, receiver)  # Everyone decided covering or not

        # ++++++++++++++++++ Not covering it ++++++++++++++++++
        if data["punter"] == "player2":

            # ++++++++++++++++++ Our play ++++++++++++++++++ 
            print("Betting on a " + game.bet_name(data["bet"]) + " with " + str(data["bet_value"]) + " chips!")
            print("++++ It's your play, make it good, chap ++++\n")
            if game.rollDices() == data["bet"]:
                print("You won the bet!")
                chips = chips + game.points(data["bet"])
                print("You now have " +str(chips)+ " chips ;)")
                print("------------- New round beginning, get set! -------------")
                data = network.create_package(data["origin"], "player2", data["bet_value"], data["bet"], True, chips)
                network.send_msg(sock, data, receiver)

            else:
                print("You lost the bet, better luck next time...")
                chips = chips - data["bet_value"]
                if chips <= 0:
                    print("You have no chips left, that means you lost the game :(")
                    data = network.create_package(data["origin"], "player2", data["bet_value"], data["bet"], False, chips)
                    network.send_msg(sock, data, receiver)
                    exit(0)
                print("You now have " +str(chips)+ " chips ;)")
                print("------------- New round beginning, get set! -------------")
                data = network.create_package(data["origin"], "player2", data["bet_value"], data["bet"], False, chips)
                network.send_msg(sock, data, receiver)
                # ++++++++++++++++++ Send our play through the net ++++++++++++++++++

            data = network.receiv_msg(sock, receiver)
            network.send_msg(sock, data, receiver)
        else:

            # ++++++++++++++++++ Their play ++++++++++++++++++
            print(str(data["punter"])+ " is the punter! Waiting on the roll...")
            network.send_msg(sock, data, receiver)
        
            data = network.receiv_msg(sock, receiver)
            
            if data["won"] == True:
                print("The punter won the bet! They have " +str(data["chips"])+ " chips.")
                print("------------- New round beginning, get set! -------------")
                network.send_msg(sock, data, receiver)
            else:
                print("The punter lost the bet! They have " +str(data["chips"])+ " chips left.")
                network.send_msg(sock, data, receiver)
                if data["chips"] <= 0:
                    print("That's all, folks!")
                    exit(0)
                print("------------- New round beginning, get set! -------------")
            # ++++++++++++++++++ Sends play through the net ++++++++++++++++++

        # ++++++++++++++++++ Receives baton ++++++++++++++++++
        if data["origin"] == "player1":
            data = network.receiv_msg(sock, receiver)
            baton = data["baton"]
        # ++++++++++++++++++ New round ++++++++++++++++++
                