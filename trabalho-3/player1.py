#!/usr/bin/env python3

import socket
import network
import game

UDP_IP = "localhost"
UDP_PORT_SEND = 6662
UDP_PORT_REC = 6661

receiver = (UDP_IP, UDP_PORT_SEND)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT_REC))

baton = True
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

        data = network.create_package("player1", "player1", bet_value, bet)
        network.send_msg(sock, data, receiver)
        print("Waiting other bets")

        data = network.receiv_msg(sock)
        print("Alright. Let's see...")

        if data["punter"] == "player1":
            print("++++ It's your play, make it good, chap ++++\n")
            if game.rollDices() == data["bet"]:
                print("You won the bet!")
                chips = chips + game.points(data["bet"])
                print("You now have " +str(chips)+ " chips ;)")

            else:
                print("You lost the bet, better luck next time...")
                chips = chips - data["bet_value"]
                if chips <= 0:
                    print("You have no chips left, that means you lost the game :(")
                    exit(0)
                print("You now have " +str(chips)+ " chips ;)")