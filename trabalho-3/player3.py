#!/usr/bin/env python3

import socket
import network
import game

UDP_IP = "localhost"
UDP_PORT_SEND = 6664
UDP_PORT_REC = 6663

receiver = (UDP_IP, UDP_PORT_SEND)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT_REC))

baton = False
chips = 5

print("All set? Let's play.")
print("You have " + str(chips) + " chips")
while(True):
    if baton == True:
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

        data = network.create_package("player3", "player3", bet_value, bet, False, chips)
        network.send_msg(sock, data, receiver)
        print("Waiting other bets")

        data = network.receiv_msg(sock)
        print("Alright. Let's see...")

        if data["punter"] == "player3":
            data = network.create_package("player3", "player3", data["bet_value"], data["bet"], False, chips)
            network.send_msg(sock, data, receiver)

            data= network.receiv_msg(sock)

            print("Betting on a " + game.bet_name(data["bet"]) + " with " + str(data["bet_value"]) + " chips!")
            print("++++ It's your play, make it good, chap ++++\n")
            if game.rollDices() == data["bet"]:
                print("You won the bet!")
                chips = chips + game.points(data["bet"])
                print("You now have " +str(chips)+ " chips ;)")
                data = network.create_package("player3", "player3", data["bet_value"], data["bet"], True, chips)
                network.send_msg(sock, data, receiver)

            else:
                print("You lost the bet, better luck next time...")
                chips = chips - data["bet_value"]
                if chips <= 0:
                    print("You have no chips left, that means you lost the game :(")
                    data = network.create_package("player3", "player3", data["bet_value"], data["bet"], False, chips)
                    network.send_msg(sock, data, receiver)
                    exit(0)
                print("You now have " +str(chips)+ " chips ;)")
                data = network.create_package("player3", "player3", data["bet_value"], data["bet"], False, chips)
                network.send_msg(sock, data, receiver)
        else:
            print("Here they come! Betting on " +game.bet_name(data["bet"])+ " with " +str(data["chips"])+ " chips! Punter " +str(data["punter"])+ " is about to roll!")
            data = network.create_package("player3", data["punter"], data["bet_value"], data["bet"], False, chips)
            network.send_msg(sock, data, receiver)

            if data["punter"] != "player3":
                data = network.receiv_msg(sock)

                if data["won"] == True:
                    print("He did it! The chad won, nice roll " +str(data["punter"]))
                    print("Now " +str(data["punter"])+ " has " +str(data["chips"])+ " chips.")
                    network.send_msg(sock, data, receiver)
                else:
                    print("Uuh, " +str(data["punter"])+ " didn't got that one...")
                    if data["chips"] <= 0:
                        print("Punter is broke!" +str(data["punter"])+ " just lost the game!")
                        print("That's all, folks!")
                        exit(0)
                    network.send_msg(sock, data, receiver)
        
        data = network.receiv_msg(sock)
        if data["origin"] == "player3":
            print("Alright, now it's player2's turn")
            # data["baton"] = True
            baton = False
            # network.send_msg(sock, data, receiver)
        print("------------- New round beginning, get set! -------------")
    else:
        data = network.receiv_msg(sock)

        if data["origin"] != "player3":
            print("Waiting bets...")
            print("The bet is a " + game.bet_name(data["bet"])+ " by " +str(data["punter"])+ ". " +str(data["bet_value"])+ " chips are being bet!")
            cover = input("Will you cover the bet?[yes/no]\n")
            if cover == "yes":
                print("Covering bet! Now we have " +str(data["bet_value"] + 1)+ " chips on the table!")
                data = network.create_package(data["origin"], "player3", int(data["bet_value"])+1, data["bet"], data["won"], data["chips"])
                network.send_msg(sock, data, receiver)
                print("Waiting on the dices")
            else:
                network.send_msg(sock, data, receiver)

            data = network.receiv_msg(sock)

            if data["punter"] == "player3":

                print("Betting on a " + game.bet_name(data["bet"]) + " with " + str(data["bet_value"]) + " chips!")
                print("++++ It's your play, make it good, chap ++++\n")
                if game.rollDices() == data["bet"]:
                    print("You won the bet!")
                    chips = chips + game.points(data["bet"])
                    print("You now have " +str(chips)+ " chips ;)")
                    data = network.create_package(data["origin"], "player3", data["bet_value"], data["bet"], True, chips)
                    network.send_msg(sock, data, receiver)

                else:
                    print("You lost the bet, better luck next time...")
                    chips = chips - data["bet_value"]
                    if chips <= 0:
                        print("You have no chips left, that means you lost the game :(")
                        data = network.create_package(data["origin"], "player3", data["bet_value"], data["bet"], False, chips)
                        network.send_msg(sock, data, receiver)
                        exit(0)
                    print("You now have " +str(chips)+ " chips ;)")
                    data = network.create_package(data["origin"], "player3", data["bet_value"], data["bet"], False, chips)
                    network.send_msg(sock, data, receiver)
                data = network.receiv_msg(sock)
                network.send_msg(sock, data, receiver)
            else:
                print(str(data["punter"])+ " is the punter! Waiting on the roll...")
                network.send_msg(sock, data, receiver)
            
                data = network.receiv_msg(sock)
                
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

            if data["origin"] == "player2":
                baton = True

            

